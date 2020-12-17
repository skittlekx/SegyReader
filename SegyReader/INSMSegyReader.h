#pragma once
#include <string>
#include <vector>
#include <iostream>

union HeadData { unsigned short a; char s[2]; };
union Data { unsigned int i; float a; char s[4]; };

class CINSMSegyReader;

struct ThreadParam {
	int id;
	int count;
	int blockSize;
	CINSMSegyReader* obj;
};

class CINSMSegyReader
{
private:
	std::string fileName;
	int ThreadNum;
	int isok;
	int xline, iline;
	int traceCount, xlineCount, ilineCount;
	float*Thresholds[2];
	float vMin, vMax;
	int si;//采样点数
	int dataType;
	//std::vector<std::vector<Data>> data;
	float** data;
	inline void add() {
		isok++; 
		//std::cout << "233: "<<isok << std::endl ;
	}

	bool isReadHeader;
	float(*dataTransfrom)(Data);//函数指针，读取IEEE float 或者 IBM float
public:
	CINSMSegyReader();
	CINSMSegyReader(const char* _name, int _n);

	~CINSMSegyReader();

	bool ReadSegyHeader();
	bool ReadSegyFile();
	int getSize();
	static void pRead(ThreadParam * para);
	static float IBM2IEEE(Data);
	static float IEEEfloat(Data);

	inline bool isFinish() {
		return isok >= ThreadNum;
	}

	inline void swap4(Data &data) {
		char temp = data.s[3];
		data.s[3] = data.s[0];
		data.s[0] = temp;

		temp = data.s[1];
		data.s[1] = data.s[2];
		data.s[2] = temp;

	}
	inline void swap2(HeadData &data) {
		char temp = data.s[1];
		data.s[1] = data.s[0];
		data.s[0] = temp;
	}
	inline void setData(int i, float* _data) {
		data[i] = _data;
	}
	inline int getTraceCount() {
		return traceCount;
	}
	inline int getiLineCount() {
		return ilineCount;
	}
	inline int getxLineCount() {
		return xlineCount;
	}
	inline int getiLineStart() {
		return iline;
	}
	inline int getxLineStart() {
		return xline;
	}
	inline int getSampleSize() {
		return si;
	}
	inline float getMaxValue() {
		return vMax;
	}
	inline float getMinValue() {
		return vMin;
	}
	/*std::vector<std::vector<Data>>& getData() {
		std::vector<std::vector<Data>>& res = data;
		return res;
	}*/
	float ** _NormlizeData();
	float ** getData() {
		return data;
	}

	void UpdataThreshold();
};

