#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>
#include "latlong.h"


using namespace std;
#define dsize 500
#define dsize2 400000
#define csize 21
vector<string> record;
vector<string> strvec;
//string s;

//vector<vector<string>> outp;
//vector<string> row;
string dailyid="b";
string OS="a";
int point,pointac;
double Sx, Sy, Ex, Ey;
string St, Et;
double dis,dis2, meansp, maxsp, meanac, sumac, worstac;
double Sx2, Sy2, Ex2, Ey2;
int dis2f = 0;
string meansp2, maxsp2;
string loop;
double Sac, Eac;
//int i = 0;
int rnum = 0;
int elnum = 0;
int flag = 0;

ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;

int i2 = 0;

int flagimp = 0;
int CID, CSec, CLat, CLon, COS, CAc, CSp, CCo, CDoW;

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
			COS = i;
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



//int split(string&input, char delimiter) {//line‚Ì•ªŠ„
	//istringstream stream(input);
	//string field;
	//while (getline(stream, field, delimiter)) {
		//record.push_back(field);
	//}
	//return 0;
//}

//int row1(void) {//1s–Ú
	//row.push_back("DailyID");
	//row.push_back("OS");
	//row.push_back("pointsize");
	//row.push_back("Sx");
	//row.push_back("Sy");
	//row.push_back("Ex");
	//row.push_back("Ey");
	//row.push_back("St");
	//row.push_back("Et");
	//row.push_back("Distance");
	//row.push_back("maxspeed");
	//row.push_back("meanaccuracy");
	//row.push_back("worstaccuracy");
	//row.push_back("meanspeed");
	//row.push_back("loop");
	//outp.push_back(row);
	//row.clear();
	//cout << "row1" << endl;
	//return 0;
//}



int rown(void){//ŒJ‚è•Ô‚µŒvŽZ
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
	//if (rnum > 1210000) {
		//cout << "point" << endl;
	//}
	//cout << "a";
	if (strvec[CSp] != ""&& strvec[CSp] != " ") {
		if (maxsp < stod(strvec[CSp])) {
			maxsp = stod(strvec[CSp]);
		}
	}
	//if (rnum > 1210000) {
		//cout << "maxsp" << endl;
	//}
	//cout << strvec[i][14] << endl;

	if (strvec[CAc] != ""&& strvec[CAc] != " ") {
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
	
	//if (rnum > 1210000) {
		//cout << "ac" << endl;
	//}
	//cout << "ac" << endl;
	return 0;
}

int ini(void) {//‰Šú‰»
	dailyid = strvec[CID];
	OS = "unknown";
	point = 0;
	pointac = 0;
	xy(stod(strvec[CLon]), stod(strvec[CLat]));
	Sx = x;
	Sy = y;
	St = strvec[CSec];
	if (strvec[CSp] == "" || strvec[CSp] == " ") {
		maxsp = 0.0;
	}
	else {
		maxsp = stod(strvec[CSp]);
	}
	sumac =0;

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
	//cout << "f";
	rown();
	return 0;
}

int outpn(void) {//outp‚Ö‚Ì‘}“ü
	
	xy(stod(record[CLon]), stod(record[CLat]));
	Ex = x;
	Ey = y;
	Et = record[CSec];
	dis = sqrt(pow(Sx - Ex, 2) + pow(Sy - Ey, 2));
	meanac = sumac / pointac;
	if (stod(Et) == stod(St)) {
		meansp2 = "1point";
	}
	else {
		meansp2 = "2ormore";
	}
	meansp = dis2 / (stod(Et) - stod(St));

	if (record[CAc] == "" || record[CAc] == " ") {
		Eac = 2000.0;
	}
	else {
		Eac = stod(record[CAc]);
	}
	
	if (dis < (Eac + Sac)*2.0) {
		loop = "loop";
	}
	else {
		loop = "not";
	}


	outfile << dailyid << ",";
	outfile << OS << ",";
	outfile << point << ",";
	outfile << Sx << "," << Sy << "," << Ex << "," << Ey << ",";
	outfile << St << "," << Et << ",";
	outfile << dis2 << "," << maxsp << "," << meanac << "," << worstac << ",";
	if (meansp2 == "1point") {
		outfile << meansp2 << ",";
	}
	else {
		outfile << meansp << ",";
	}
	outfile << loop << "\n";
	if (flag == 0) {
		ini();
	}
	

	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);
	//ifstream infile;
	//infile.open("iOSac30.csv");
	
	outfile.open("m01ID.csv");
	
	record.resize(csize);
	strvec.resize(csize);
	//outp.reserve(dsize2);
	//row.reserve(15);

	outfile << "DailyID,OS,pointsize,Sx,Sy,Ex,Ey,St,Et,Distance,maxspeed,meanaccuracy,worstaccuracy,meanspeed,loop" << "\n";
	//row1();

	cout << "import" << "\n";
	error = fopen_s(&fp,"m01iOSac30ika.csv", "r");

	//while (getline(infile, s)) {
		//split(s,",");
	while (fgets(buf, sizeof(buf), fp) != NULL) {

		if (i2 != 0) {
			for (int i3 = 0; i3 < csize; ++i3) {
				record[i3] = strvec[i3];
			}
		}
		strvec[0]=strtok_s(buf, ",",&ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn]=strtok_s(NULL, ",",&ctx);
		}

		//csvr();
		//if (rnum > 1210000) {
			//cout << rnum << endl;
		//}
		//cout << "sp" << endl;

		if (flagimp==0) {
			cout << strvec[20];
			imp();
			flagimp = 1;
		}
		else if (OS=="a") {
			ini();
			i2 = 1;
			//cout << "a" << endl;

		}
		else if (strvec[0] == dailyid) {

			//if (rnum > 1210000) {
				//cout << "same" << endl;
			//}

			rown();
			//cout << "b" << endl;
			
		}
		else {

			//if (rnum > 1210000) {
				//cout << "dif" << endl;
			//}

			outpn();

			//++elnum;
			//if (elnum % 100 == 0) {
				//cout << elnum << endl ;
			//}
			//cout << i <<"c"<< endl;
			
		}
		if (rnum % 500000 == 0) {

			t2 = time(NULL);
			cout << rnum <<"\n"<<t2-t1<< "\n";
		}
		//if (rnum > 1210000) {
			//cout << rnum << endl;
		//}
		++rnum;
	}
	flag = 1;
	for (int i3 = 0; i3 < csize; ++i3) {
		record[i3] = strvec[i3];
	}
	outpn();



	//for (unsigned int j = 0; j < outp.size(); ++j) {
		//for (unsigned int i = 0; i < outp[j].size(); ++i) {
			//if (i < outp[j].size() - 1) {
				//outfile << outp[j].at(i) << ",";
			//}
			//else {
				//outfile << outp[j].at(i);
			//}
			
		//}
		//outfile << endl;
		//if (j % 100000 == 0) {
			//t2 = time(NULL);
			//cout <<j<<endl<< t2-t1 << endl;
		//}
	//}
	fclose(fp);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "\n";
	return 0;
}