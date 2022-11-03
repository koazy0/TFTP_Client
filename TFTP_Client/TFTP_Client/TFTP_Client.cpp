// TFTP_Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include"TFTP_Client.h"

CString Strtmp;
int sizeofsockaddr =sizeof(sockaddr);
WSADATA wsaData;

//目标addr,recv addr
sockaddr_in servAddr, bindaddr;
//发送sock,接收sock
SOCKET sock, sockrecv;

char buf[1024];

PRQ_Information Information=new(TRQ);

//定义五个全局变量 方便处理
RRQ rrq; WRQ wrq; TRQ trq; ERQ erq; ARQ arq;


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

int SendPacket(void *dst,int size) {
	int count = sendto(sock, (char*)dst, size, 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
	if (count == SOCKET_ERROR) {
		Strtmp.Format("Error code:%d", WSAGetLastError());
		MessageBox(NULL, Strtmp, "SendPacket Error!", MB_OK);
	}
	return count;
	 
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
			count=SendPacket(&rrq, sizeof(RRQ));
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
			FillMode(wrq.Mode, mode);
			wrq.OP = code;


			count = SendPacket(&wrq, sizeof(WRQ));
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
			count = SendPacket(&arq, sizeof(ARQ));
			cout << "Send ARQ\n\n";
			return count;

		case ERR:
			memset(&erq, 0, sizeof(ERQ));
			erq.OP = code;
			erq.Number = mode;
			tmp = erq.Data;
			Fill_ERROR(tmp,mode);

			count = SendPacket(&erq, sizeof(ERQ));
			cout << "Send ERQ\n\n";
			return count;

		default:
			return -1;
	}
}

int RecvPack(PRQ_Information Information) {
	int len = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&bindaddr, &sizeofsockaddr);
	if (PACKET_ERR == len)
	{
		cout<<"udp server recv error ";
		return -1;
	}
	buf[len] = '\0';
	
	

	//switch opcode
	switch ((WORD)*buf) {
	case 3:
		
		//接收数据包的时候trq和才会更新
		memcpy(&trq,buf,sizeof(TRQ));
		trq.OP = len - 4;		//data段的长度
		return len;	//return的是data的长度
		//data包
	case 4:
		//ack数据包
		memcpy(&arq,buf, sizeof(ARQ));
		return len;		//return the length of packet
	case 5:
		//Err包
		memcpy(&erq, buf, sizeof(ERQ));
		cout << "12345\n\n";
		return ERROR;
	default:
		cout << "error\n\n";
		//checkerror();
		return PACKET_ERR;
	}
}

void Download(PRQ_Information Information, FILE *fp) {
	//这时PRQ_Information 存储着第一个包的信息
	cout << "Download()\n";
	int record = 1;		//记录包数
	while (1) {
		if (trq.OP < 512) break;
		else if (trq.Number != record) {
			
			//中途有包丢失,或者传来的是error包
			//这个时候信息存储在erq中
			//不需要发送响应包
			RecordLog();
		}
		else {

			//将缓存输入文件中
			
			//发送响应包
			//执行下一个接收
		}
		record++;
	}

	//输入文件中


	
}

//下载文件
BOOL RequestRRQ() {
	char file[30] = {0},mode; int recv; FILE *fp=NULL;
	
	PRQ_Information Information=new(TRQ);
	
	while (1) {
		cout << "Download files\n\n";
		cout << "Input the filename:\n";
		cout << "send 'q' to quit\n";
		cin >> file;
		system("cls");
		cout << "Choose the mode and input the number\n\n";
		cout << "1 : netascii\n";
		cout << "2 : octet\n";
		cout << "send 'q' to quit\n";
		cin >> mode;

		if (file[0] == 'q'||mode=='q') exit(0);
		else if (!file) {
			cout << "Input error! Please retype!\n";
		}
		else {
			
			//choose the mode of RRQ
			switch (mode)
			{
			case '1':
				SendRQ(file, RT, READ);
				fp = fopen(file, "rt+");
				break;
			case '2':
				SendRQ(file, RT, READ);
				fp = fopen(file, "rb+");
			default:
				break;
			}
			if (!fp) {
				Strtmp.Format("Open file %s Error!",file);
				MessageBox(NULL, Strtmp, "File Error", MB_OK);
			}

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

int init() {
	char ip[20];
	int port,tmp;
	
	//绑定发送接收端口
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_port = htons(10000);
	bindaddr.sin_addr.s_addr=INADDR_ANY;
	bind(sock, (struct sockaddr*)&bindaddr, sizeofsockaddr);
	bind(sockrecv, (struct sockaddr*)&bindaddr, sizeofsockaddr);
	
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
					return 0;
				case 2:
					RequestWRQ();
					Sleep(1000);
					return 0;
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

