// TFTP_Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include"TFTP_Client.h"
using namespace std;

CString Strtmp;
int sizeofsockaddr = sizeof(sockaddr);
WSADATA wsaData;
ofstream outfile;

//targrt addr and recv addr
sockaddr_in serv_Addr, bind_addr,resend_addr;
//send sock and recv sock
SOCKET sock, sockrecv;

char buf[BUF_SIZE];			//the buffer of recvfrom 

//定义五个全局变量 方便处理
RRQ rrq; WRQ wrq; TRQ trq; ERQ erq; ARQ arq;

TRQ trqs[BUF_SIZE*500],ptrq_error ;
Block block[BUF_SIZE * 500];




void ShowError(int code) {
	
	switch (code)
	{
	case ERROR:
		cout << "ShowError:"<< erq.Data<<"\n\n";
		break;
	case PACKET_ERR:
		cout << "ShowError:Recv Error\n\n";
		break;
	default:
		break;
	}
}

//finish the mode of text or octet
void FillMode(char *dst,int mode) {
	switch (mode) {
	case RT:
		memcpy(dst,RT_C,sizeof(RT_C));
		break;
	case RB:
		memcpy(dst, RB_C, sizeof(RB_C));
	default:
		break;
	}
}

void FillMode(vector<char> &tmp ,int mode) {
	switch (mode)
	{
	case RT:
		
		for (int i = 0; i < 9; i++) {
			tmp.push_back(RT_C[i]);
		}
		break;
	case RB:
		for (int i = 0; i < 9; i++) {
			tmp.push_back(RB_C[i]);
		}
		break;
	default:
		break;
	}
}

void Fill_ERROR(char *dst,int ErrorCode) {
	//
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

//function of sendto
int SendPacket(void *dst,int size) {
	
	int count = sendto(sock, (char*)dst, size, 0, (struct sockaddr*)&serv_Addr, sizeof(serv_Addr));
	if (count == SOCKET_ERROR) {
		Strtmp.Format("Error code:%d", WSAGetLastError());
		MessageBox(NULL, Strtmp, "SendPacket Error!", MB_OK);
	}
	return count;
	 
}


//Send all types of request query
int SendRQ(char *file,int mode,WORD code) {
	int count; char *tmp,*tmp_rrq;
	int a;
	switch (code) {
		case READ:
			//发送RRQ请求包
			 

			//初始化请求包
			memset(&rrq, 0, sizeof(RRQ));
			tmp = file;
			tmp_rrq = rrq.Mode_file;
			while (*tmp) {
				//*tmp++ = *file++;			//strcpy the file name
				*tmp_rrq++ = *tmp++;
			}
			*tmp_rrq++ = 0;					//fill the file name 

			FillMode(tmp_rrq, mode);		//fill the mode
			rrq.OP = code;					//fill the opcode

			//send the packet
		
			count=SendPacket(&rrq,sizeof(rrq));
			//cout << "Send RRQ\n\n";
			return count;

		case WTIRE:

			//reset the wrq
			memset(&wrq, 0, sizeof(WRQ));
			//strcpy
			tmp = file;
			tmp_rrq = wrq.Mode_file;
			while (*tmp) {
				//*tmp++ = *file++;			//strcpy the file name
				*tmp_rrq++ = *tmp++;
			}
			*tmp_rrq++ = 0;
		
			FillMode(tmp_rrq, mode);
			wrq.OP = code;
			count = SendPacket(&wrq, sizeof(WRQ));
			//cout << "Send WRQ\n\n";
			return count;

		case TRAN:	
			
			memset(&trq, 0, sizeof(TRQ));
			count=SendPacket(&trq, sizeof(TRQ));
			return count;
			//
		case ACK:
			//SendRQ(NULL,int BlockNumber,WORD code);
			memset(&arq, 0, sizeof(ARQ));
			arq.OP = code;
			arq.Number = mode;

			//send the ACK packet
			count = SendPacket(&arq, sizeof(ARQ));
			//cout << "Send ARQ\n\n";
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


//recv the single pack
//and classify the type
int RecvPack() {
	int len = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&resend_addr, &sizeofsockaddr);
	if (PACKET_ERR == len)
	{
		cout<<"udp server recv error ";
		return -1;
	}
	//switch opcode
	switch (*(WORD*)buf) {
	case TRAN:
		
		//接收数据包的时候trq和才会更新
		memcpy(&trq,buf,sizeof(TRQ));
		trq.OP = len-4 ;		//length of data
		return len;				//return the length of packet
		//data包
	case ACK:
		//ack数据包
		memcpy(&arq,buf, sizeof(ARQ));
		return len;		//return the length of packet
	case ERR:
		//Err包
		memcpy(&erq, buf, sizeof(ERQ));
		return ERROR;
	default:
		cout << "error\n\n";
		//checkerror();
		return PACKET_ERR;
	}
}



//the whole traceport stage
//store the file stream
void Download( FILE *fp,char *file) {
	
	int record = 0;		//count of packet
	WORD record_htons;
	TRQ *ptrqs=trqs;
	
	
	while (1) {
		//record_htons = htons(record+1);
		//if (trq.OP < 512)
		//{
		//	/*for (int i = 0; i < trq.OP-1&&trq.Data[i]!=EOF; i++) {
		//		fputc(trq.Data[i], fp);
		//	}*/
		//	memcpy(ptrqs++, &trq, sizeof(trq));
		//	SendRQ(NULL, trq.Number, ACK);

		//	ptrqs->OP = -1;
		//	block[record].flag = -1;	//没记录第一个data包
		//	record++;					//记录了包的个数
		//	
		//	break;
		//}
		//else if (trq.Number != record_htons) {
		//	
		//	//中途有包丢失,或者传来的是error包
		//	//这个时候信息存储在erq中
		//	//不需要发送响应包
		//	//直接发送下一个包
		//	//SendRQ(NULL, ++trq.Number, ACK);
		//	//RecordLog();
		//	//block[record].throughput = 0;
		//	
		//}
		//else {
		//	//将缓存输入文件中
		//	/*for (int i = 0; i < 512; i++) {
		//		fputc(trq.Data[i],fp);
		//	}*/
		//	
		//	memcpy(ptrqs++, &trq, sizeof(trq));
		//	//Send ACK
		//	SendRQ(NULL, trq.Number, ACK);
		//	//执行下一个接收n
		//	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		//	double flen = (double)trq.OP;
		//	RecvPack();
		//	auto _time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time;
		//	double f_time = (double)_time;
		//	block[record].flag = 0x1;
		//	block[record].throughput = (double) 1000*flen/f_time;
		//	cout << "Throughput is " << block[record].throughput << "MB/s\n";
		//	
		//	RecordLog(outfile, file, record+1, block[record].throughput, READ);
		//	record++;
		//}
		
		//Send ACK
		do {

			trqs[ntohs(trq.Number) - 1] = trq;
			SendRQ(NULL, trq.Number, ACK);
					//执行下一个接收n
		auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					double flen = (double)trq.OP;
					RecvPack();
					auto _time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time;
					double f_time = (double)_time;
					block[ntohs(trq.Number)-1].flag = 0x1;
					block[ntohs(trq.Number) - 1].throughput = (double) 1000*flen/f_time;
					//cout << "Throughput is " << block[ntohs(trq.Number) - 1].throughput << "MB/s\n";
					//cout << "trq.Number - 1 " << ntohs(trq.Number) - 1 << "\n\n";
					
					//RecordLog(outfile, file, ntohs(trq.Number), block[ntohs(trq.Number) - 1].throughput, READ);
		} while (trq.OP == 512);
		trqs[ntohs(trq.Number) - 1] = trq;
		SendRQ(NULL, trq.Number, ACK);
		block[ntohs(trq.Number) - 1].flag = -1;
		break;
	}
	
	
	for(int i=0; i<ntohs(trq.Number);i++)
	{
		for (int j = 0; j < trqs[i].OP && trqs[i].Data[j] != EOF;j++) {
			fputc(trqs[i].Data[j], fp);
			//cout<<"i:"<<i<<"  j:"<<j<<"	trqs[i].Data[j]: "<< trqs[i].Data[j] <<endl;
		}
	}
	for (int i = 1; i < ntohs(trq.Number); i++)
	{
		RecordLog(outfile, file, i, block[i].throughput, READ);
	}

}

void Upload(FILE *fp,char *file) {
	struct timeval tv;
	FD_SET readfds;
	int record = 1,len;		//record from 1~n
	do
	{
		len = initbuf(fp,trq.Data);
		trq.OP = TRAN;
		trq.Number = htons(record);

		auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		double flen = (double)trq.OP;

		SendPacket(&trq, len + 4);
		for (int i = 0; i < 10; i++) {
			FD_ZERO(&readfds);
			FD_SET(sock, &readfds);
			tv.tv_sec = 3;
			tv.tv_usec = 0;
			select(sock, &readfds, NULL, NULL, &tv);

			if (FD_ISSET(sock, &readfds)) {
				//未超时
				RecvPack();
				break;
				//if (arq.Number != record) {
				//	//发送的非arq包,所以未更新
				//	//sendto(sock, (char *)&trq, len + 4, 0, (struct sockaddr*)&serv_Addr, sizeof(sockaddr));
				//}
				//else {
				//	break;
				//}
			}
			//超时处理

			else {
				std::cout << "第" << record << "个包第" << i << "次重传中..." << std::endl;
				SendPacket(&trq, len + 4);
			}
		}

		auto _time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time;
		double f_time = (double)_time;
		block[record].flag = 0x1;
		block[record].throughput = (double)1000 * flen / f_time;
		//cout << "Throughput is " << block[record].throughput << "MB/s\n";
		memset(&trq, 0, sizeof(trq));
		
		record++;
	} while (len == 512);
	
	for (int i = 1; i < record; i++) {
		RecordLog(outfile, file, i, block[i].throughput, READ);
	}
	block[record].flag = -1;
}

//function 1
BOOL RequestRRQ() {
	char file[30] = {0},mode; int recv; FILE *fp=NULL;

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

		if (file[0] == 'q' || mode == 'q') {
			fclose(fp);
			exit(0);
		}
		else if (!file) {
			Strtmp.Format("Input Error!");
			MessageBox(NULL, Strtmp, "Error", MB_OK);
		}
		else {	
			//choose the mode of RRQ
			switch (mode)
			{
			case '1':
				SendRQ(file, RT, READ);
				fp = fopen(file, "wt+");
				break;
			case '2':
				SendRQ(file, RB, READ);
				fp = fopen(file, "wb+");
			default:
				break;
			}
			if (!fp) {
				Strtmp.Format("Open file %s Error!",file);
				MessageBox(NULL, Strtmp, "File Error", MB_OK);
				exit(0);
			}

			recv = RecvPack();
			if (recv == PACKET_ERR || recv == ERROR) {
				ShowError(recv);
				system("pause");
				system("cls");
			}
			else {
				serv_Addr.sin_port = resend_addr.sin_port;
				Download(fp,file);
				Strtmp.Format("Download %s Ends!", file);
				MessageBox(NULL, Strtmp, "Download", MB_OK);
				fclose(fp);
				summary(block);
				serv_Addr.sin_port = htons(69);
				system("pause");
				system("cls");
			}
		}
	}	
	
	return TRUE;
}

//function 2
BOOL RequestWRQ() {
	char file[30] = { 0 }, mode; int recv; FILE *fp = NULL;

	while (1) {
		cout << "Upload files\n\n";
		cout << "Input the filename:\n";
		cout << "send 'q' to quit\n";
		cin >> file;
		system("cls");
		cout << "Choose the mode and input the number\n\n";
		cout << "1 : netascii\n";
		cout << "2 : octet\n";
		cout << "send 'q' to quit\n";
		cin >> mode;

		if (file[0] == 'q' || mode == 'q') exit(0);
		else if (!file) {
			Strtmp.Format("Input Error!");
			MessageBox(NULL, Strtmp, "Error", MB_OK);
		}
		else {

			//choose the mode of RRQ
			memset(&arq, 0, sizeof(ARQ));
			switch (mode)
			{
			case '1':
				SendRQ(file, RT, WTIRE);
				fp = fopen(file, "rt+");
				break;
			case '2':
				SendRQ(file, RB, WTIRE);
				fp = fopen(file, "rb+");
			default:
				break;
			}
			if (!fp) {
				Strtmp.Format("Open file %s Error!", file);
				MessageBox(NULL, Strtmp, "File Error", MB_OK);
				exit(0);
			}

			recv = RecvPack();
			if (recv == PACKET_ERR || recv == ERROR) {
				ShowError(recv);
				system("pause");
				system("cls");
			}
			else {
				//change to the new port
				serv_Addr.sin_port = resend_addr.sin_port;
				Upload(fp,file);
				Strtmp.Format("Upload %s Ends!", file);
				MessageBox(NULL, Strtmp, "Upload", MB_OK);
				fclose(fp);
				summary(block);
				serv_Addr.sin_port = htons(69);
				system("cls");
			}
		}
	}
	fclose(fp);
	return TRUE;
}

void menu() {
	cout << "A Simple TFMP Client:\n\n";
	cout << "1.Download Files\n";
	cout << "2.Uploads Files\n";
	cout << "0.Exit\n";
}

int init() {


	outfile.open("log.txt", ios::app | ios::in | ios::out);

	memset(&block, 0, sizeof(Block));
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		MessageBox(NULL, "WSAStartup Error!", "Error", MB_OK);
		return 1;
	}
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		MessageBox(NULL, "socket Error!", "Error", MB_OK);
		Strtmp.Format("ErrorCode: %d", WSAGetLastError());
		MessageBox(NULL, Strtmp, "Error", MB_OK);
		return 1;
	}

	memset(&ptrq_error, 'x', sizeof(ptrq_error) - 1);
	char *tmp1 = (char *)&ptrq_error;
	tmp1[sizeof(ptrq_error) - 1] = 0;

	char ip[20];
	int port,tmp;
	
	//绑定发送接收端口
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(10000);
	bind_addr.sin_addr.s_addr=INADDR_ANY;
	bind(sock, (struct sockaddr*)&bind_addr, sizeofsockaddr);
	bind(sockrecv, (struct sockaddr*)&bind_addr, sizeofsockaddr);
	
	while (1) {
		while (1)		//命令行界面
		{
			memset(&serv_Addr, 0, sizeof(serv_Addr));  //清零
			system("cls");
			cout << "请输入服务器IP: ";
			cin >> ip;
			cout << "请输入服务器端口号: ";
			cin >> port;

			serv_Addr.sin_family = PF_INET;
			serv_Addr.sin_addr.s_addr = inet_addr(ip);
			serv_Addr.sin_port = htons(port);
			if (port != 69) {
				cout << "端口号错误,应为69\n";		//添加这一句在这里是表明要进行输入检查
				Sleep(1500);
			}

			else {
				cout << "初始化完成!\n";
				Sleep(1500);
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

	
	system("cls");
	
	system("pause");
}


int main()
{
	
	init();
	closesocket(sock);
	WSACleanup();
	outfile.close();
	return 0;
}

