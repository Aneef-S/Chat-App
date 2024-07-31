#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool InitializeWinSock()
{
    WSADATA data;
    return (WSAStartup(MAKEWORD(2, 2), &data) == 0);
}

void InteractWithClient(SOCKET clientSocket, std::vector<SOCKET>& clients)
{
    cout << "Client connected" << endl;
    char buffer[4096];

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvLen <= 0)
        {
            break;
        }
        string message(buffer, recvLen);
        if (message == "STOP")
        {
            break;
        }
        cout << message << endl;

        for (auto client : clients)
        {
            if (client != clientSocket)
            {
                send(client, message.c_str(), message.length(), 0);
            }
        }
    }

    auto it = find(clients.begin(), clients.end(), clientSocket);
    if (it != clients.end())
    {
        clients.erase(it);
    }

    closesocket(clientSocket);
    cout << "Client disconnected" << endl;
}

int main()
{
    if (!InitializeWinSock())
    {
        cout << "Initialize Failed" << endl;
        return 1;
    }

    int port = 12345;
    vector<SOCKET> clients;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (InetPton(AF_INET, _T("0.0.0.0"), &serverAddr.sin_addr) != 1)
    {
        cout << "Setting server address failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Bind error" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Listen Error" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server started listening on port - " << port << endl;

    while (true)
    {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            cout << "Invalid client socket" << endl;
            continue;
        }
        clients.push_back(clientSocket);
        thread t1(InteractWithClient, clientSocket, std::ref(clients));
        t1.detach();  // Detach the thread to allow it to run independently
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
