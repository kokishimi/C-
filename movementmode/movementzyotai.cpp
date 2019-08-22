#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;

#define dsize 10000//point数
#define csize 15
#define window 600

vector<vector<string>> person;
vector<string> strvec;

string dailyid;

ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;

int flagimp = 0;

int CID, CSec, CAc, CSp, CmS, CDis, CSD, CSp2;

int IDnum = 0;
int IDfir = 0;

int modeflag[dsize];
int staytime = 0;
int stayID = 0;

double mindis,dis;

int rnum = 0;

int spmode;

int imp(void) {
	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "DailyID") {
			CID = i;
			cout << i << "ID\n";
		}
		else if (strvec[i] == "SecondH") {
			CSec = i;
			cout << "Secimp\n";
		}
		else if (strvec[i] == "Accuracy") {
			CAc = i;
		}
		else if (strvec[i] == "Speed") {
			CSp = i;
		}
		else if (strvec[i] == "minSpeed") {
			CmS = i;
			cout << i << "minSp\n";
		}
		else if (strvec[i] == "Distance") {
			CDis = i;
		}
		else if (strvec[i] == "SecondDif") {
			CSD = i;
		}
		else if (strvec[i] == "Speed2") {
			CSp2 = i;
		}
		cout << i <<strvec[i]<<"a\n";
	}
	return 0;
}

int record(void) {
	//一人の人を格納
	person[IDnum] = strvec;

	//if (rnum > 480) {
		//cout << "person" << endl;
	//}
	if (spmode == 1) {
		if (stod(strvec[CmS]) > 3.0) {
			//if (rnum > 480) {
			//cout << "CmS" << endl;
			//}
			modeflag[IDnum] = 1;
			//cout << "Cms" << IDnum << "," << strvec[CmS] << "\n";
		}
		else if (strvec[CSp] != " ") {
			if (stod(strvec[CSp]) > 3.0) {
				modeflag[IDnum] = 1;
				//if (rnum > 480) {
				//cout << "CSp" << endl;
				//}
				//cout << "CSp"<<IDnum;
			}
			else {
				modeflag[IDnum] = 0;
				//cout << "CSp" << IDnum;
			}
		}
		else {
			modeflag[IDnum] = 0;
			//cout << "brank";
		}
	}
	else {
		if (stod(strvec[CSp2]) > 3.0) {
			//if (rnum > 480) {
			//cout << "CmS" << endl;
			//}
			modeflag[IDnum] = 1;
			//cout << "Cms" << IDnum << "," << strvec[CmS] << "\n";
		}
		else if (strvec[CSp] != " ") {
			if (stod(strvec[CSp]) > 3.0) {
				modeflag[IDnum] = 1;
				//if (rnum > 480) {
				//cout << "CSp" << endl;
				//}
				//cout << "CSp"<<IDnum;
			}
			else {
				modeflag[IDnum] = 0;
				//cout << "CSp" << IDnum;
			}
		}
		else {
			modeflag[IDnum] = 0;
			//cout << "brank";
		}
	}

	//if (rnum > 480) {
		//cout << "person" << endl;
	//}

	IDnum++;

	return 0;
}

int stay(int sIDi) {
	if (sIDi > 0) {
		mindis = stod(person[sIDi][CDis]) - stod(person[sIDi][CAc]) - stod(person[sIDi - 1][CAc]);

		if (mindis <= 0 && modeflag[sIDi] == 0) {
			modeflag[sIDi] = 3;
			staytime += stoi(person[sIDi][CSD]);

			if (staytime == stoi(person[sIDi][CSD])) {
				stayID = sIDi;
			}

			if (staytime > window * 3 && stayID != sIDi) {
				//範囲を変更する
				for (int IDi4 = sIDi; IDi4 > stayID - 2; --IDi4) {

					for (int IDi5 = IDi4 - 2; IDi5 > stayID - 2; ++IDi5) {
						//dis = x * x + y * y;
						mindis = dis - stod(person[IDi4][CAc]) - stod(person[IDi5][CAc]);
						if (mindis > 0) {
							modeflag[IDi4] = 0;
						}
					}

					modeflag[IDi4] = 4;
					staytime = 0;
				}

			}
			else if (modeflag[sIDi - 2] == 4 && modeflag[sIDi - 1] == 3) {
				modeflag[sIDi - 1] = 4;
				staytime = 0;
			}
		}
		else {
			staytime = 0;
		}

		
	}

	return 0;
}

int mode(void) {
	//0:初期（歩行かも）, 1:乗り物, 2:乗り物かも, 3:滞在かも, 4:滞在かも
	for (int IDi = 0; IDi < IDnum; ++IDi) {
		//乗り物判定
		if (modeflag[IDi] == 0) {

			for (int IDi2 = IDfir; IDi2 < IDi; ++IDi2) {
				//if (IDi2 % 20 == 0) {
					//cout << IDi2 << stoi(person[IDi2][CSp]) << "\n";
				//}
				if (stoi(person[IDi][CSec]) - window < stoi(person[IDi2][CSec])) {
					IDfir = IDi2;
					if (modeflag[IDi2] == 1) {

						modeflag[IDi] = 2;
						break;
					}
				}
			}
			if (modeflag[IDi] == 2) {
				for (int IDi3 = IDi; IDi3 < IDnum; ++IDi3) {
					if (stoi(person[IDi3][CSec]) < stoi(person[IDi][CSec]) + window) {
						if (modeflag[IDi3] == 1) {
							modeflag[IDi] = 1;
							break;
						}
					}
				}
				if (modeflag[IDi] == 2) {
					modeflag[IDi] = 0;
				}
			}
		}
		
		//滞在判定//window分同地点にいるなど
		//stay(IDi);
		

	}

	return 0;
}

int outpn(void) {//outpへの挿入
	
	mode();

	for (int IDj = 0; IDj < IDnum; ++IDj) {
		outfile << person[IDj][CID] << "," << person[IDj][CSec] << "," << person[IDj][CSp] << "," << person[IDj][CSp2] << ",";
		if (modeflag[IDj] == 1) {
			outfile << "Vehicle\n";
		}
		else {
			outfile << "Pedestrian\n";
		}
	}

	person.clear();
	person.resize(dsize);
	IDnum = 0;
	if (flagimp == 2) {
		record();
		dailyid = strvec[CID];
	}
	
	return 0;
}

int main(void) {
	time_t t1, t2;
	t1 = time(NULL);

	cout << "imput spmode::speed2:0, minspeed:1\n";
	cin >> spmode;

	outfile.open("m09iOSac30ikaWin600sp2.csv");


	strvec.resize(csize);
	person.resize(dsize);

	outfile << "DailyID,SecondH,Speed,Speed2,mode" << "\n";

	std::cout << "import" << "\n";
	//DailyID, DayofWeek, SecondH, SecondDif Latitude, Longtitude, x, y, State, Accuracy, Speed, Course, CityCode, Distance, Speed2
	error = fopen_s(&fp, "m09iOSac30ikaadd.csv", "r");
	std::cout << "open\n";
	while (fgets(buf, sizeof(buf), fp) != NULL) {

		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();
		
		//cout << "strvec\n";

		//if (rnum > 480) {
			//cout << "strvec" << endl;
		//}
		//cout << "sp" << endl;

		if (flagimp == 0) {//項目の読み込み
			std::cout << strvec[csize-1]<<"first\n";
			if (strvec[0] != "DailyID") {
				strvec[0] = "DailyID";
			}
			flagimp = 1;
			imp();
		}
		else if (flagimp == 1) {
			cout << "Second\n";
			dailyid = strvec[CID];
			record();
			flagimp = 2;
		}
		else if (strvec[CID] == dailyid) {

			//if (rnum > 65917) {
				//std::cout << "same" << "\n";
			//}
			
			record();
			//cout << "b" <<rnum<<"\n";


		}
		else {
			

			//if (rnum > 1210000) {
			//cout << "dif" << endl;
			//}
			outpn();//今までのを計算

			//++elnum;
			//if (elnum % 100 == 0) {
			//cout << elnum << endl ;
			//}
			//cout <<"c"<< rnum<<"\n";

		}
		//if (rnum > 480){
			//std::cout << rnum << "\n";
		//}
		//std::cout << rnum << "\n";
		if (rnum % 100000 == 0) {

			t2 = time(NULL);
			std::cout << rnum << "\n" << t2 - t1 << "\n";
		}
		
		++rnum;
	}
	flagimp = 3;
	outpn();

	std::fclose(fp);
	outfile.close();
	t2 = time(NULL);
	std::cout << t2 - t1 << "\n";
	return 0;
}