# P2P Image Sharing

A lightweight peer-to-peer image sharing application written in C++. Each peer acts as both a server and a client, allowing users to request and serve images across a decentralized network.

## 📦 Features

- Request images from other peers  
- Serve local images to requesting peers  
- Dynamically discover additional peers  
- Minimal dependencies (POSIX sockets, threads)

## 🚀 Usage

Run a peer by specifying its port:

```bash
./p2p 9001
```

Then follow the prompt to request an image:

```bash
Enter image name to request (or 'exit'):

```

Images should be placed in the `images/` folder, and received images are saved there as well (prefixed with `received_`).

## 🔗 Message Formats

Peers communicate using the following message formats:

| Message              | Description                    | When Sent                                |
|----------------------|--------------------------------|-------------------------------------------|
| `REQ: <image>`       | Requests an image              | When a user asks for an image             |
| `RES:`               | Begins binary stream of image  | If a peer has the requested image         |
| `ERR: not_found`     | Indicates missing image        | If a peer does not have the image         |
| `HELLO`              | Peer discovery request         | When a peer starts up                     |
| `PEERS: ip:port ...` | List of known peers            | Response to `HELLO`                       |

## 🗂️ Directory Structure

```plaintext
.
├── include/
│   └── p2p.hpp             # Shared declarations
├── src/
│   ├── main.cpp            # Entry point
│   └── p2p.cpp             # Core functionality
├── build/                  # Compiled object files
├── images/                 # Local and received images
├── Makefile
├── p2p                     # Executable file
└── README.md
```

## 🧪 Example

To test locally:

1. Open multiple terminals.  
2. Run different peers on different ports:  
   ```bash
   ./p2p 9001  
   ./p2p 9002
   ```
3. Put an image in the `images/` directory of one peer.  
4. From another peer, request it by filename.