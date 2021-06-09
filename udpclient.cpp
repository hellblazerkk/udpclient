// пример простого UDP-клиента
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 666
#define SERVERADDR "127.0.0.1"

int main(int argc, char* argv[])
{
    char buff[10 * 1014];
    printf("UDP DEMO Client\nType quit to quit\n");

    // Шаг 1 - иницилизация библиотеки Winsocks
    if (WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        printf("WSAStartup error: %d\n",
            WSAGetLastError());
        return -1;
    }

    // Шаг 2 - открытие сокета
    SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_sock == INVALID_SOCKET)
    {
        printf("socket() error: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    // Шаг 3 - обмен сообщений с сервером
    HOSTENT* hst;
    sockaddr_in dest_addr;

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    // определение IP-адреса узла
    if (inet_addr(SERVERADDR))
        dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    else
        if (hst = gethostbyname(SERVERADDR))
            dest_addr.sin_addr.s_addr = ((unsigned long**)
                hst->h_addr_list)[0][0];
        else
        {
            printf("Unknown host: %d\n", WSAGetLastError());
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }

    while (1)
    {
        // чтение сообщения с клавиатуры
        printf("S<=C:"); fgets(&buff[0], sizeof(buff) - 1,
            stdin);
        if (!strcmp(&buff[0], "quit\n")) break;

        // Передача сообщений на сервер
        sendto(my_sock, &buff[0], strlen(&buff[0]), 0,
            (sockaddr*)&dest_addr, sizeof(dest_addr));

        // Прием сообщения с сервера
        sockaddr_in server_addr;
        int server_addr_size = sizeof(server_addr);

        int n = recvfrom(my_sock, &buff[0], sizeof(buff) - 1, 0,
            (sockaddr*)&server_addr, &server_addr_size);

        if (n == SOCKET_ERROR)
        {
            printf("recvfrom() error:"\
                "%d\n", WSAGetLastError());
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }

        buff[n] = 0;

        // Вывод принятого с сервера сообщения на экран
        printf("S=>C:%s", &buff[0]);
    }

    // шаг последний - выход
    closesocket(my_sock);
    WSACleanup();

    return 0;
}