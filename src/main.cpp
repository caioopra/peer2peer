#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>

#include <thread>

#include "../include/PeerNetwork.h"
#include "../include/utils.h"

#define BASE_PORT 9000

int my_port = BASE_PORT; // change via command line argument

void start_server() {}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::string port_str = argv[1];
    if (is_number(port_str)) {
      my_port = std::stoi(port_str);
    } else {
      std::cerr << "[MAIN] Invalid port number: " << port_str << std::endl;

      return 1;
    }
  }

  PeerNetwork peer_network(my_port);

  // starting server in a separate thread
  std::thread server_thread(&PeerNetwork::start_server, &peer_network);
  server_thread.detach();

  std::cout << "[MAIN] Peer network started on port " << my_port << std::endl;

  // simulate sending a hello message and receiving peers
  std::cout << "[MAIN] Sending hello message and receiving peers...\n";
  peer_network.send_hello_and_receive_peers();

  // Simulate requesting an image
  const std::string image_name = "example.jpg";
  std::cout << "[MAIN] Requesting image: " << image_name << "\n";
  peer_network.request_image(image_name);

  // Keep the main thread alive to allow server to run
  while (true) {
    sleep(1);
  }

  return 0;
}
