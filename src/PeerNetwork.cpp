#include "../include/PeerNetwork.h"

#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

#define IMAGE_FOLDER "../images/"

PeerNetwork::PeerNetwork(int port) : port(port) {
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
  server_addr.sin_port = htons(port);

  bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr));
  listen(server_socket, 5);

  std::cout << "[PEER_NETWORK] Server started on port " << port << std::endl;

  while (true) {
    int client_socket = accept(server_socket, nullptr, nullptr);
    std::thread(&PeerNetwork::handle_client, this, client_socket).detach();
  }
}

void PeerNetwork::send_hello_and_receive_peers() {}

void PeerNetwork::request_image(const std::string &image_name) {}

void PeerNetwork::handle_client(int client_socket) {}
