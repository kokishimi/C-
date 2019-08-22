#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;

#define csize 3
#define dsize 1000

vector<vector<string>> strvec;
vector<string> bufvec;

int rnum = 0;
ofstream outfile;

FILE *fp;
char buf[600] = { 0 };
errno_t error;
char *ctx;

double t;
double tb;
double ts;

double v;
double vb;
double vs;

double c1, c2;

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("tvalue.csv");

	bufvec.resize(csize);
	strvec.resize(dsize);

	cout << "import" << "\n";
	error = fopen_s(&fp, "forttest2.csv", "r");
	cout << "success\n";
	while (fgets(buf, sizeof(buf), fp) != NULL) {

		cout << "a";
		bufvec[0] = strtok_s(buf, ",", &ctx);
		cout << "b";
		for (int rn = 1; rn < csize; rn++) {
			bufvec[rn] = strtok_s(NULL, ",", &ctx);
			cout << rn << "\n";
		}
		bufvec[csize - 1].pop_back();
		strvec[rnum] = bufvec;
		bufvec.clear();
		bufvec.resize(csize);

		if (rnum % 1000000 == 0) {

			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}
		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		if (strvec[0][0] != "Count_") {
			++rnum;
		}
		cout << rnum<<"\n";
	}

	for (int i = 0; i < rnum; ++i) {
		for (int j = 0; j < i+1; ++j) {
			c1 = pow(stod(strvec[i][2]), 2)*stod(strvec[i][0]) / (stod(strvec[i][0]) - 1);
			c2 = pow(stod(strvec[j][2]), 2)*stod(strvec[j][0]) / (stod(strvec[j][0]) - 1);
			ts = stod(strvec[i][1]) - stod(strvec[j][1]);
			tb = sqrt(c1 / stod(strvec[i][0]) + c2 / stod(strvec[j][0]));
			t = ts / tb;
			outfile << t;
			if (j == i) {
				outfile << "\n";
			}
			else {
				outfile << ",";
			}
		}
	}
	outfile.close();
	outfile.open("DoF.csv");

	for (int i = 0; i < rnum; ++i) {
		for (int j = 0; j < i + 1; ++j) {
			c1 = pow(stod(strvec[i][2]), 2)*stod(strvec[i][0]) / (stod(strvec[i][0]) - 1.0);
			c2 = pow(stod(strvec[j][2]), 2)*stod(strvec[j][0]) / (stod(strvec[j][0]) - 1.0);
			tb= c1 / stod(strvec[i][0]) + c2 / stod(strvec[j][0]);
			vs = tb * tb;
			ts = pow(c1, 2) / (stod(strvec[i][0])*stod(strvec[i][0])*(stod(strvec[i][0]) - 1.0));
			t = pow(c2, 2) / (stod(strvec[j][0])*stod(strvec[j][0])*(stod(strvec[j][0]) - 1.0));
			vb = ts + t;
			v = vs / vb;
			outfile << v;
			if (j == i) {
				outfile << "\n";
			}
			else {
				outfile << ",";
			}
		}
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