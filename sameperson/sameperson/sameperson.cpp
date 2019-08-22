#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>


using namespace std;
constexpr auto dsize = 120000;//record��
constexpr auto csize = 15;//column��
constexpr auto distlim = 100;//����n�_����臒l

//����
time_t t1, t2;//�J�n�����A���ݎ���

//�\�̋L��
vector<vector<string>> strAnd;//�S�̂̋L��Android
vector<vector<string>> striOS;//�S�̂̋L��iOS
vector<string> strvec;//1�s���̃o�b�t�@

string month;//dailyID�̌�
string date;//dailyID�̓�

//ini�ŏ�����
double Sx, Sy, Ex, Ey;//��r�����W
double maxdis;//����n�_�Ɣ��f�����_�Q�̍ő�l
int datenum;//����l���̓o�����
string dlyID[366];//����n�_��DailyID
int dlyrw[366];//����n�_�̍s�i���o�[

//��r��p�t���O
int ntsmflag;//��r�悪�����ł͂Ȃ��F0

//status
int rnum = 0;//�ǂݍ��ݍs��
int Anum = 0;//Android�l��
int inum = 0;//iOS�l��

//���o��
ofstream outfile;

FILE *fp;
char buf[300] = { 0 };
errno_t error;
char *ctx;

//1�s��column�p
int flagimp = 0;//1�s��column�p
int CID, COS, CSx, CSy, CEx, CEy, CLp;

int imp(void) {//column�̋L��
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


int smpt(int dstn, vector<vector<string>> &data3) {//����n�_����ƒǋL
	double dist[4];//0:ss,1:se,2:es,3:ee
	double maxdsbf = 0.0;
	int spflag = 0;//����n�_�t���O 0:����
	dist[0] = sqrt(pow(Sx - stod(data3[dstn][CSx]), 2)+pow(Sy-stod(data3[dstn][CSy]),2));
	dist[1] = sqrt(pow(Sx - stod(data3[dstn][CEx]), 2) + pow(Sy - stod(data3[dstn][CEy]), 2));
	dist[2] = sqrt(pow(Ex - stod(data3[dstn][CSx]), 2) + pow(Ey - stod(data3[dstn][CSy]), 2));
	dist[3] = sqrt(pow(Ex - stod(data3[dstn][CEx]), 2) + pow(Ey - stod(data3[dstn][CEy]), 2));
	for (int i = 0; i < 4; ++i) {
		if (dist[i] < distlim) {//���ׂ�distlim�����Ȃ瓯��n�_
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

int ini(int rwnm, vector<vector<string>> &data2) {//������
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

int outp(void) {//��������
	//Sx,Sy,maxdis,datenum,DailyID
	outfile << Sx << "," << Sy << "," << maxdis << "," << datenum << ",";
	for (int i = 0; i < datenum-1; ++i) {
		outfile << dlyID[i] << ",";
	}
	outfile << dlyID[datenum - 1] << "\n";
	return 0;
}

int smps(vector<vector<string>> &data,int dnum) {//����l������
	int rwbf;//����n�_�̍s�ԍ�
	int alrflag[dsize];//��r�������łɎg���Ă���F0
	for (int i = 0; i < dnum; ++i) {
		alrflag[i] = 1;
	}

	for (int i = 0; i < dnum - 1; ++i) {//��r��
		ini(i, data);
		if (alrflag[i] == 1) {
			for (int j = i + 1; j < dnum; ++j) {//��r��
				if (alrflag[j] == 1) {
					ntsmflag = 0;
					for (int k = 0; k < datenum; ++k) {
						rwbf = dlyrw[k];
						if (stoi(data[j][csize]) == stoi(data[rwbf][csize])) {
							if (stoi(data[j][csize + 1]) == stoi(data[rwbf][csize + 1])) {
								ntsmflag = 1;//�����m��
								break;
							}
						}
					}
					if (ntsmflag == 0) {
						alrflag[j] = smpt(j, data);//����n�_����Ə�������
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

	while (fgets(buf, sizeof(buf), fp) != NULL) {//�ǂݍ���

		//strvec�ɍs����
		strvec.resize(csize);
		strvec[0] = strtok_s(buf, ",", &ctx);
		for (int rn = 1; rn < csize; rn++) {
			strvec[rn] = strtok_s(NULL, ",", &ctx);
		}
		strvec[csize - 1].pop_back();
		if (flagimp == 1) {//���E���̑}��
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

		if (flagimp == 0) {//�ŏ��̍s
			cout << strvec[14];
			imp();
			flagimp = 1;
			cout << "firstrowEnd\n";
			cout << CLp << "=CLp\n";
		}
		else if (strvec[CLp] == "loop") {
			if (strvec[COS] == "Android") {//�A���h���C�h�z��̍쐬
				strAnd.emplace_back(strvec);
				++Anum;
			}
			else if (strvec[COS] == "iOS") {//iOS�z��̍쐬
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

	//Android�t�@�C��
	outfile.open("smprsn01Android.csv");
	outfile << "Sx,Sy,maxdis,datenum,DailyID\n";
	//����l���̐���
	cout << Anum << "Anum\n";
	smps(strAnd, Anum);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "Androidfin\n";

	//iOS�t�@�C��
	outfile.open("smprsn01iOS.csv");
	outfile << "Sx,Sy,maxdis,datenum,DailyID\n";
	//����l���̐���
	cout << inum << "inum\n";
	smps(striOS, inum);
	outfile.close();
	t2 = time(NULL);
	cout << t2 - t1 << "iOSfin\n";
	return 0;
}