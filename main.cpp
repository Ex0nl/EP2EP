#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sodium.h>
#include <stdlib.h>
#include <cstdio>
#include <netdb.h>

unsigned char key[] = "a-very-secret-key-that-is-32-byte"; // Make sure the encryption-key is the same for you and your peer!!!

void setup_tunnel(int port) {
    // Check if Bore is installed. If not, install it
    if (access("bore", F_OK) == -1) {
        std::cout << "\n[!] Setting up 'bore'..." << std::endl;
        const char* install_cmd = "curl -L https://github.com/ekzhang/bore/releases/download/v0.6.0/bore-v0.6.0-i686-unknown-linux-musl.tar.gz -o bore.tar.gz && tar -xzf bore.tar.gz && rm bore.tar.gz && chmod +x bore";
        system(install_cmd);
    }

    // Clear old log
    system("rm -f bore_output.txt");
    
    // Start Bore
    std::string cmd_str = "./bore local " + std::to_string(port) + " --to bore.pub > bore_output.txt 2>&1 &";
    system(cmd_str.c_str()); 
    
    // Give Bore time to write the file
    sleep(2);
    
    // Cat Bore port outputs
    std::cout << "\n==========================================" << std::endl;
    std::cout << ">>> TUNNEL OUTPUT:" << std::endl;
    system("cat bore_output.txt");
    std::cout << "\n==========================================\n" << std::endl;
}

void listener_thread(int port) {
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1; setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr = {AF_INET, htons(port), INADDR_ANY};
    bind(srv, (struct sockaddr*)&addr, sizeof(addr));
    listen(srv, 10);
    while (true) {
        int client = accept(srv, NULL, NULL);
        if (client < 0) continue;
        unsigned char nonce[crypto_secretbox_NONCEBYTES], cipher[4096];
        if (recv(client, nonce, sizeof(nonce), MSG_WAITALL) > 0) {
            int c_len = recv(client, cipher, 4096, 0);
            if (c_len > 0) {
                std::string decrypted(c_len - crypto_secretbox_MACBYTES, 0);
                if (crypto_secretbox_open_easy((unsigned char*)decrypted.data(), cipher, c_len, nonce, key) == 0)
                    std::cout << "\n[Peer]: " << decrypted << "\n> " << std::flush;
            }
        }
        close(client);
    }
}

int main() {
    std::cout << "EP2EP: E2EE P2P Chat (TCP Bore Edition)\n";
    int my_port;
    std::cout << "Enter your local port: "; std::cin >> my_port;
    
    setup_tunnel(my_port);
    
    std::string peer_ip_str;
    int peer_port;
    std::cout << "Enter Peer's IP (or bore.pub): "; std::cin >> peer_ip_str;
    std::cout << "Enter Peer's assigned remote port: "; std::cin >> peer_port;

    if (sodium_init() < 0) return 1;
    std::thread(listener_thread, my_port).detach();

    std::cout << "Chat started. Type messages below:\n> ";
    std::string msg;
    std::cin.ignore();
    while (std::getline(std::cin, msg)) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct hostent* server = gethostbyname(peer_ip_str.c_str());
        if (server) {
            sockaddr_in srv;
            srv.sin_family = AF_INET;
            srv.sin_port = htons(peer_port);
            memcpy(&srv.sin_addr.s_addr, server->h_addr, server->h_length);
            
            if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) == 0) {
                unsigned char nonce[crypto_secretbox_NONCEBYTES], cipher[4096 + crypto_secretbox_MACBYTES];
                randombytes_buf(nonce, sizeof(nonce));
                crypto_secretbox_easy(cipher, (unsigned char*)msg.c_str(), msg.length(), nonce, key);
                send(sock, nonce, sizeof(nonce), 0);
                send(sock, cipher, msg.length() + crypto_secretbox_MACBYTES, 0);
            } else {
                std::cout << "Error: Could not connect to peer." << std::endl;
            }
        }
        close(sock);
        std::cout << "> ";
    }
    return 0;
}