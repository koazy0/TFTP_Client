#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<atlstr.h>

#pragma comment(lib, "ws2_32.lib")

int initbuf(char *sendbuf1, FILE *fp) {
	int count = 0;
	do {
		sendbuf1[count++] = fgetc(fp);
	} while (count < 512 && sendbuf1[count-1]!=EOF);

	return count;
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	char a[30] = { 0 };
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

	struct sockaddr_in clientAddr = { 0 };
	clientAddr.sin_family = AF_INET;                         
	clientAddr.sin_port = htons(10000);						
	bind(sockfd, (struct sockaddr*)&clientAddr, sizeof(sockaddr));//指定特定端口发送
	// 3. 发送数据到指定的ip和端口
	char sendbuf[] = { "hello world, I am UDP." };
	char sendbuf1[512] ;
	int len;


	FILE *fp = fopen("text.txt", "r");

	do
	{
		len = initbuf(sendbuf1, fp);
	
		ret = sendto(sockfd, sendbuf1, len, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
		printf("ret = %d \n", ret);
		memset(sendbuf1, 0, 512);
	
	}while (len == 512);

	// 4. 关闭套接字
	fclose(fp);
	closesocket(sockfd);
	WSACleanup();

 }


