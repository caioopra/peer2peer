#include "../include/PeerNetwork.h"

#define IMAGE_FOLDER "../images/"

PeerNetwork::PeerNetwork(int port) : port(port) {
  peer_list = {
      {"127.0.0.1", 9001},
      {"127.0.0.1", 9002},
      {"127.0.0.1", 9003},
      {"127.0.0.1", 9004},
  };
}
