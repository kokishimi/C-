#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>

using namespace std;

constexpr auto dsize = 10000;//一人のpoint数
constexpr auto csize = 15;//属性数
constexpr auto window = 300;//窓の大きさ[s]
constexpr auto radius = 250;//直径の大きさ[m]
constexpr auto interval = 1800;//滞在地と考える間隔

vector<vector<string>> person;//一人の軌跡データ
vector<string> strvec;//ポイントデータ

string dailyid;

ofstream outfile;

FILE* fp;
char buf[300] = { 0 };
errno_t error;
char* ctx;

int flagimp = 0;//0:1行目,1:2行目,2:三行目以降,3:終わった後

int CID, CSec, CAc, CSp, CmS, CDis, CSD, CSp2,Cx,Cy;

int IDnum = 0;//ポイント数/人

int modeflag[dsize];//1：暫定滞在
int staynum[dsize];//滞在地点番号（0：移動）
int staynum2;//滞在地点数
int staytime = 0;//滞在時間
int startID;//滞在候補の最初の点

double distmat[dsize][dsize];//距離01行列
int excrflag;//0:rを超えない　1:rを超えた

int secnum[2];//暫定最大区間の始点終点
double maxst;//暫定最大区間時間

int spoint;//間隔があいた後の開始点

double mindis, dis;
double xdis, ydis;

int rnum = 0;

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
		else if (strvec[i] == "x") {
			Cx = i;
		}
		else if (strvec[i] == "y") {
			Cy = i;
		}
		cout << i << strvec[i] << "a\n";
	}
	return 0;
}

int record(void) {
	//一人の人を格納
	person[IDnum] = strvec;
	modeflag[IDnum] = 0;

	IDnum++;

	return 0;
}

int cpt2(int sID2, int lID2) {//暫定最大値の計算
	staytime = stoi(person[lID2][CSec]) - stoi(person[sID2][CSec]);
	if (staytime > maxst) {
		maxst = staytime;
		secnum[0] = sID2;
		secnum[1] = lID2;
	}
	return 0;
}

int cptime(int sID,int lID) {//一番長い滞在時間となる系列を見つける
	maxst = 0;
	for (int k = sID; k < lID - 1; ++k) {
		for (int l = k + 1; l < lID; ++l) {
			if (distmat[k][l] == 1) {
				cpt2(k, l - 1);
				break;
			}
			else if (l == lID - 1) {
				cpt2(k, l);
				break;
			}
		}
	}

	return 0;
}

int staymode(int stID, int lastID) {//1個前までの滞在地候補を確定させる lastID:滞在候補の次の点
	if (staytime < window) {//窓より短かったら移動と判定
		for (int i = stID + 1; i < lastID; ++i) {
			staynum[i] = 0;
		}
		return 0;
	}

	excrflag = 0;
	for (int i = stID; i < lastID-1; ++i) {//直径r内に収まっていたら滞在判定
		for (int j = i + 1; j < lastID; ++j) {
			xdis = stod(person[i][Cx]) - stod(person[j][Cx]);
			ydis = stod(person[i][Cy]) - stod(person[j][Cy]);
			dis = sqrt(pow(xdis, 2) + pow(ydis, 2));
			if (dis > radius) {
				distmat[i][j] = 1;
				excrflag = 1;
			}
		}
	}
	
	if (excrflag == 1) {
		cptime(stID, lastID);//一番長い滞在時間となる系列を見つける

		if (maxst < window) {
			for (int i = stID + 1; i < lastID; ++i) {
				staynum[i] = 0;
			}
		}
		else {
			if (secnum[0] == spoint) {
				--staynum2;
			}
			if(secnum[0]!=stID){
				for (int i = stID; i < secnum[0]; ++i) {
					staynum[i] = 0;
				}
			}
			for (int i = secnum[0]; i < secnum[1] + 1; ++i) {
				staynum[i] = staynum2;
			}
			if (secnum[1] != lastID - 1) {
				for (int i = secnum[1] + 1; i < lastID; ++i) {
					staynum[i] = 0;
				}
			}
			++staynum2;
		}
	}
	else {
		if (stID == spoint) {
			--staynum2;
		}
		for (int i = stID; i < lastID; ++i) {//直径r内に収まっていた
			staynum[i] = staynum2;
		}
		++staynum2;
	}

	return 0;
}

int mode(void) {
	for (int i = 0; i < IDnum-1; ++i) {
		for (int j = i + 1; j < IDnum; ++j) {
			distmat[i][j] = 0;
		}
	}

	for (int IDi = 0; IDi < IDnum; ++IDi) {
		//滞在判定
		if (IDi >0) {//2ポイント目以降

			if (stod(person[IDi][CDis]) < radius) {
				modeflag[IDi] = 1;
				staytime += stoi(person[IDi][CSD]);
			}
			else {
				if (modeflag[IDi - 1] == 1) {
					staymode(startID,IDi);//1個前までの滞在地候補を確定させる
				}
				if (stoi(person[IDi][CSD]) > interval) {
					staynum[IDi] = staynum2;
					spoint = IDi;
					staytime = stoi(person[IDi][CSD]);
					++staynum2;
				}
				else {
					staynum[IDi] = 0;
					staytime = 0;
				}
				
				startID = IDi;
			}
		}
		else {
			staytime = 0;
			startID = IDi;
			staynum[IDi] = 1;
			staynum2 = 2;
			spoint = 0;
		}
	}

	if (modeflag[IDnum - 1] == 1) {
		staymode(startID, IDnum);
	}
	if (staynum[IDnum - 1] == 0) {
		staynum[IDnum - 1] = staynum2;
	}

	return 0;
}

int outpn(void) {

	mode();

	for (int IDj = 0; IDj < IDnum; ++IDj) {
		outfile << person[IDj][CID] << "," << person[IDj][CSec] << "," << person[IDj][CSp] << ",";
		outfile << person[IDj][CSp2] << "," << person[IDj][CDis] << ","<<staynum[IDj]<<"\n";
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

	outfile.open("m10iOSac30ikaMS.csv");


	strvec.resize(csize);
	person.resize(dsize);

	outfile << "DailyID,SecondH,Speed,Speed2,dis,mode" << "\n";

	std::cout << "import\n";
	//DailyID, DayofWeek, SecondH, SecondDif Latitude, Longtitude, x, y, State, Accuracy, Speed, Course, CityCode, Distance, Speed2
	error = fopen_s(&fp, "m10iOSac30ikaadd.csv", "r");
	std::cout << "open\n";
	while (fgets(buf, sizeof(buf), fp) != NULL) {

		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();

		if (flagimp == 0) {//項目の読み込み
			std::cout << strvec[csize - 1] << "first\n";
			if (strvec[0] != "DailyID") {
				strvec[0] = "DailyID";
			}
			flagimp = 1;
			imp();
		}
		else if (flagimp == 1) {//2行目
			cout << "Second\n";
			dailyid = strvec[CID];
			record();
			flagimp = 2;
		}
		else if (strvec[CID] == dailyid) {
			record();
		}
		else {
			outpn();//今までのを計算
		}

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