#include "INSMSegyReader.h"

#include <fstream>
#include <thread>
using namespace std;



CINSMSegyReader::CINSMSegyReader()
{
}

CINSMSegyReader::CINSMSegyReader(const char* _name, int _n)
	:fileName(_name)
	,ThreadNum(_n)
	,isok(0)
{
}


CINSMSegyReader::~CINSMSegyReader()
{
}

int CINSMSegyReader::getSize()
{
	struct stat st;
	memset(&st, 0, sizeof(st));
	stat(fileName.c_str(), &st);
	return st.st_size;
}

void CINSMSegyReader::pRead(ThreadParam * para) {
	int start = 3600 + para->id * para->blockSize * para->count;

	ifstream ifile;
	ifile.open(para->obj->fileName, ios::binary | ios::in);
	if (ifile.fail())
		return;

	ifile.seekg(start, ios::beg);

	for (int i = 0; i < para->count; i++) {
		ifile.seekg(240, ios::cur);
		vector<Data> temp;
		if (ifile.eof())
			break;
		for (int j = 0; j < para->obj->si.a; j++) {
			Data buffer;
			ifile.read(buffer.s, 4);
			para->obj->swap4(buffer);
			buffer.a = IBM2IEEE(buffer);
			temp.push_back(buffer);
		}
		//if(i % 1000 == 0)std::cout << i << std::endl;
		para->obj->setData(para->id * para->count + i, temp);
	}
	//std::cout << "Done" << std::endl;
	para->obj->add();
	ifile.close();
	delete para;
}


float CINSMSegyReader::IBM2IEEE(Data in)
{
	unsigned int fraction = in.i;
	int exponent;
	int sign;
	Data ui;

	//����������Ƿ�����ֽ�ת��
	//System.Array.Reverse(bb);

	// @ ��ʶ����λ   
	// # ��ʶ����λ   
	// * ��ʶβ��λ
	//IBM�������� SEEEEEEE MMMMMMMM MMMMMMMM MMMMMMMM        Value = (-1)^s * M * 16^(E-64)
	//IEEE��������SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM        Value = (-1)^s * (1 +  M) * 2^(E-127)


	sign = (int)(fraction >> 31);           // ��ȡ����λ;
	fraction <<= 1;                         // �����Ƴ�����λ���Ҳ���0;
	exponent = (int)(fraction >> 25);       // ��ȡ����;
	fraction <<= 7;                         //�Ƴ�����λ �� ���� ʣ��Ĳ��֣�β������;

	/*
	 * ���β������Ϊ0,��˵���������ض�ֵ��0�������
	 * ��ָ��=127��˵����ǰ���������; ��Ӧ��IEEE�����ʱ��ָ��Ϊ255
	 * ��ָ��=0,˵����ǰ��Ϊ0;��Ӧ��IEEEΪ0ʱ��ָ��Ϊ255
	 * ��0<ָ��<127�����ݹ�ʽ (-1)^s * M * 16^(E-64) ��MΪ0,���������0����Ӧ��IEEEΪ0ʱ��ָ��Ϊ255
	 */
	if (fraction == 0)
	{
		if (exponent != 127)
			exponent = 0;
		else
			exponent = 255;
		ui.i = (unsigned int)((exponent << 23) | (sign << 31));
		ui.i = ui.i | (fraction >> 9);
		return ui.a;
	}

	// ��IBM �������Ľ���ת���� IEEE �Ľ��룺(exp - 64) * 4 + 127 - 1 == exp * 4 - 256 + 126 == (exp << 2) - 130
	// IEEE����= IBM���� * 4 -130
	exponent = (exponent << 2) - 130;

	// ��β����񻯣���Ϊ�������ܱ�ʾ����С����1/16�����Թ�񻯹�������������Ρ�
	while (fraction < 0x80000000)
	{
		--exponent;
		fraction <<= 1;
	}


	if (exponent <= 0)  //���������ָ������С����  0<=E��ieee��<=254
	{
		//if (exponent < -24)
		//{
		//    // complete underflow - return properly signed zero
		//    fraction = 0;
		//}
		//else
		//{
		//    // partial underflow - return denormalized number
		//    fraction >>= -exponent;
		//}

		exponent = 0;
		fraction = 0;
	}
	else if (exponent >= 255)   //�������:ָ�����ܴ���255;��ʾ�����
	{
		fraction = 0;
		exponent = 255;
	}
	else //IEEEβ�� = IBMβ�� * 2 -1;�൱������һλ
	{
		fraction <<= 1;
	}

	ui.i = (unsigned int)((exponent << 23) | (sign << 31));
	ui.i = ui.i | (fraction >> 9);
	return ui.a;
}

bool CINSMSegyReader::ReadSegyFile() 
{
	ifstream ifile;
	ifile.open(fileName, ios::binary | ios::in);

	isok = 0;
	if (ifile.fail()) {
		return false;
	}

	//3221-3222�ֽ� ��ȡ���ݵ���������
	ifile.seekg(3220L, ios::beg);
	ifile.read(si.s, 2);
	char ch = si.s[0];
	si.s[0] = si.s[1];
	si.s[1] = ch;
	
	//union HeadData sp;//���������
	////3217-3218�ֽ� ��ȡ���ݲ�����
	//ifile.seekg(3216L, ios::beg);
	//ifile.read(sp.s, 2);
	//ch = sp.s[0];
	//sp.s[0] = sp.s[1];
	//sp.s[1] = ch;
	
	
	int traceSize = si.a * 4 + 240;
	traceCount = (getSize() - 3600) / traceSize;//������� 

	int count = traceCount / ThreadNum;
	
	data.resize(traceCount);

	Data buffer;
	ifile.seekg(3608, ios::beg);
	ifile.read(buffer.s, 4);
	swap4(buffer);
	int is = buffer.i;

	ifile.seekg(3620, ios::beg);
	ifile.read(buffer.s, 4);
	swap4(buffer);
	int xs = buffer.i;

	ifile.seekg(3608 + (traceCount - 1) * traceSize, ios::beg);
	ifile.read(buffer.s, 4);
	swap4(buffer);
	int ie = buffer.i;

	ifile.seekg(3620 + (traceCount - 1) * traceSize, ios::beg);
	ifile.read(buffer.s, 4);
	swap4(buffer);
	int xe = buffer.i;

	xline = xs;
	iline = is;
	xlineCount = xe - xs + 1;
	ilineCount = ie - is + 1;

	ifile.close();
	for (int i = 0; i < ThreadNum; i++) {
		ThreadParam* para = new ThreadParam{
			i,
			count,
			traceSize,
			this
		};

		thread f(pRead, para);
		f.detach();
	}
	
	//while (!isOk());

	return true;
}