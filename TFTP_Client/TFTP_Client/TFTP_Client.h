#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>

#define RT "netascii"	//传输字符串
#define RB "octet"		//传输二进制

//definition of opcode
#define READ 0x0001
#define WTIRE 0x0002
#define TRAN 0x0003
#define ACK 0x0004
#define ERR 0x0005

struct RRQ
{
	WORD OP;
	char FileName[20];
	char Mode[20];

};

typedef RRQ WRQ;

struct TRQ
{
	WORD OP;
	char data[512];
};

struct ARQ
{
	WORD OP;
	WORD Number;
};

struct ERQ
{
	WORD OP;
	WORD ErrorCode;
};


BOOL SendRRQ() {
	return TRUE;
}

BOOL SendACK() {
	return TRUE;
}


int length(char *) {
	return 1;
}

int RecvTRQ(TRQ TranceferRQ) {
	return length(TranceferRQ.data);
}

void checkData() {

}


BOOL WriteData() {

}
BOOL RecordNum() {

}

BOOL closeconnction() {

}





void SendWRQ() {

}
