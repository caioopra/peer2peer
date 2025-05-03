#include <mutex>
#include <string>
#include <vector>

class PeerNetwork {
public:
  PeerNetwork(int port);

  void start_server();
  void send_hello_and_receive_peers();
  void request_image(const std::string &image_name);

  int get_port() const { return port; }

private:
  int port;

  std::vector<std::pair<std::string, int>> peer_list;

  std::mutex peer_mutex;

  void handle_client(int client_socket);
  void respond_with_peer_list(int client_socket);
  void send_image_to_peer(int client_socket, const std::string &image_name);
};
