#include <algorithm>
#include <string>

// check if a string is a number
bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void user_interface() {

}
