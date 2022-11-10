#pragma once
#include<Windows.h>
#include<iostream>
#include<fstream>
#include <iomanip>
#include<chrono>
#include<vector>


#define RT_C "Netascii"		//传输字符串
#define RT 0x01		//传输字符串
#define RB_C "Octet"		//传输二进制
#define RB 0x02		//传输二进制

#define BUF_SIZE 1024

//definition of opcode
#define READ	0x0100
#define WTIRE	0x0200
#define TRAN	0x0300
#define ACK		0x0400
#define ERR		0x0500


using namespace std;
struct Block
{
	char flag;
	double throughput;
};

struct RRQ
{
	WORD OP;
	/*char FileName[20];
	char Mode[20];*/
	char Mode_file[20];
};

typedef RRQ WRQ;

struct TRQ
{
	WORD OP;
	WORD Number;
	char Data[512];
};

typedef TRQ ERQ, *PRQ_Information;

struct ARQ
{
	WORD OP;
	WORD Number;
};


void summary(Block *pblock);

void RecordLog(ofstream &outfile, char *file, int count, double throught, WORD mode);

void WritetoFile(FILE *fp, TRQ *ptrq);

int initbuf(FILE *fp, char *buf);