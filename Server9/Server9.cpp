#include <iostream>
#include <Winsock2.h>
#include <fstream>
#include <string>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

class TCPServer {
private:
    SOCKET server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const int port = 8080;

public:
    TCPServer() {
        WSADATA wsaData;
        int opt = 1;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
            exit(EXIT_FAILURE);
        }
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
            std::cerr << "setsockopt failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }

    void run() {
        std::cout << "Waiting for connections..." << std::endl;
        SOCKET client_socket;
        while (true) {
            if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) == INVALID_SOCKET) {
                std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
                closesocket(server_fd);
                WSACleanup();
                exit(EXIT_FAILURE);
            }
            char buffer[1024] = { 0 };
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received == SOCKET_ERROR) {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                closesocket(client_socket);
                continue;
            }
            std::string filename(buffer, bytes_received);
            std::cout << "Received filename: " << filename << std::endl;
            std::string file_content;
            // Попытка открыть и прочитать содержимое файла
            std::ifstream file(filename);
            if (!file) {
                std::cerr << "File not found: " << filename << std::endl;
                const char* not_found_message = "File not found";
                send(client_socket, not_found_message, strlen(not_found_message), 0);
                closesocket(client_socket);
                continue;
            }
            std::string line;
            while (std::getline(file, line)) {
                file_content += line + "\n";
            }
            file.close();
            // Отправка содержимого файла клиенту
            send(client_socket, file_content.c_str(), file_content.size(), 0);
            closesocket(client_socket);
        }
    }

    ~TCPServer() {
        closesocket(server_fd);
        WSACleanup();
    }
};

int main() {
    TCPServer server;
    server.run();
    return 0;
}
