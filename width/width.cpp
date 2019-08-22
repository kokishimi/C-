#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;

#define csize 63

vector<string> strvec;

string fid = "b";

int rnum = 0;
int elnum = 0;
ofstream outfile;

FILE *fp;
char buf[600] = { 0 };
errno_t error;
char *ctx;


int flagimp = 0;
int CW, CW3, CW5, CW13, CW19;

double MWidth;

int imp(void) {//1çsñ⁄

	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "width") {
			CW = i;
		}
		else if (strvec[i] == "Width_3") {
			CW3 = i;
		}
		else if (strvec[i] == "Width3_55") {
			CW5 = i;
		}
		else if (strvec[i] == "Width55_13") {
			CW13 = i;
		}
		else if (strvec[i] == "Width195_") {
			CW19 = i;
		}

	}
	for (int i = 0; i < csize; ++i) {
		outfile << strvec[i] << ",";
	}
	outfile << "MeanWidth\n";
	return 0;
}




int rown(void) {//åJÇËï‘ÇµåvéZ

	if (stod(strvec[CW]) == 0) {
		if (stoi(strvec[CW3]) ==1) {
			MWidth = 1.5;
		}
		else if (stoi(strvec[CW5]) == 1) {
			MWidth = 4.25;
		}
		else if (stoi(strvec[CW13]) == 1) {
			MWidth = 9.25;
		}
		else if (stoi(strvec[CW19]) == 1) {
			MWidth = 19.5;
			cout << "19.5m\n";
		}
		else {
			cout << "error"<<strvec[CW];
			flagimp = 2;
		}
	}
	else {
		MWidth = stod(strvec[CW]);
		cout << MWidth << "\n";
	}

	return 0;
}

int outpn(void) {//outpÇ÷ÇÃë}ì¸

	rown();

	for (int i = 0; i < csize; ++i) {
		outfile << strvec[i] << ",";
	}
	outfile << MWidth << "\n";

	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("addMeanWidthLGD.csv");

	strvec.resize(csize);

	cout << "import" << "\n";
	error = fopen_s(&fp, "020LSChoice2.csv", "r");
	cout << "success\n";
	while (fgets(buf, sizeof(buf), fp) != NULL) {

		cout << "a";
		strvec[0] = strtok_s(buf, ",", &ctx);
		cout << "b";
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
			cout << rn << "\n";
		}
		strvec[csize - 1].pop_back();

		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		//cout << "strvec" << endl;

		if (flagimp == 0) {//1çsñ⁄
			cout << strvec[1] << "width\n";
			imp();
			flagimp = 1;
			cout << "row1end\n";
		}
		else if (flagimp == 2) {
			break;
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