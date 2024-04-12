#include <iostream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка при запуске WSAStartup: " << WSAGetLastError() << std::endl;
        return 1;
    }

    const char* serverAddress = "127.0.0.1";
    const int serverPort = 8080;

    // Создание сокета клиента.
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Не удалось создать сокет: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Задание параметров адреса сервера.
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverAddress, &serverAddr.sin_addr);

    // Подключение к серверу.
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка подключения к серверу: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Отправка названия файла серверу.
    const char* filename = "library1.txt";
    size_t filenameLength = strlen(filename);
    if (send(clientSocket, filename, filenameLength, 0) == SOCKET_ERROR) {
        std::cerr << "Ошибка отправки имени файла на сервер: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Получение содержимого файла от сервера.
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Ошибка при получении содержимого файла от сервера: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Вывод содержимого файла на экран.
    std::cout << "Получено содержимое файла от сервера:" << std::endl;
    std::cout.write(buffer, bytesReceived);
    std::cout << std::endl;

    // Код для дальнейшей работы с сервером или других действий.

    // Оставляем клиент открытым и ожидаем дополнительных действий пользователя.
    std::cout << "Нажмите Enter для завершения программы...";
    std::cin.get();

    // Закрытие соединения и очистка ресурсов.
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
