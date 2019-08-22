#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;

#define csize 48

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
int CID, CLU, CSA;

int flag = 0;

int LUflag[16];//0はダミー
int LUS[16];//0はダミー、面積

int imp(void) {//1行目

	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "fid_1") {
			CID = i;
		}
		else if (strvec[i] == "id") {
			CLU = i;
		}
		else if (strvec[i] == "Shape_Area") {
			CSA = i;
		}
	}
	for (int i = 0; i < csize; ++i) {
		outfile << strvec[i] << ",";
	}
	outfile << "LU1,LU2,LU3,LU4,LU5,LU6,LU7,LU8,LU9,LU10,LU11,LU12,LU13,LU14,LU15,";
	outfile << "LUS1,LUS2,LUS3,LUS4,LUS5,LUS6,LUS7,LUS8,LUS9,LUS10,LUS11,LUS12,LUS13,LUS14,LUS15\n";
	return 0;
}




int rown(void) {//繰り返し計算

	strvec[CLU].erase(strvec[CLU].begin(), strvec[CLU].begin() + 3);//sf_の削除

	for (int i = 1; i < 16; ++i) {
		if (stoi(strvec[CLU]) == i) {
			LUflag[i] = 1;
			LUS[i] += stod(strvec[CSA]);
		}
	}
	return 0;
}

int ini(void) {//初期化

	fid = strvec[CID];
	for (int i = 1; i < 16; ++i) {
		LUflag[i] = 0;
		LUS[i] = 0;
	}
	for (int i = 0; i < csize; ++i) {
		outfile << strvec[i] << ",";
	}

	rown();
	return 0;
}

int outpn(void) {//outpへの挿入

	
	for (int i = 1; i < 16; ++i) {
		outfile << LUflag[i] << ",";
	}
	for (int i = 1; i < 15; ++i) {
		outfile << LUS[i] << ",";
	}
	outfile << LUS[15] << "\n";

	if (flagimp == 2) {
		ini();
	}
	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("addLanduse.csv");

	strvec.resize(csize);

	cout << "import" << "\n";
	error = fopen_s(&fp, "forlanduseS.csv", "r");
	cout << "success\n";
	while (fgets(buf, sizeof(buf), fp) != NULL) {


		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();
		
		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		//cout << "strvec" << endl;

		if (flagimp == 0) {//1行目
			cout << strvec[47] << "luname\n";
			imp();
			flagimp = 1;
			cout << "row1end\n";
		}
		else if (flagimp==1) {//2行目
			ini();
			flagimp = 2;
			cout << "row2end" << endl;

		}
		else if (strvec[CID] == fid) {

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
	flagimp = 3;
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