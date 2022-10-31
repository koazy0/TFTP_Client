// TFTP_Client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include"TFTP_Client.h"


using namespace std;


BOOL RequestWRQ() {
	TRQ TranceferRQ;
	SendWRQ();
	while (RecvTRQ(TranceferRQ)) {
		checkData();
		WriteData();
		RecordNum();
		SendACK();
		if (lenth < 512)
			closeconnction();
		break;
	}
}

BOOL RequestRRQ() {
	TRQ TranceferRQ;
	SendRRQ();
	while (RecvTRQ(TranceferRQ)) {
		checkData();
		RecordData();
		RecordNum();
		SendACK();
		if (lenth < 512)
			closeconnction();
		break;
	}
}




int main()
{
	char a[10] = "123";
	char a1[11] = "123";
	char *a2 = new(char[15]);
	strcpy(a2, a);
	cout<<strcmp(a, a1)<<endl;
	cout<<sizeof(a) << endl;
	cout<<sizeof(a1) << endl;
	cout<<sizeof(a2) << endl;
}

