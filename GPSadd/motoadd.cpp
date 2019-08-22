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
double Sx=0, Sy=0, Ex, Ey;
double dis;
double sp = 0, sp2 = 0, St = 0 , Et=0;
double tdif = 0;//’Ç‰Á

int rnum = 0;
int flagini = 0;
int flagfir = 0;

ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;


int outpn(void) {//outp‚Ö‚Ì‘}“ü
	if (flagini == 0) {
		Sx = Ex;
		Sy = Ey;
	}
	dailyid = strvec[0];

	xy(stod(strvec[10]), stod(strvec[9]));
	Ex = x;
	Ey = y;

	dis = sqrt(pow(Sx - Ex, 2) + pow(Sy - Ey, 2));

	St = Et;
	Et = stod(strvec[8]);
	tdif = Et - St;
	sp = sp2;

	if (St == Et) {
		St = Et - 1;
	}
	sp2 = dis / (2 * (Et - St));

	if (flagfir == 1) {		
		if (flagini == 0) {
			sp += sp2;
			flagfir = 2;
		}
		else {
			sp += sp;
			flagfir = 3;
		}	
	}
	else if (flagfir == 2) {	
		if (flagini == 0) {
			sp += sp2;
		}
		else {
			sp += sp;
			flagfir = 3;
		}
	}
	else if(flagfir==3){
		if (flagini == 0) {
			sp = sp2 * 2;
			flagfir = 1;
		}
		else {
			flagfir = 4;
		}
	}
	else if (flagfir == 4) {
		if (flagini == 0) {
			sp = sp2 * 2;
			flagfir = 1;
		}
	}
	else if (flagfir == 5) {
		sp = sp2 * 2;
		flagfir = 1;
	}
	

	//DailyID, Month, Day, DayofWeek, SecondH,Seconddif, Latitude, Longtitude, x, y,State Accuracy, Speed, Course, CityCode, Distance, Speed2
	if (flagfir == 0) {
		flagfir = 5;
	}
	else if(flagfir==4){
		outfile << "1point" << "\n";
	}
	else {
		outfile << sp << "\n";
	}
	outfile << strvec[0] << "," << strvec[2] << "," << strvec[3] << "," << strvec[4] << ",";
	outfile << strvec[8] << ",";
	if (flagini == 0) {
		outfile << tdif << ",";

	}
	else {
		outfile << "Start" << ",";
	}
	outfile << strvec[9] << "," << strvec[10] << ",";
	outfile << Ex << "," << Ey << ",";
	outfile << strvec[13] <<"," << strvec[14] << "," << strvec[15] << "," << strvec[16] << "," << strvec[19] << ",";
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

	outfile.open("iOSac30addtime.csv");


	strvec.resize(csize);

	outfile << "DailyID,Month,Day,DayofWeek,SecondH,Seconddif,Latitude,Longtitude,x,y,State,Accuracy,Speed,Course,CityCode,Distance,Speed2" << "\n";

	cout << "import" << "\n";
	error = fopen_s(&fp, "iOSac30.csv", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL) {

		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}

		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		//cout << "sp" << endl;

		if (strvec[1] == "Year") {
			cout << strvec[20];
			dailyid = strvec[0];
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