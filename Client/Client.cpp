#include <iostream>
#include <string>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

class TCPClient {
private:
    SOCKET sock;
    struct sockaddr_in serv_addr;

public:
    TCPClient(const char* address, int port) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            exit(EXIT_FAILURE);
        }
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address: Address not supported." << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection Failed." << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }

    void sendFileName(const std::string& filename) {
        int bytes_sent = send(sock, filename.c_str(), filename.size(), 0);
        if (bytes_sent == SOCKET_ERROR) {
            std::cerr << "Send failed." << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }

    std::string receiveFileContent() {
        char buffer[1024];
        std::string file_content;
        int bytes_received;
        do {
            bytes_received = recv(sock, buffer, sizeof(buffer), 0);
            if (bytes_received == SOCKET_ERROR) {
                std::cerr << "Recv failed." << std::endl;
                closesocket(sock);
                WSACleanup();
                exit(EXIT_FAILURE);
            }
            file_content.append(buffer, bytes_received);
        } while (bytes_received > 0);
        return file_content;
    }

    ~TCPClient() {
        closesocket(sock);
        WSACleanup();
    }
};

int main() {
    while (true) {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;

        TCPClient client("127.0.0.1", 8080);
        client.sendFileName(filename);

        std::string file_content = client.receiveFileContent();

        std::cout << "File content:" << std::endl << file_content << std::endl;

        // Запрос пользователя о продолжении работы.
        std::string input;
        std::cout << "Press 'q' to quit, or any other key to continue: ";
        std::cin >> input;
        if (input == "q") {
            break;
        }
    }

    return 0;
}
