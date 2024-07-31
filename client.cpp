#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include <string>
// #include<bits/stdc++.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool InitializeWinSock()
{
    WSADATA data;
    return (WSAStartup(MAKEWORD(2, 2), &data) == 0);
}

void sendMessage(SOCKET serverSocket)
{
    string name;
    cout<<"Enter your Name - ";
    getline(cin,name);
    string message;
    while (1)
    {
        cout << "Enter your chat - ";
        
        getline(cin, message);
        if (message == "QUIT")
        {
            break;
        }
        message = name + " : " + message;
        if (send(serverSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR)
        {
            cout << "Message not send" << endl;
        }
    }
    closesocket(serverSocket);
    
}

void reciveMessage(SOCKET serverSocket)
{
    char buffer[4096];
    int recvLen;
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        recvLen = recv(serverSocket, buffer, sizeof(buffer), 0);
        if (recvLen <= 0)
        {
            cout << "Error while reciving a message" << endl;
        }
        string message(buffer, recvLen);
        cout <<endl<< message << endl;
    }
}

int main()
{
    if (!InitializeWinSock())
    {
        cout << "Initalize Failed" << endl;
        return 1;
    }

    int port = 12345;
    string serverAddress = "127.0.0.1";

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == SOCKET_ERROR)
    {
        cout << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) != 1)
    {
        cout << "Setting server adress failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (connect(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << " Connection error" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Successfully connected to server" << endl;

    // if(listen(serverSocket,SOMAXCONN) == SOCKET_ERROR)
    // {
    //     cout << "Listen Error" << endl;
    //     closesocket(serverSocket);
    //     WSACleanup();
    //     return 1;
    // }

    // cout << "Server statred listening to " << port <<endl;

    // SOCKET clientSocket = accept(serverSocket,nullptr,nullptr);
    // if(clientSocket == INVALID_SOCKET)
    // {
    //     cout << "Inalid client socket" << endl;
    // }

    thread senderThread(sendMessage, serverSocket);
    thread reciverThread(reciveMessage, serverSocket);
    

    senderThread.join();
    reciverThread.join();

    // closesocket(clientSocket);
    closesocket(serverSocket);

    WSACleanup();
    return 0;
}