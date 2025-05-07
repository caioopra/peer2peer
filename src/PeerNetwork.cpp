#include "../include/PeerNetwork.h"

#include <cstring>
#include <fstream>
#include <iostream>

#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#define IMAGE_FOLDER "images/"

PeerNetwork::PeerNetwork(int port) : _port(port) {
  peer_list = {
      {"127.0.0.1", 9001},
      {"127.0.0.1", 9002},
      {"127.0.0.1", 9003},
      {"127.0.0.1", 9004},
  };
}

void PeerNetwork::start_server() {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(_port);

  bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr));
  listen(server_socket, 5);

  std::cout << "[PEER_NETWORK] Server started on port " << _port << std::endl;

  while (true) {
    int client_socket = accept(server_socket, nullptr, nullptr);
    std::thread(&PeerNetwork::handle_client, this, client_socket).detach();
  }
}

void PeerNetwork::send_hello_and_receive_peers() {
  PeerSet discovered;

  std::lock_guard<std::mutex> lock(peer_mutex);

  for (const auto &[ip, port] : peer_list) {
    if (port == _port)
      continue;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in peer_addr{};
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &peer_addr.sin_addr);

    if (connect(sock, (sockaddr *)&peer_addr, sizeof(peer_addr)) == 0) {
      _connect_to_peer(sock, &discovered);
    }

    close(sock);
  }

  for (const auto &p : discovered) {
    if (std::find(peer_list.begin(), peer_list.end(), p) == peer_list.end()) {
      peer_list.push_back(p);
    }
  }
}

void PeerNetwork::_connect_to_peer(int sock, PeerSet *discovered) {
  send(sock, "HELLO", 5, 0);

  char buffer[BUFFER_SIZE] = {0};
  recv(sock, buffer, BUFFER_SIZE, 0);
  std::string response(buffer);

  if (response.find("PEERS:") == 0) {
    std::string peer_data = response.substr(7);

    size_t start = 0, end;
    while ((end = peer_data.find(' ', start)) != std::string::npos) {
      std::string peer_entry = peer_data.substr(start, end - start);

      size_t sep = peer_entry.find(':');
      std::string ip = peer_entry.substr(0, sep);
      int port = std::stoi(peer_entry.substr(sep + 1));

      discovered->insert({ip, port});
      start = end + 1;
    }

    if (start < peer_data.size()) {
      std::string peer_entry = peer_data.substr(start);
      size_t sep = peer_entry.find(':');
      std::string ip = peer_entry.substr(0, sep);
      int port = std::stoi(peer_entry.substr(sep + 1));

      discovered->insert({ip, port});
    }
  }
}

void PeerNetwork::request_image(const std::string &image_name) {
  std::string request = "REQ: " + image_name;

  for (const auto &[ip, port] : peer_list) {
    if (port == _port)
      continue;

    if (_connect_and_request(ip, port, request, image_name)) {
      return;
    }
  }

  std::cout << "[PEER_NETWORK] Image '" << image_name
            << "' not found on any peer." << std::endl;
}

bool PeerNetwork::_connect_and_request(const std::string &ip, int port,
                                       const std::string &request,
                                       const std::string &image_name) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in peer_addr{};
  peer_addr.sin_family = AF_INET;
  peer_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &peer_addr.sin_addr);

  if (connect(sock, (sockaddr *)&peer_addr, sizeof(peer_addr)) == 0) {
    send(sock, request.c_str(), request.length(), 0);

    char response[BUFFER_SIZE] = {0};
    recv(sock, response, 5, 0);

    if (strncmp(response, "RES: ", 5) == 0) {
      std::string filepath =
          std::string(IMAGE_FOLDER) + "received_" + image_name;
      std::ofstream file(filepath, std::ios::binary);

      int bytes;
      while ((bytes = recv(sock, response, BUFFER_SIZE, 0)) > 0) {
        file.write(response, bytes);
      }

      file.close();

      std::cout << "[PEER_NETWORK] Image '" << image_name
                << "' received and saved to '" << filepath << "'." << std::endl;
      close(sock);

      return true;
    } else {
      std::cerr << "[PEER_NETWORK] Invalid response from peer on 'REQ': "
                << response << std::endl;
    }
  }
  close(sock);
  return false;
}

void PeerNetwork::handle_client(int client_socket) {
  char buffer[BUFFER_SIZE] = {0};

  recv(client_socket, buffer, BUFFER_SIZE, 0);
  std::string request(buffer);

  if (request.find("REQ: ") == 0) {
    std::string image_name = request.substr(5);
    send_image_to_peer(client_socket, image_name);
  } else if (request == "HELLO") {
    std::cout << "[PEER_NETWORK " << _port << "] Received HELLO from peer."
              << std::endl;
    respond_with_peer_list(client_socket);
  }

  close(client_socket);
}

void PeerNetwork::send_image_to_peer(int client_socket,
                                     const std::string &image_name) {
  char buffer[BUFFER_SIZE] = {0};

  std::string filepath = IMAGE_FOLDER + image_name;
  std::ifstream file(filepath, std::ios::binary);

  std::cout << "[PEER_NETWORK " << _port << "] Searching for file: " << filepath
            << std::endl;

  if (file) {
    send(client_socket, "RES: ", 5, 0);
    while (!file.eof()) {
      file.read(buffer, BUFFER_SIZE);
      send(client_socket, buffer, file.gcount(), 0);
    }
  } else {
    std::string err = "ERR: File not found";
    send(client_socket, err.c_str(), err.length(), 0);
  }
}

void PeerNetwork::respond_with_peer_list(int client_socket) {
  std::string msg = "PEERS:";

  std::lock_guard<std::mutex> lock(peer_mutex);
  for (const auto &[ip, port] : peer_list) {
    if (port == _port)
      continue;

    msg += " " + ip + ":" + std::to_string(port);
  }

  send(client_socket, msg.c_str(), msg.length(), 0);
}
