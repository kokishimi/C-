#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;

constexpr auto csize = 9;//������
constexpr auto Hsize = 12;//Home_CC��
constexpr auto Wsize = 15;//WorkPlace_CC��

vector<string> strvec;//���݂̃f�[�^
int HDummy[Hsize];//Home_CC�̃_�~�[�ϐ�
int WDummy[Wsize];//WorkPlace_CC�̃_�~�[�ϐ�

ofstream outfile;

FILE* fp;
char buf[300] = { 0 };
errno_t error;
char* ctx;

int CHCC, CWPCC;

int flagimp = 0;//0:1�s��
int rnum = 0;//�s��

int imp(void) {
	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "Home_CityCode") {
			CHCC = i;
		}
		else if (strvec[i] == "WorkPlace_CityCode") {
			CWPCC = i;
		}
	}
	return 0;
}

int CityCode(void) {//CityCode�_�~�[�̌v�Z
	if (strvec[CHCC] != " " && strvec[CHCC] != "") {
		for (int i = 0; i < 10; ++i) {
			if (stoi(strvec[CHCC]) == i+11101) {
				HDummy[i] = 1;
				break;
			}
		}
		if (stoi(strvec[CHCC]) == 11203) {
			HDummy[10] = 1;
		}
		if (stoi(strvec[CHCC]) == 11219) {
			HDummy[11] = 1;
		}
	}

	if (strvec[CWPCC] != " " && strvec[CWPCC] != "") {
		for (int i = 0; i < 10; ++i) {
			if (stoi(strvec[CWPCC]) == i + 11101) {
				WDummy[i] = 1;
				return 0;
			}
		}
		if (stoi(strvec[CWPCC]) == 11203) {
			WDummy[10] = 1;
			return 0;
		}
		for (int i = 0; i < 4; ++i) {
			if (stoi(strvec[CWPCC]) == i + 13101) {
				WDummy[i+11] = 1;
				return 0;
			}
		}
	}

	return 0;
}

int outpn(void) {//outp�ւ̑}��

	for (int i = 0; i < Hsize; ++i) {
		HDummy[i] = 0;
	}
	for (int i = 0; i < Wsize; ++i) {
		WDummy[i] = 0;
	}

	CityCode();

	for (int i = 0; i < csize; ++i) {
		outfile << strvec[i] << ",";
	}
	for (int i = 0; i < Hsize; ++i) {
		outfile << HDummy[i] << ",";
	}
	for (int i = 0; i < Wsize - 1; ++i) {
		outfile << WDummy[i] << ",";
	}
	outfile << WDummy[Wsize - 1] << "\n";

	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("2018456ac30IDCCDummy.csv");

	strvec.resize(csize);

	outfile << "DailyID,Month,DayofWeek,Home_CityCode,WorkPlace_CityCode,pointsize,Distance,log10Dist,time,";
	outfile << "HNishi,HKita,HOmiya,HMinuma,HTyuo,HSakura,HUrawa,HMinami,Hmidori,HIwatsuki,HKawaguchi,HAgeo,";
	outfile << "WNishi,WKita,WOmiya,WMinuma,WCyuo,WSakura,WUrawa,WMinami,Wmidori,WIwatsuki,WKawaguchi,";
	outfile << "WChiyoda,WChuoT,WMinato,WShinjuku\n";

	cout << "import" << "\n";
	error = fopen_s(&fp, "2018456ac30IDsmall.csv", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL) {

		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();

		if (flagimp == 0) {//1�s��
			cout << strvec[csize-1];
			imp();
			flagimp = 1;
		}
		else {//����ȊO
			outpn();
		}
		if (rnum % 10000 == 0) {

			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}
		++rnum;
	}

	fclose(fp);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "\n";
	return 0;
}