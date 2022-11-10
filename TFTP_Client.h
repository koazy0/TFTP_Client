#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<fstream>
#include<atlstr.h>
#include <iomanip>
#include<chrono>
#include"record.h"

#pragma comment(lib, "ws2_32.lib")  //╪сть ws2_32.dll



#define	PACKET_ERR		-1



void checkData() {

}


BOOL RequestRRQ();
BOOL RequestWRQ();

void menu();
int init();
