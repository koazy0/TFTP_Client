#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<atlstr.h>

#pragma comment(lib, "ws2_32.lib")


#define PORT 9988
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	int sockfd;
	int sin_len;
	struct sockaddr_in saddr;
	struct sockaddr_in remote_addr;
	char buff[BUFF_SIZE];
	int res, len, count;
	FILE *fp=fopen("recv.txt","w");
	
	//1. create socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sockfd)
	{
		perror("Udp server socket: ");
		return -1;
	}
	printf("Udp server socket create succ!\n");

	//2. prepare IP and port
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = INADDR_ANY;

	//3. bind
	res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (-1 == res)
	{
		perror("udp server bind: ");
		return -1;
	}
	memset(buff, 0, BUFF_SIZE);
	do {
		//4. recvfrom
		count =0;
		printf("Wait for a packet ...\n");
		sin_len = sizeof(struct sockaddr_in);
		len = recvfrom(sockfd, buff, BUFF_SIZE, 0, (struct sockaddr *)&remote_addr, &sin_len);

		while (buff[count] != EOF && count < 512) {
			fputc(buff[count++], fp);
		}
		if (-1 == len)
		{
			perror("udp server recvform: ");
			return -1;
		}
		buff[len] = '\0';
		
		printf(" contents is: %s \n", \
			 buff);
		
		
	} while (count ==512);
	printf("Wait for a packet ...\n");
	


	//5. close
	fclose(fp);
	closesocket(sockfd);
	WSACleanup();
	return 0;
}