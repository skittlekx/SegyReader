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
	,isReadHeader(false)
	,vMax(-FLT_MAX)
	,vMin(FLT_MAX)
{
	Thresholds[0] = nullptr;
	Thresholds[1] = nullptr;
	dataTransfrom = NULL;
}


CINSMSegyReader::~CINSMSegyReader()
{
	if (data) {
		for (int i = 0; i < traceCount; i++)
			delete[] data[i];
		delete[] data;
	}
	if (Thresholds[0]) {
		delete Thresholds[0];
		Thresholds[0] = nullptr;
	}

	if (Thresholds[1]) {
		delete Thresholds[1];
		Thresholds[1] = nullptr;
	}
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
		float* temp = new float[para->obj->si];
		if (para->id * para->count + i >= para->obj->traceCount || ifile.eof())
			break;
		for (int j = 0; j < para->obj->si; j++) {
			Data buffer;
			ifile.read(buffer.s, 4);
			para->obj->swap4(buffer);
			temp[j] = para->obj->dataTransfrom(buffer);

			para->obj->Thresholds[0][para->id] = 
				temp[j] < para->obj->Thresholds[0][para->id] ? temp[j] : para->obj->Thresholds[0][para->id];
			para->obj->Thresholds[1][para->id] = 
				temp[j] > para->obj->Thresholds[1][para->id] ? temp[j] : para->obj->Thresholds[1][para->id];
		}
		//if(i % 1000 == 0)std::cout << i << std::endl;
		para->obj->setData(para->id * para->count + i, temp);
	}
	//std::cout << "Done" << std::endl;
	para->obj->add();
	ifile.close();
	delete para;
}

void CINSMSegyReader::UpdataThreshold() {
	for (int i = 0; i < ThreadNum; i++) {
		vMax = Thresholds[1][i] > vMax ? Thresholds[1][i] : vMax;
		vMin = Thresholds[0][i] < vMin ? Thresholds[0][i] : vMin;
	}
}

float CINSMSegyReader::IBM2IEEE(Data in)
{
	unsigned int fraction = in.i;
	int exponent;
	int sign;
	Data ui;

	//根据情况看是否进行字节转换
	//System.Array.Reverse(bb);

	// @ 标识符号位   
	// # 标识阶数位   
	// * 标识尾数位
	//IBM浮点数： SEEEEEEE MMMMMMMM MMMMMMMM MMMMMMMM        Value = (-1)^s * M * 16^(E-64)
	//IEEE浮点数：SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM        Value = (-1)^s * (1 +  M) * 2^(E-127)


	sign = (int)(fraction >> 31);           // 获取符号位;
	fraction <<= 1;                         // 左移移出符号位，右侧填0;
	exponent = (int)(fraction >> 25);       // 获取阶数;
	fraction <<= 7;                         //移出符号位 和 阶数 剩余的部分：尾数部分;

	/*
	 * 如果尾数部分为0,则说明该数是特定值：0或者无穷。
	 * 当指数=127，说明当前数是无穷大; 对应的IEEE无穷大时，指数为255
	 * 当指数=0,说明当前数为0;对应的IEEE为0时，指数为255
	 * 当0<指数<127，根据公式 (-1)^s * M * 16^(E-64) ，M为0,则最后结果是0；对应的IEEE为0时，指数为255
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

	// 将IBM 浮点数的阶码转化成 IEEE 的阶码：(exp - 64) * 4 + 127 - 1 == exp * 4 - 256 + 126 == (exp << 2) - 130
	// IEEE阶码= IBM阶码 * 4 -130
	exponent = (exponent << 2) - 130;

	// 将尾数规格化，因为浮点数能表示的最小数是1/16，所以规格化过程最多左移三次。
	while (fraction < 0x80000000)
	{
		--exponent;
		fraction <<= 1;
	}


	if (exponent <= 0)  //下限溢出，指数不能小于零  0<=E（ieee）<=254
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
	else if (exponent >= 255)   //上限溢出:指数不能大于255;表示无穷大；
	{
		fraction = 0;
		exponent = 255;
	}
	else //IEEE尾码 = IBM尾码 * 2 -1;相当于左移一位
	{
		fraction <<= 1;
	}

	ui.i = (unsigned int)((exponent << 23) | (sign << 31));
	ui.i = ui.i | (fraction >> 9);
	return ui.a;
}

float CINSMSegyReader::IEEEfloat(Data in) {
	return in.a;
}

bool CINSMSegyReader::ReadSegyHeader() {
	ifstream ifile;
	ifile.open(fileName, ios::binary | ios::in);

	isok = 0;
	if (ifile.fail()) {
		return false;
	}

	HeadData th;

	//3221-3222字节 读取数据道采样点数
	ifile.seekg(3220L, ios::beg);
	ifile.read(th.s, 2);
	swap2(th);
	si = th.a;
	
	//3225-3225字节 读取数据类型
	//1  4 byte IBM浮点数
	//2  4字节，两互补整数
	//3  2字节，两互补整数
	//4,6,7  不使用
	//5  4 byte IEEE浮点数
	//8  1字节，两互补整数
	ifile.seekg(3224L, ios::beg);
	ifile.read(th.s, 2);
	swap2(th);
	dataType = th.a;

	//union HeadData sp;//定义采样率
	////3217-3218字节 读取数据采样率
	//ifile.seekg(3216L, ios::beg);
	//ifile.read(sp.s, 2);
	//ch = sp.s[0];
	//sp.s[0] = sp.s[1];
	//sp.s[1] = ch;


	int traceSize = si * 4 + 240;
	traceCount = (getSize() - 3600) / traceSize;//计算道数 

	int count = traceCount / ThreadNum;

	data = new float*[traceCount];

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
	isReadHeader = true;
	return true;
}

bool CINSMSegyReader::ReadSegyFile() 
{
	if (!isReadHeader && !ReadSegyHeader())
		return false;

	switch (dataType)
	{
	case 1:
		dataTransfrom = IBM2IEEE;
		break;
	case 5:
	default:
		dataTransfrom = IEEEfloat;
		break;
	}
	int count = traceCount / ThreadNum + 1;
	int traceSize = si * 4 + 240;

	Thresholds[0] = new float[ThreadNum];
	Thresholds[1] = new float[ThreadNum];

	for (int i = 0; i < ThreadNum; i++) {
		Thresholds[0][i] = FLT_MAX;
		Thresholds[1][i] = -FLT_MAX;
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


float ** CINSMSegyReader::_NormlizeData() {
	return nullptr;
}