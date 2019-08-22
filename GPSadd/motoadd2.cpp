#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>
#include "motoh.h"


using namespace std;

#define dsize 500
#define dsize2 400000
#define csize 21

vector<string> strvec;

string dailyid;
double Sx = 0, Sy = 0, Ex, Ey;
double dis;
double sp = 0, sp2 = 0, St = 0, Et = 0;
double tdif = 0;
double mindis, minsp, minsp2,foac=0;

int rnum = 0;
int flagini = 0;
int flagfir = 0;

ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;

int flagimp = 0;

int CID, CSec, CLat, CLon, CSta, CAc, CSp, CCo, CDoW;

int imp(void) {
	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "Dailyid") {
			CID = i;
		}
		else if (strvec[i] == "SecondH") {
			CSec = i;
		}
		else if (strvec[i] == "Latitude") {
			CLat = i;
		}
		else if (strvec[i] == "Longitude") {
			CLon = i;
		}
		else if (strvec[i] == "Logtype_subcategory") {
			CSta = i;
		}
		else if (strvec[i] == "Accuracy") {
			CAc = i;
		}
		else if (strvec[i] == "Speed") {
			CSp = i;
		}
		else if (strvec[i] == "Course") {
			CCo = i;
		}
		else if (strvec[i] == "DayofWeek") {
			CDoW = i;
		}
	}
	return 0;
}

int outpn(void) {//outp‚Ö‚Ì‘}“ü
	
	dailyid = strvec[CID];

	St = Et;
	Et = stod(strvec[CSec]);
	tdif = Et - St;

	Sx = Ex;
	Sy = Ey;
	
	xy(stod(strvec[CLon]), stod(strvec[CLat]));
	Ex = x;
	Ey = y;

	dis = sqrt(pow(Sx - Ex, 2) + pow(Sy - Ey, 2));
	mindis = dis - stod(strvec[CAc]) - foac;
	foac = stod(strvec[CAc]);
	if (mindis < 0) {
		mindis = 0;
	}
	
	sp = sp2;
	minsp = minsp2;

	if (St == Et) {
		St = Et - 1;
	}
	sp2 = dis / (2.0 * (Et - St));
	minsp2 = mindis / (2.0 * (Et - St));
	

	if (flagfir == 1) {
		if (flagini == 0) {
			sp += sp2;
			minsp += minsp2;
			flagfir = 2;
		}
		else {
			sp += sp;
			minsp += minsp;
			flagfir = 3;
		}
	}
	else if (flagfir == 2) {
		if (flagini == 0) {
			sp += sp2;
			minsp += minsp2;
		}
		else {
			sp += sp;
			minsp += minsp;
			flagfir = 3;
		}
	}
	else if (flagfir == 3) {
		if (flagini == 0) {
			sp = sp2 * 2;
			minsp = minsp2 * 2;
			flagfir = 1;
		}
		else {
			flagfir = 4;
		}
	}
	else if (flagfir == 4) {
		if (flagini == 0) {
			sp = sp2 * 2;
			minsp = minsp2 * 2;
			flagfir = 1;
		}
	}
	else if (flagfir == 5) {
		sp = sp2 * 2;
		minsp = minsp2 * 2;
		flagfir = 1;
	}


	//DailyID, DayofWeek, SecondH, SecondDif Latitude, Longtitude, x, y, State, Accuracy, Speed, Course, CityCode, Distance, Speed2, minSpeed
	if (flagfir == 0) {
		flagfir = 5;
	}
	else if (flagfir == 4) {
		outfile << "1point,1point" << "\n";
	}
	else {
		outfile << sp << "," << minsp << "\n";
	}
	outfile << strvec[CID] << "," << strvec[CDoW] << ",";
	outfile << strvec[CSec] << ",";
	if (flagini == 0) {
		outfile << tdif << ",";
	}
	else {
		outfile << "Start,";
	}
	outfile << strvec[CLat] << "," << strvec[CLon] << ",";
	outfile << Ex << "," << Ey << ",";
	outfile << strvec[CSta] << "," << strvec[CAc] << "," << strvec[CSp] << "," << strvec[CCo] << ",";
	if (flagini == 0) {
		outfile << dis << ",";

	}
	else {
		outfile << "Start" << ",";
	}

	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("m12iOSac30ikaadd.csv");


	strvec.resize(csize);

	outfile << "DailyID,DayofWeek,SecondH,SecondDif,Latitude,Longtitude,x,y,State,Accuracy,Speed,Course,Distance,Speed2,minSpeed" << "\n";

	cout << "import" << "\n";
	error = fopen_s(&fp, "m12iOSac30ika.csv", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL) {

		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}

		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		//cout << "sp" << endl;

		if (flagimp == 0) {
			cout << strvec[20];
			dailyid = strvec[0];
			flagimp = 1;
			imp();
		}
		else if (strvec[0] == dailyid) {

			//if (rnum > 1210000) {
			//cout << "same" << endl;
			//}
			flagini = 0;
			outpn();
			//cout << "b" << endl;

		}
		else {

			//if (rnum > 1210000) {
			//cout << "dif" << endl;
			//}
			flagini = 1;
			outpn();

			//++elnum;
			//if (elnum % 100 == 0) {
			//cout << elnum << endl ;
			//}
			//cout << i <<"c"<< endl;

		}
		if (rnum % 500000 == 0) {

			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}
		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		++rnum;
	}
	sp += sp;
	outfile << sp << "\n";

	fclose(fp);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "\n";
	return 0;
}