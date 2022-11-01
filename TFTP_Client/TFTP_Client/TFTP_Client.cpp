// TFTP_Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include"TFTP_Client.h"

CString Strtmp;
int addrlen;
WSADATA wsaData;
sockaddr_in servAddr;
sockaddr fromAddr;
PRQ_Information Information;

//创建套接字
SOCKET sock;

using namespace std;

void ShowError(PRQ_Information, int code) {
	cout << "ShowError\n\n";
	switch (code)
	{
	case ERROR:
	case PACKET_ERR:
		break;
	default:
		break;
	}
}

void Download(PRQ_Information Information, FILE *fp) {
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

int SendRRQ(char *file) {
	cout << "Send RRQ\n\n";
	return 1;
}

int RecvPack(PRQ_Information Informtion) {
	DWORD OP;
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


BOOL RequestRRQ() {
	char file[30]; int recv; FILE *fp;
	PRQ_Information Information=new(TRQ);
	
	while (1) {
		cout << "Download files\n\n";
		cout << "Input the filename:\n";
		cout << "send 'q' to quit\n";
		cin >> file;
		if (file[0] == 'q') exit(0);
		SendRRQ(file);
		recv = RecvPack(Information);
		if (recv == PACKET_ERR) {
			ShowError(Information,recv);
			system("pause");
			system("cls");
		}
		else if (recv == ERROR) {
			ShowError(Information, recv);
			system("pause");
			system("cls");
		}
		else {
			Download(Information,fp);
			cout << "Download Ends\n\n";
			system("pause");
			system("cls");
		}		
	}	
	
	free(Information);
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

