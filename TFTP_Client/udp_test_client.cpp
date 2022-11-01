#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<atlstr.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	int ret = -1;
	// 1、使用socket()函数获取一个socket文件描述符
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sockfd)
	{
		printf("socket open err.");
		return -1;
	}

	// 2. 准备接收方的地址和端口，'192.168.0.107'表示目的ip地址，8266表示目的端口号 
	struct sockaddr_in sock_addr = { 0 };
	sock_addr.sin_family = AF_INET;                         // 设置地址族为IPv4
	sock_addr.sin_port = htons(9988);						// 设置地址的端口号信息
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//　设置IP地址

	// 3. 发送数据到指定的ip和端口
	char sendbuf[] = { "hello world, I am UDP." };
	int cnt = 10;
	while (1)
	{
		ret = sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
		printf("ret = %d \n", ret);
		Sleep(100);
	}

	// 4. 关闭套接字
	closesocket(sockfd);
	WSACleanup();

 }


