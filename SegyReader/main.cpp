#include <iostream>
#include "INSMSegyReader.h"
#include <time.h>
using namespace std;

int main() {
	/*CINSMSegyReader st1("D:\\Attributes\\tah67_ant.sgy", 1);
	time_t begin_t1 = clock();
	st1.ReadSegyFile();

	while (!st1.isFinish());
	time_t finish_t1 = clock();
	cout << "Single Thread cost " << (double)(finish_t1 - begin_t1) / CLOCKS_PER_SEC << " s" << endl;
*/

	CINSMSegyReader* st = new CINSMSegyReader("D:\\Attributes\\tah67_ant.sgy", 1);
	time_t begin_t = clock();
	st->ReadSegyFile();

	while (!st->isFinish());
	time_t finish_t = clock();
	cout << "Single Thread cost " << (double)(finish_t - begin_t) / CLOCKS_PER_SEC << " s" << endl;

	//system("pause");
	delete st;

	st = new CINSMSegyReader("D:\\Attributes\\tah67_ant.sgy", 2);
	begin_t = clock();
	st->ReadSegyFile();

	while (!st->isFinish());
	finish_t = clock();
	cout << "2 Thread cost " << (double)(finish_t - begin_t) / CLOCKS_PER_SEC << " s" << endl;

	//system("pause");
	delete st;

	st = new CINSMSegyReader("D:\\Attributes\\tah67_ant.sgy", 3);
	begin_t = clock();
	st->ReadSegyFile();

	while (!st->isFinish());
	finish_t = clock();
	cout << "3 Thread cost " << (double)(finish_t - begin_t) / CLOCKS_PER_SEC << " s" << endl;

	//system("pause");
	delete st;

	st = new CINSMSegyReader("D:\\Attributes\\tah67_ant.sgy", 4);
	begin_t = clock();
	st->ReadSegyFile();

	while (!st->isFinish());
	finish_t = clock();
	cout << "4 Thread cost " << (double)(finish_t - begin_t) / CLOCKS_PER_SEC << " s" << endl;

	//system("pause");
	delete st;

	st = new CINSMSegyReader("D:\\Attributes\\tah67_ant.sgy", 5);
	begin_t = clock();
	st->ReadSegyFile();

	while (!st->isFinish());
	finish_t = clock();
	cout << "5 Thread cost " << (double)(finish_t - begin_t) / CLOCKS_PER_SEC << " s" << endl;

	//system("pause");
	delete st;

	CINSMSegyReader st1("D:\\Attributes\\tah67_ant.sgy", 6);
	time_t begin_t1 = clock();
	st1.ReadSegyFile();

	while (!st1.isFinish());
	time_t finish_t1 = clock();
	cout << "Muti Thread cost " << (double)(finish_t1 - begin_t1) / CLOCKS_PER_SEC << " s" << endl;
	
	std::cout << st1.getTraceCount() << "," << st1.getiLineCount() << "," << st1.getxLineCount() << "," << st1.getiLineStart() << "," << st1.getxLineStart();
	auto data = st1.getData()[0];
	std::cout << std::endl;
	for (auto v : data) {
		std::cout << v.a << " ";
	}
	std::cout << std::endl;
	system("pause");
	return 0;
}