#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <mutex>
#include <thread>

#include "utils.cpp"
#include "network.cpp"

#define BASE_PORT 9000

int my_port = BASE_PORT; // change via command line argument

void start_server() {}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::string port_str = argv[1];
    if (is_number(port_str)) {
      my_port = std::stoi(port_str);
    } else {
      std::cerr << "Invalid port number: " << port_str << std::endl;

      return 1;
    }
  }

  std::thread server_thread(start_server);
  server_thread.detach();

  sleep(1);

  send_hello_and_receive_peers();
  user_interface();

  return 0;
}
