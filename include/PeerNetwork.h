#include <mutex>
#include <set>
#include <string>
#include <vector>

#define BUFFER_SIZE 4096

using PeerSet = std::set<std::pair<std::string, int>>;

class PeerNetwork {
public:
  PeerNetwork(int port);

  void start_server();
  void send_hello_and_receive_peers();
  void request_image(const std::string &image_name);

  int get_port() const { return _port; }

private:
  int _port;

  std::vector<std::pair<std::string, int>> peer_list;

  std::mutex peer_mutex;

  void handle_client(int client_socket);
  void respond_with_peer_list(int client_socket);
  void send_image_to_peer(int client_socket, const std::string &image_name);

  void _connect_to_peer(int sock, PeerSet *discovered);

  bool _connect_and_request(const std::string &ip, int port,
                            const std::string &request,
                            const std::string &image_name);
};
