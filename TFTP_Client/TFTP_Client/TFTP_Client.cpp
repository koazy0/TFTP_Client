// TFTP_Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include"TFTP_Client.h"

CString Strtmp;
int addrlen;
WSADATA wsaData;
sockaddr_in servAddr;
sockaddr fromAddr;
PRQ_Information Information;

//定义五个全局变量 方便处理
RRQ rrq; WRQ wrq; TRQ trq; ERQ erq; ARQ arq;
SOCKET sock;

using namespace std;

void ShowError(PRQ_Information Information, int code) {
	cout << "ShowError.\n\n";
	switch (code)
	{
	case ERROR:
		cout << "ErrorCode:"<<Information->Number <<endl;
		cout << "ErrorMessage:"<<Information->Data <<endl;
	case PACKET_ERR:
		break;
	default:
		break;
	}
}

void Download(PRQ_Information Information, FILE *fp) {
	
	cout << "Download()\n";
	//
	//while (1) {
	//	checkData();
	//	//RecordData();
	//	RecordNum();
	//	SendACK();
	//	if (1 < 512)
	//		closeconnction();
	//	break;
	//}
}

void FillMode(char *dst,int mode) {
	switch (mode) {
	case RT:
		memcpy(dst,RT_C,sizeof(RT_C));
	case RB:
		memcpy(dst, RB_C, sizeof(RB_C));
	default:
		break;
	}
}
void Fill_ERROR(char *dst,int ErrorCode) {
	//实验报告里面没要求写这个
	//这里补充在这里芝士为了体现完整性
	switch (ErrorCode)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	default:
		strcpy_s(dst,15, "File not found");
		break;
	}
}
int SendRQ(char *file,int mode,WORD code) {
	int count; char *tmp;
	switch (code) {
		case READ:
			//发送RRQ请求包
			 tmp= (char *)rrq.FileName;

			//初始化请求包
			memset(&rrq, 0, sizeof(RRQ));
			while (file) {
				*tmp++ = *file++;		//strcpy the file name
			}
			FillMode(rrq.Mode, mode);	//fill the mode
			rrq.OP = code;					//fill the opcode

			//send the packet
			count = sendto(sock, (char*)&rrq, sizeof(RRQ), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
			if (count == SOCKET_ERROR) {
				Strtmp.Format("Error code:%d", WSAGetLastError());
				MessageBox(NULL, Strtmp, "Send RRQ Error!", MB_OK);
			}
			cout << "Send RRQ\n\n";
			return count;

		case WTIRE:
			//发送WRQ请求包
			tmp = (char *)wrq.FileName;
			//初始化请求包
			memset(&wrq, 0, sizeof(WRQ));
			while (file) {
				*tmp++ = *file++;	//strcpy
			}
			FillMode(rrq.Mode, mode);
			wrq.OP = code;


			count = sendto(sock, (char*)&rrq, sizeof(RRQ), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
			if (count == SOCKET_ERROR) {
				Strtmp.Format("Error code:%d", WSAGetLastError());
				MessageBox(NULL, Strtmp, "Send WRQ Error!", MB_OK);
			}
			cout << "Send WRQ\n\n";
			return count;
		case TRAN:

			count = 123;
			return count;
			//
		case ACK:
			//SendRQ(NULL,int BlockNumber,WORD code);
			memset(&arq, 0, sizeof(ARQ));
			arq.OP = code;
			arq.Number = mode;

			//send the ACK packet
			count = sendto(sock, (char*)&arq, sizeof(ARQ), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
			if (count == SOCKET_ERROR) {
				Strtmp.Format("Error code:%d", WSAGetLastError());
				MessageBox(NULL, Strtmp, "Send WRQ Error!", MB_OK);
			}
			cout << "Send WRQ\n\n";
			return count;


		case ERR:
			memset(&erq, 0, sizeof(ERQ));
			erq.OP = code;
			erq.Number = mode;
			tmp = erq.Data;
			Fill_ERROR(tmp,mode);

			return ERROR;
		default:
			return -1;
	}
}

int RecvPack(PRQ_Information Informtion) {
	DWORD OP=1;
	switch (OP) {
	case 1:
		
	case 2:

	case 3:

	case 4:
		return 1;		//return the length of packet
	case 5:
		cout << "12345\n\n";
		return ERROR;
	default:
		cout << "error\n\n";
		//checkerror();
		return PACKET_ERR;
	}
}

BOOL RequestRRQ() {
	char file[30] = {0}; int recv; FILE *fp=NULL;
	PRQ_Information Information=new(TRQ);
	
	while (1) {
		cout << "Download files\n\n";
		cout << "Input the filename:\n";
		cout << "send 'q' to quit\n";
		cin >> file;
		if (file[0] == 'q') exit(0);
		else if (!file) {
			cout << "Input error! Please retype!\n";
		}
		else {
			SendRQ(file,0x01,READ);
			recv = RecvPack(Information);
			if (recv == PACKET_ERR || recv == ERROR) {
				ShowError(Information, recv);
				system("pause");
				system("cls");
			}
			else {
				Download(Information, fp);
				cout << "Download Ends\n\n";
				system("pause");
				system("cls");
			}
		}
	}	
	free(Information);
	return TRUE;
}


BOOL RequestWRQ() {

	//TRQ TranceferRQ;
	//SendWRQ();
	////Recvdata();
	//SendACeK();
	//while (RecvData()) {
	//	checkData();
	//	WriteData();
	//	RecordNum();
	//	SendACK();
	//	if (1 < 512)
	//		closeconnction();
	//	break;
	//}
	cout << "Upload File\n";
	return TRUE;
}

void menu() {
	cout << "A Simple TFMP Client:\n\n";
	cout << "1.Download Files\n";
	cout << "2.Uploads Files\n";
	cout << "0.Exit\n";
}

void init() {
	char ip[20];
	int port,tmp;
	while (1) {
		while (1)		//命令行界面
		{
			memset(&servAddr, 0, sizeof(servAddr));  //清零
			system("cls");
			cout << "请输入服务器IP: ";
			cin >> ip;
			cout << "请输入服务器端口号: ";
			cin >> port;

			servAddr.sin_family = PF_INET;
			servAddr.sin_addr.s_addr = inet_addr(ip);
			servAddr.sin_port = htons(port);
			if (port != 69) {
				cout << "端口号错误,应为69\n";		//添加这一句在这里是表明要进行输入检查
				Sleep(2500);
			}

			else {
				cout << "初始化完成!\n";
				Sleep(2500);
				system("cls");
				break;
			}

		}
		while (1) {
			system("cls");
			menu();
			cin >> tmp;
			switch (tmp)
			{
				case 1:
					RequestRRQ();
					Sleep(1000);
					break;
				case 2:
					RequestWRQ();
					Sleep(1000);
					break;
				case 0:
					exit(0);
				default:
					cout << "您输入的有误，请重新输入\n";
					Sleep(2500);
					break;
			}
		}
		
	}
}


int main()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		MessageBox(NULL, "WSAStartup Error!", "Error", MB_OK);
		return 1;
	}
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock==INVALID_SOCKET) {	
		MessageBox(NULL, "socket Error!", "Error", MB_OK);
		Strtmp.Format("ErrorCode: %d",WSAGetLastError());
		MessageBox(NULL, Strtmp, "Error", MB_OK);
		return 1;
	}

	//服务器地址信息
	init();

	closesocket(sock);
	WSACleanup();
	return 0;
}

