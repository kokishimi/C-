#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>
#include "latlong.h"


using namespace std;

constexpr auto dsize = 500;//一人のデータ数
constexpr auto csize = 23;//属性数

vector<string> record;//一個前のデータ
vector<string> strvec;//現在のデータ

string dailyid = "b";
string OS = "a";
int point, pointac;
double Sx, Sy, Ex, Ey;
int St, Et;
double dis, dis2, meansp, maxsp, meanac, sumac, worstac;
double Sx2, Sy2, Ex2, Ey2;
int dis2f = 0;
string meansp2, maxsp2;
string loop;
double Sac, Eac;

int rnum = 0;//行目
int flag = 0;//最後のみ1

ofstream outfile;

FILE* fp;
char buf[300] = { 0 };
errno_t error;
char* ctx;

int i2 = 0;//3行目から1

int flagimp = 0;//0:1行目、1:その他
int CID, CYear, CMon, CDay, CDoW, CHour, CMin, CLat, CLon, COS, CAc, CSp, CHCC, CWPCC;

int imp(void) {
	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "Dailyid") {
			CID = i;
		}
		else if (strvec[i] == "Year") {
			CYear = i;
		}
		else if (strvec[i] == "Month") {
			CMon = i;
		}
		else if (strvec[i] == "Day") {
			CDay = i;
		}
		else if (strvec[i] == "DayofWeek") {
			CDoW = i;
		}
		else if (strvec[i] == "Hour") {
			CHour = i;
		}
		else if (strvec[i] == "Minute") {
			CMin = i;
		}
		else if (strvec[i] == "Latitude") {
			CLat = i;
		}
		else if (strvec[i] == "Longitude") {
			CLon = i;
		}
		else if (strvec[i] == "Logtype_subcategory") {
			COS = i;
		}
		else if (strvec[i] == "Accuracy") {
			CAc = i;
		}
		else if (strvec[i] == "Speed") {
			CSp = i;
		}
		else if (strvec[i] == "Home_CityCode") {
			CHCC = i;
		}
		else if (strvec[i] == "WorkPlace_CityCode") {
			CWPCC = i;
		}
	}
	return 0;
}

int rown(void) {//繰り返し計算
	if (OS == "unknown") {
		if (strvec[COS] == "move" || strvec[COS] == "arrival" || strvec[COS] == "departure") {
			OS = "iOS";
		}
		else if (strvec[COS] == "timer") {
			OS = "Android";
		}
	}

	++point;
	++pointac;

	if (strvec[CSp] != "" && strvec[CSp] != " ") {
		if (maxsp < stod(strvec[CSp])) {
			maxsp = stod(strvec[CSp]);
		}
	}

	if (strvec[CAc] != "" && strvec[CAc] != " ") {
		sumac += stod(strvec[CAc]);

		if (worstac < stod(strvec[CAc])) {
			worstac = stod(strvec[CAc]);
		}
	}
	else {
		--pointac;
	}
	if (dis2f != 1) {
		xy(stod(record[CLon]), stod(record[CLat]));
		Sx2 = x;
		Sy2 = y;
		xy(stod(strvec[CLon]), stod(strvec[CLat]));
		Ex2 = x;
		Ey2 = y;
		dis2 += sqrt(pow(Sx2 - Ex2, 2) + pow(Sy2 - Ey2, 2));

	}
	dis2f = 0;

	return 0;
}

int ini(void) {//初期化
	dailyid = strvec[CID];
	OS = "unknown";
	point = 0;
	pointac = 0;
	xy(stod(strvec[CLon]), stod(strvec[CLat]));
	Sx = x;
	Sy = y;

	St = stoi(strvec[CHour])*60+stoi(strvec[CMin]);
	if (strvec[CSp] == "" || strvec[CSp] == " ") {
		maxsp = 0.0;
	}
	else {
		maxsp = stod(strvec[CSp]);
	}
	sumac = 0;

	if (strvec[CAc] == "" || strvec[CAc] == " ") {
		worstac = 1000000.0;
	}
	else {
		worstac = stod(strvec[CAc]);
	}

	Sac = worstac;
	if (Sac == 1000000.0) {
		Sac = 2000.0;
	}
	loop = "unknown";
	dis2f = 1;
	dis2 = 0;
	rown();
	return 0;
}

int outpn(void) {//outpへの挿入

	xy(stod(record[CLon]), stod(record[CLat]));
	Ex = x;
	Ey = y;
	Et = stoi(record[CHour]) * 60 + stoi(record[CMin]);
	dis = sqrt(pow(Sx - Ex, 2) + pow(Sy - Ey, 2));
	meanac = sumac / pointac;
	if (Et == St) {
		meansp2 = "1point";
	}
	else {
		meansp2 = "2ormore";
		meansp = dis2 / ((double)Et - (double)St);
	}

	if (record[CAc] == "" || record[CAc] == " ") {
		Eac = 2000.0;
	}
	else {
		Eac = stod(record[CAc]);
	}

	if (dis < Eac + Sac) {
		loop = "loop";
	}
	else {
		loop = "not";
	}

	//DailyID,Year,Month,Day,DayofWeek,Home_CityCode,WorkPlace_CityCode,OS,pointsize,Sx,Sy,Ex,Ey,St,Et,
	//Distance,meanspeed,maxspeed,meanaccuracy,worstaccuracy,loop
	outfile << dailyid << "," << record[CYear] << "," << record[CMon] << "," << record[CDay] << "," << record[CDoW] << ",";
	outfile << record[CHCC] << "," << record[CWPCC] << "," << OS << ",";
	outfile << point << ",";
	outfile << Sx << "," << Sy << "," << Ex << "," << Ey << ",";
	outfile << St << "," << Et << ",";
	outfile << dis2 << "," ;
	if (meansp2 == "1point") {
		outfile << meansp2 << ",";
	}
	else {
		outfile << meansp << ",";
	}
	outfile << maxsp << "," << meanac << "," << worstac << "," << loop << "\n";
	
	if (flag == 0) {
		ini();
	}

	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("201806ac30ID.csv");

	record.resize(csize);
	strvec.resize(csize);

	outfile << "DailyID,Year,Month,Day,DayofWeek,Home_CityCode,WorkPlace_CityCode,OS,pointsize,Sx,Sy,Ex,Ey,St,Et,";
	outfile << "Distance,meanspeed,maxspeed,meanaccuracy,worstaccuracy,loop\n";

	cout << "import" << "\n";
	error = fopen_s(&fp, "201806ac30.csv", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL) {

		if (i2 != 0) {
			for (int i3 = 0; i3 < csize; ++i3) {
				record[i3] = strvec[i3];
			}
		}

		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();

		if (flagimp == 0) {//1行目
			cout << strvec[20];
			imp();
			flagimp = 1;
		}
		else if (OS == "a") {//2行目
			ini();
			i2 = 1;
			cout << "row2\n" << endl;

		}
		else if (strvec[CID] == dailyid) {//同一人物
			rown();
			//cout << "b" << endl;

		}
		else {//人が変わった場合
			outpn();

		}
		if (rnum % 500000 == 0) {

			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}
		++rnum;
	}

	flag = 1;
	for (int i3 = 0; i3 < csize; ++i3) {
		record[i3] = strvec[i3];
	}
	outpn();

	fclose(fp);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "\n";
	return 0;
}