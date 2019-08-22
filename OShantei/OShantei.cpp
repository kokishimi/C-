#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;
#define dsize 500
#define dsize2 400000
#define csize 21

vector<string> strvec;


string dailyid = "b";
string OS = "a";

int rnum = 0;
int elnum = 0;
ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;

int i2 = 0;

int flagimp = 0;
int COS, CID;

int flag = 0;

int imp(void) {
	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "Logtype_subcategory") {
			COS = i;
		}
		else if (strvec[i] == "Dailyid") {
			CID = i;
		}
	}
	return 0;
}




int rown(void) {//ŒJ‚è•Ô‚µŒvŽZ
	if (OS == "unknown") {
		if (strvec[COS] == "move" || strvec[COS] == "arrival" || strvec[COS] == "departure") {
			OS = "iOS";
		}
		else if (strvec[COS] == "timer") {
			OS = "Android";
		}
	}

	return 0;
}

int ini(void) {//‰Šú‰»
	dailyid = strvec[CID];
	OS = "unknown";

	rown();
	return 0;
}

int outpn(void) {//outp‚Ö‚Ì‘}“ü

	outfile << dailyid << ",";
	outfile << OS << "\n";
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

	outfile.open("m12OS.csv");

	strvec.resize(csize);
	//outp.reserve(dsize2);
	//row.reserve(15);

	outfile << "DailyID,OS\n";
	//row1();

	cout << "import" << "\n";
	error = fopen_s(&fp, "m01origin2.csv", "r");

	//while (getline(infile, s)) {
	//split(s,",");
	while (fgets(buf, sizeof(buf), fp) != NULL) {


		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
			//cout << strvec[rn] << "\n";
		}

		//csvr();
		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		//cout << "sp" << endl;

		if (flagimp == 0) {
			cout << strvec[20] << "imp";
			imp();
			flagimp = 1;
		}
		else if (OS == "a") {
			ini();
			i2 = 1;
			cout << "a" << endl;

		}
		else if (strvec[CID] == dailyid) {

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
			//cout << "c" << endl;

		}
		if (rnum % 1000000 == 0) {

			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}
		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		++rnum;
	}
	flag = 1;
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