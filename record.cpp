#include"record.h"


void summary(Block *pblock) {
	int count = 0;
	double max = 0.00001, min = 99999,sum=0;
	cout << "���°�����ʧ��:\n";
	++pblock;
	while (pblock->flag != -1) {
		if (pblock->flag) {		//����ɹ��İ�
			if (pblock->throughput > max) if(pblock->throughput <100) max = pblock->throughput; 
			if (pblock->throughput < min) min = pblock->throughput;
			count++;
			sum += pblock->throughput;
		}
		else {					//����ʧ�ܵİ�
			cout << ++count<<" ";
		}
		++pblock;
	}
	cout << std::setprecision(3)<<endl<<"���������Ϊ:"<< max<<"MB/s,��С������Ϊ:"<< min<<"MB/s"<<endl;
	cout << std::setprecision(3)<<"ƽ��������Ϊ:"<< sum/count<<"MB/s"<<endl;

}

void RecordLog(ofstream &outfile,char *file,int count, double throught,WORD mode) {
	outfile <<file<< ":��" << count << "������"; 
	switch (mode)
	{
	case READ:
		outfile << "����";
		break;
	case WTIRE:
		outfile << "�ϴ�";
		break;
	}
	outfile <<"�ٶ�Ϊ:"<< setw(4) << setfill('0') << setiosflags(ios::fixed) << setprecision(2) << throught <<"MB/s"<< endl;
}

void WritetoFile(FILE *fp, TRQ *ptrq) {
	int count = -1;
	while (ptrq->OP != -1) {
		for (int i = 0; i < ptrq->OP; i++) {
			fputc(ptrq->Data[i], fp);
		}
		ptrq++;
	}
}

int initbuf(FILE *fp, char *sendbuf1) {
	int count = 0;
	do {
		sendbuf1[count++] = fgetc(fp);
	} while (count < 512 && sendbuf1[count - 1] != EOF);

	return count;
}
