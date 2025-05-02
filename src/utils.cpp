#include <iostream>

#include "../include/network.h"
#include "../include/utils.h"

// check if a string is a number
bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void user_interface() {
  std::string input;

  while (true) {
    std::cout << "Enter image name to request (or 'quit' to exit): ";
    std::cin >> input;

    if (input == "quit")
      break;

    request_image(input);
  }
}
