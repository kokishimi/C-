#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;
constexpr auto dsize = 120000;//record数
constexpr auto csize = 15;//column数
constexpr auto distlim = 100;//同一地点距離閾値

//時間
time_t t1, t2;//開始時刻、現在時刻

//表の記憶
vector<vector<string>> strAnd;//全体の記憶Android
vector<vector<string>> striOS;//全体の記憶iOS
vector<string> strvec;//1行分のバッファ

string month;//dailyIDの月
string date;//dailyIDの日

//iniで初期化
double Sx, Sy, Ex, Ey;//比較元座標
double maxdis;//同一地点と判断した点群の最大値
int datenum;//同一人物の登場日数
string dlyID[366];//同一地点のDailyID
int dlyrw[366];//同一地点の行ナンバー

//比較先用フラグ
int ntsmflag;//比較先が同日ではない：0

//status
int rnum = 0;//読み込み行数
int Anum = 0;//Android人数
int inum = 0;//iOS人数

//入出力
ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;

//1行目column用
int flagimp = 0;//1行目column用
int CID, COS, CSx, CSy, CEx, CEy, CLp;

int imp(void) {//columnの記憶
	//DailyID,OS,pointsize,Sx,Sy,Ex,Ey,St,Et,Distance,maxspeed,meanaccuracy,worstaccuracy,meanspeed,loop
	for (int i = 0; i < csize; ++i) {
		if (strvec[i] == "Dailyid") {
			CID = i;
			cout << "CID,";
		}
		else if (strvec[i] == "OS") {
			COS = i;
			cout << "CID,";
		}
		else if (strvec[i] == "Sx") {
			CSx = i;
			cout << "CSx,";
		}
		else if (strvec[i] == "Sy") {
			CSy = i;
			cout << "CSy,";
		}
		else if (strvec[i] == "Ex") {
			CEx = i;
			cout << "CEx,";
		}
		else if (strvec[i] == "Ey") {
			CEy = i;
			cout << "CEy,";
		}
		else if (strvec[i] == "loop") {
			CLp = i;
			cout << "CLp,";
		}
		cout << i;
	}
	return 0;
}


int smpt(int dstn, vector<vector<string>> &data3) {//同一地点判定と追記
	double dist[4];//0:ss,1:se,2:es,3:ee
	double maxdsbf = 0.0;
	int spflag = 0;//同一地点フラグ 0:同一
	dist[0] = sqrt(pow(Sx - stod(data3[dstn][CSx]), 2)+pow(Sy-stod(data3[dstn][CSy]),2));
	dist[1] = sqrt(pow(Sx - stod(data3[dstn][CEx]), 2) + pow(Sy - stod(data3[dstn][CEy]), 2));
	dist[2] = sqrt(pow(Ex - stod(data3[dstn][CSx]), 2) + pow(Ey - stod(data3[dstn][CSy]), 2));
	dist[3] = sqrt(pow(Ex - stod(data3[dstn][CEx]), 2) + pow(Ey - stod(data3[dstn][CEy]), 2));
	for (int i = 0; i < 4; ++i) {
		if (dist[i] < distlim) {//すべてdistlim未満なら同一地点
			if (dist[i] > maxdsbf) {
				maxdsbf = dist[i];
			}
		}
		else {
			spflag = 1;
			break;
		}
	}
	if (spflag == 0) {
		if (maxdis < maxdsbf) {
			maxdis = maxdsbf;
		}
		dlyID[datenum] = data3[dstn][CID];
		dlyrw[datenum] = dstn;
		++datenum;
	}
	return spflag;
}

int ini(int rwnm, vector<vector<string>> &data2) {//初期化
	Sx = stod(data2[rwnm][CSx]);
	Sy = stod(data2[rwnm][CSy]);
	Ex = stod(data2[rwnm][CEx]);
	Ey = stod(data2[rwnm][CEy]);
	maxdis = sqrt(pow(Sx - Ex, 2) + pow(Sy - Ey, 2));
	datenum = 1;
	dlyID[0] = data2[rwnm][CID];
	dlyrw[0] = rwnm;
	return 0;
}

int outp(void) {//書き込み
	//Sx,Sy,maxdis,datenum,DailyID
	outfile << Sx << "," << Sy << "," << maxdis << "," << datenum << ",";
	for (int i = 0; i < datenum-1; ++i) {
		outfile << dlyID[i] << ",";
	}
	outfile << dlyID[datenum - 1] << "\n";
	return 0;
}

int smps(vector<vector<string>> &data,int dnum) {//同一人物判定
	int rwbf;//同一地点の行番号
	int alrflag[dsize];//比較元がすでに使われている：0
	for (int i = 0; i < dnum; ++i) {
		alrflag[i] = 1;
	}

	for (int i = 0; i < dnum - 1; ++i) {//比較元
		ini(i, data);
		if (alrflag[i] == 1) {
			for (int j = i + 1; j < dnum; ++j) {//比較先
				if (alrflag[j] == 1) {
					ntsmflag = 0;
					for (int k = 0; k < datenum; ++k) {
						rwbf = dlyrw[k];
						if (stoi(data[j][csize]) == stoi(data[rwbf][csize])) {
							if (stoi(data[j][csize + 1]) == stoi(data[rwbf][csize + 1])) {
								ntsmflag = 1;//同日確定
								break;
							}
						}
					}
					if (ntsmflag == 0) {
						alrflag[j] = smpt(j, data);//同一地点判定と書き込み
					}
				}
			}
			outp();
		}
		
		if (i % 100 == 0) {
			t2 = time(NULL);
			cout << i << "\n" << t2 - t1 << "\n";
		}
	}
	return 0;
}

int main(void) {
	
	t1 = time(NULL);

	strAnd.reserve(dsize);
	striOS.reserve(dsize);

	CLp = 0;
	COS = 0;

	cout << "import" << "\n";
	error = fopen_s(&fp, "m01ID.csv", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL) {//読み込み

		//strvecに行を代入
		strvec.resize(csize);
		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();
		if (flagimp == 1) {//月・日の挿入
			month =strvec[CID].substr(2, 2);
			strvec.emplace_back(month);

			date = strvec[CID].substr(4, 2);
			strvec.emplace_back(date);
		}
		else {
			strvec.emplace_back("month");
			strvec.emplace_back("date");
			for (int i = 0; i < csize + 2; ++i) {
				cout << strvec[i] << ",";
			}
		}

		if (flagimp == 0) {//最初の行
			cout << strvec[14];
			imp();
			flagimp = 1;
			cout << "firstrowEnd\n";
			cout << CLp << "=CLp\n";
		}
		else if (strvec[CLp] == "loop") {
			if (strvec[COS] == "Android") {//アンドロイド配列の作成
				strAnd.emplace_back(strvec);
				++Anum;
			}
			else if (strvec[COS] == "iOS") {//iOS配列の作成
				striOS.emplace_back(strvec);
				++inum;
			}
		}

		if (rnum % 50000 == 0) {
			t2 = time(NULL);
			cout << rnum << "\n" << t2 - t1 << "\n";
		}

		strvec.clear();
		++rnum;
	}
	fclose(fp);
	cout << t2 - t1 << "imporfin\n";

	//Androidファイル
	outfile.open("smprsn01Android.csv");
	outfile << "Sx,Sy,maxdis,datenum,DailyID\n";
	//同一人物の推定
	cout << Anum << "Anum\n";
	smps(strAnd, Anum);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "Androidfin\n";

	//iOSファイル
	outfile.open("smprsn01iOS.csv");
	outfile << "Sx,Sy,maxdis,datenum,DailyID\n";
	//同一人物の推定
	cout << inum << "inum\n";
	smps(striOS, inum);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "iOSfin\n";
	return 0;
}