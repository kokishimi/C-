#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;

#define csize 2
#define dsize 6000
#define samplenum 100.0
#define upnum 400

vector<vector<string>> strvec;
vector<string> bufvec;

int rnum = 0;
ofstream outfile;

FILE *fp;
char buf[600] = { 0 };
errno_t error;
char *ctx;

int snum;//これ以上の番号は保護
double rate;//どのくらいの間隔に一個サンプリングするか
int sampled;//サンプリングされた番号
int stnum = 0;//前回の最後の番号

double c1, c2;

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	outfile.open("sampling14_500.csv");

	bufvec.resize(csize);
	strvec.resize(dsize);

	cout << "import" << "\n";
	error = fopen_s(&fp, "forsampling.csv", "r");
	cout << "success\n";
	while (fgets(buf, sizeof(buf), fp) != NULL) {

		bufvec[0] = strtok_s(buf, ",", &ctx);
		//cout << "b";
		for (int rn = 1; rn < csize; rn++) {
			bufvec[rn] = strtok_s(NULL, ",", &ctx);
			//cout << rn << "\n";
		}
		bufvec[csize - 1].pop_back();
		strvec[rnum] = bufvec;
		bufvec.clear();
		bufvec.resize(csize);

		if (rnum % 1000 == 0) {

			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}
		//if (rnum > 1210000) {
		//cout << rnum << endl;
		//}
		if (strvec[0][1] != "Density") {
			++rnum;
		}
		//cout << rnum << "\n";
	}
	cout << rnum << "\n";

	snum = rnum - upnum;
	rate = stod(strvec[snum-1][1]) / samplenum;

	for (int i = 0; i < samplenum; ++i) {
		for (int j = stnum; j < snum; ++j) {
			if (rate*(i + 1) <= stod(strvec[j][1])) {
				sampled = (rand() % (j + 1 - stnum)) + stnum;
				outfile << strvec[sampled][0] << ",";
				outfile << strvec[sampled][1] << "\n";
				stnum = j + 1;
				break;
			}
		}
	}
	//num250-1までサンプリング

	for (int i = snum; i < rnum; ++i) {
		outfile << strvec[i][0] << ",";
		outfile << strvec[i][1] << "\n";
	}

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