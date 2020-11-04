#pragma once
#include <string>
#include <vector>

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
	int traceCount,xlineCount,ilineCount;
	union HeadData si;//采样点数
	std::vector<std::vector<Data>> data;
public:
	CINSMSegyReader();
	CINSMSegyReader(const char* _name,int _n);

	~CINSMSegyReader();

	bool ReadSegyFile();
	int getSize();
	inline void add() { isok++; }
	static void pRead(ThreadParam * para);
	static float IBM2IEEE(Data);

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
	inline void setData(int i, std::vector<Data> &_data) {
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
	std::vector<std::vector<Data>>& getData() {
		std::vector<std::vector<Data>>& res = data;
		return res;
	}
};

