/////////////////////////////////////////////
//検索距離タイプと分析距離タイプを分ける/////
//integration系は検索半径内あるか確認が必要//
//choiceの計算方法を考える///////////////////
/////////////////////////////////////////////



#define _USE_MATH_DEFINES
#include "shapefil.h"
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>


#define smalln 10
#define vernum 200
#define entnum 100000
//#define testout 100000
#define nodenum 1000000

#define shapenum 410000
#define waynum 10000
#define listnum 10000
#define routenum 100 
#define radiusn 1000000


#define minx -11634
#define maxx -8850
#define miny -21064
#define maxy -16995

using namespace std;

ofstream outfile;//出力CSV用

//Shapefile
SHPHandle hSHP;
int nShapeType, nEntities;
double adfMinBound[4], adfMaxBound[4];

SHPObject *psElem;
//double padfX[entnum][vernum], padfY[entnum][vernum], padfZ[entnum][vernum];

string szDBF;
DBFHandle hDBF;

SHPHandle hSHPou;
//DBFHandle hDBFou;

SHPObject *psOObj;



//edge関数用
struct edge {//エッジの情報
	double x[2], y[2];
	double dis;

	int ConnectEdge[2][smalln];
	int ConnectPoint[2][smalln];
	int connum[2] = { 0,0 };
	double ADis[2][smalln], EDis[2][smalln];

	int opflag = 0;
};

edge edgeElem[shapenum];
int ednum = 0;

int ver0num[smalln];
int num0 = 0;

//出力判定用
double medianx, mediany;

//node関数用
/*
struct node {
	double nodex, nodey;
	int ConnectEdge[smalln];
	int connum=1;
};

node nodeElem[nodenum];
int nonum = 2;
int nodeflag[2] = { 0,0 };
int nodeiniflag = 0;
*/


//dijkstra用
struct stdijk {
	double precost=0;
	int waylist[waynum];
	int walinum = 2;//waylistnum:通ったセグメント数
	int point;
};

vector<vector<int>> dijkflag;//dijkstraを終えた組かどうか
vector<int> sdijkflag;

stdijk dijklist[listnum];//次に探索するセグメント番号リスト
double cost;//最終的な距離コスト
int dilinum = 0;//dijklistに載っているセグメントの数

double maxcost;//最大コスト
double radius;//検索半径
string Anadistype;//分析距離タイプ
string Raddistype;//検索半径距離タイプ

//最小コスト探索用
double mincost;
int minedge;
int minedgenum;

//すでに通過した地点の判別用
int passededge[nodenum];
int passededgeflag = 0;

int replaceflag;

int fwhile;//while文に入るかどうかのフラグ

int accessflag = 0;//1回でもリストに終点が乗ったかどうか
int accessflag2 = 0;

int samewayflag;//同じセグメントを使っているか1
int samewaynum;//同コストの経路が何本あるか


//SpaceSyntax用
double choice[shapenum];
double integration;//後で計算
int NodeCount[shapenum];
double MeanDepth;//後で計算
double TotalDepth[shapenum];
double TotalSegmentLength[shapenum];
//connectivity
//A_connectivity
//StepDepth
//SLWの計算方法


//距離の計算Angular,Euclidean
double AngDis(int Edge1, int Edge2,int point1,int point2) {
	double AnguDis;
	double cos1;
	double inpro;
	double vec1[2], vec2[2];
	if (point1 == 0) {
		vec1[0] = edgeElem[Edge1].x[1] - edgeElem[Edge1].x[0];
		vec1[1] = edgeElem[Edge1].y[1] - edgeElem[Edge1].y[0];
	}
	else {
		vec1[0] = edgeElem[Edge1].x[0] - edgeElem[Edge1].x[1];
		vec1[1] = edgeElem[Edge1].y[0] - edgeElem[Edge1].y[1];
	}
	if (point2 == 0) {
		vec2[0] = edgeElem[Edge2].x[1] - edgeElem[Edge2].x[0];
		vec2[1] = edgeElem[Edge2].y[1] - edgeElem[Edge2].y[0];
	}
	else {
		vec2[0] = edgeElem[Edge2].x[0] - edgeElem[Edge2].x[1];
		vec2[1] = edgeElem[Edge2].y[0] - edgeElem[Edge2].y[1];
	}
	
	cos1 = (vec1[0]*vec2[0]+vec1[1]*vec2[1]) / (edgeElem[Edge1].dis * edgeElem[Edge2].dis);
	AnguDis = 2 * acos(cos1) / M_PI;

	return AnguDis;
}

double EucDis(int Edge1, int Edge2) {
	double EuclDis;
	EuclDis = (edgeElem[Edge1].dis + edgeElem[Edge2].dis) / 2;

	return EuclDis;
}


//Edgeの読み込みと出力エッジの選択
int EdgeRead(SHPHandle ifname, SHPHandle ofname) {

	SHPGetInfo(ifname, &nEntities, &nShapeType, adfMinBound, adfMaxBound);

	for (int i = 0; i < nEntities; ++i) {

		psElem = SHPReadObject(ifname, i);
		if (psElem->nVertices >= 2) {
			

			for (int j = 0; j < psElem->nVertices-1; ++j) {

				
				edgeElem[ednum].x[0] = psElem->padfX[j];
				edgeElem[ednum].y[0] = psElem->padfY[j];
				edgeElem[ednum].x[1] = psElem->padfX[j+1];
				edgeElem[ednum].y[1] = psElem->padfY[j+1];
				edgeElem[ednum].dis = sqrt(pow(edgeElem[ednum].x[0]- edgeElem[ednum].x[1],2)+ pow(edgeElem[ednum].y[0] - edgeElem[ednum].y[1], 2));
				if (edgeElem[ednum].dis != 0) {
					medianx = (edgeElem[ednum].x[0] + edgeElem[ednum].x[1]) / 2;
					mediany = (edgeElem[ednum].y[0] + edgeElem[ednum].y[1]) / 2;
					if (medianx > minx&&medianx<maxx&&mediany>miny&&mediany < maxy) {
						edgeElem[ednum].opflag = 1;
					}
					
					psOObj = SHPCreateSimpleObject(SHPT_ARC, 2, edgeElem[ednum].x, edgeElem[ednum].y, NULL);
					SHPWriteObject(ofname, -1, psOObj);

					SHPDestroyObject(psOObj);
					ednum++;
				}
			}	
		}
		else {
			psOObj = SHPCreateSimpleObject(SHPT_ARC, 0, NULL, NULL, NULL);
			cout << "vertices=0\n";
			SHPWriteObject(ofname, -1, psOObj);

			SHPDestroyObject(psOObj);
			ver0num[num0] = ednum;
			num0++;
		}
	}
	return 0;
}

//Edgeの接続関係の作成と距離の計算
int EdgeConnectivity(void) {
	for (int i = 0; i < ednum-1; ++i) {
		for (int j = i + 1; j < ednum; ++j) {
			for (int k = 0; k < 2; ++k) {
				for (int l = 0; l < 2; ++l) {
					if (edgeElem[i].x[k] == edgeElem[j].x[l] && edgeElem[i].y[k] == edgeElem[j].y[l]) {
						//距離の計算
						edgeElem[i].ADis[k][edgeElem[i].connum[k]] = AngDis(i, j, k, l);
						edgeElem[j].ADis[l][edgeElem[j].connum[l]] = edgeElem[i].ADis[k][edgeElem[i].connum[k]];
						edgeElem[i].EDis[k][edgeElem[i].connum[k]] = EucDis(i, j);
						edgeElem[j].EDis[l][edgeElem[j].connum[l]] = edgeElem[i].EDis[k][edgeElem[i].connum[k]];

						//接続関係
						edgeElem[i].ConnectEdge[k][edgeElem[i].connum[k]] = j;//セグメント番号
						edgeElem[i].ConnectPoint[k][edgeElem[i].connum[k]] = l;//どちらの端点か0,1
						++edgeElem[i].connum[k];

						edgeElem[j].ConnectEdge[l][edgeElem[j].connum[l]] = i;
						edgeElem[i].ConnectPoint[l][edgeElem[i].connum[l]] = k;
						++edgeElem[j].connum[l];
					}
				}
			}
		}
	}
	return 0;
}


//dijklistの更新
int nextlist(int point) {//新セグメントの追加と前のセグメントの削除
	//初期化
	passededgeflag = 0;//通った場所しかなかったかどうか
	replaceflag = 0;//直前のセグメントの場所に次の一つ目のセグメントを入れたかどうか
	double dicobuf;

	if (edgeElem[minedgenum].connum[point] == 0) {//行き止まりのとき
		dijklist[minedge] = dijklist[dilinum - 1];
		dilinum--;
	}
	else {
		for (int k = 0; k < edgeElem[minedgenum].connum[point]; ++k) {
			if (passededge[edgeElem[minedgenum].ConnectEdge[point][k]] == 0) {
				if (replaceflag == 0) {
					
					dicobuf = dijklist[minedge].precost;
					dijklist[minedge].precostA += edgeElem[minedgenum].ADis[point][k];
					dijklist[minedge].precostE += edgeElem[minedgenum].EDis[point][k];
					if (dijklist[minedge].precost <= maxcost) {
						dijklist[minedge].point = edgeElem[minedgenum].ConnectPoint[point][k];
						dijklist[minedge].waylist[dijklist[minedge].walinum] = edgeElem[minedgenum].ConnectEdge[point][k];
						++dijklist[minedge].walinum;

						passededge[minedgenum] = 1;
						passededgeflag = 1;
						replaceflag = 1;
					}
					else {
						dijklist[minedge].precost -= edgeElem[minedgenum].Dis[point][k];
					}
					
				}
				else {
					
					dijklist[dilinum].precost = dicobuf + edgeElem[minedgenum].Dis[point][k];
					if (dijklist[dilinum].precost <= maxcost) {
						dijklist[dilinum].point = edgeElem[minedgenum].ConnectPoint[point][k];
						for (int l = 0; l < dijklist[minedge].walinum - 1; ++l) {
							dijklist[dilinum].waylist[l] = dijklist[minedge].waylist[l];
						}
						dijklist[dilinum].waylist[dijklist[minedge].walinum - 1] = edgeElem[dilinum].ConnectEdge[point][k];
						dijklist[dilinum].walinum = dijklist[minedge].walinum;

						++dilinum;
					}
				}
			}
		}
		if (passededgeflag == 0) {
			dijklist[minedge] = dijklist[dilinum - 1];
			dilinum--;
		}
	}
	return 0;
}

int dilichange(int listnumber, int finaledge) {

	cost = dijklist[listnumber].precost;
	maxcost = cost;
	int dilinum2 = 0;
	while (dilinum2 < dilinum) {
		if (dijklist[dilinum2].precost > cost) {
			dijklist[dilinum2] = dijklist[dilinum - 1];
			--dilinum;
		}
		else {
			++dilinum2;
		}
	}
	for (int l = 0; l < dilinum; ++l) {
		if (dijklist[l].waylist[dijklist[l].walinum - 1] == finaledge) {
			dijklist[l].precost += 1;
		}
	}
	accessflag = 1;

	return 0;
}

int finornot(int finedge) {//終了判定
	accessflag2 = 0;
	for (int k = 0; k < dilinum; ++k) {
		if (cost > dijklist[k].precost) {//一個でもコストより小さかったら1になる
			accessflag2 = 1;
		}

		if (dijklist[k].waylist[dijklist[k].walinum - 1] == finedge) {
			//コストを計算し、それより小さいセグメントでダイクストラを続ける
			if (accessflag == 0) {
				dilichange(k, finedge);
				break;
			}
			else if (accessflag == 1) {
				//コストを比べる
				if (cost > dijklist[k].precost) {//先に見つけたものより短い
					dilichange(k, finedge);
					break;
				}
				else if (cost == dijklist[k].precost) {//2個目以上
					dijklist[k].precost += 1;
				}
			}
		}
	}
	return 0;
}

int inidijk(int firedge) {//初期化

	//始点終点が決定している中での変数
	fwhile = 0;//dijkstraに入るかどうかのフラグ

	accessflag = 0;

	maxcost = radius;

	//passededgeの初期化通過してないことにする
	for (int k = 0; k < ednum; ++k) {
		passededge[k] = 0;
	}
	passededge[firedge] = 1;

	//dijklistを初期化する
	dilinum = 0;
	if (edgeElem[firedge].connum[0] > 0) {
		for (int k = 0; k < edgeElem[firedge].connum[0]; ++k) {
			dijklist[k].precost = edgeElem[firedge].Dis[0][k];//後で距離変数を変更
			dijklist[k].waylist[0] = firedge;
			dijklist[k].waylist[1] = edgeElem[firedge].ConnectEdge[0][k];
			dijklist[k].point = edgeElem[firedge].ConnectPoint[0][k];
			dilinum++;
		}
	}
	if (edgeElem[firedge].connum[1] > 0) {
		for (int k = 0; k < edgeElem[firedge].connum[1]; ++k) {
			dijklist[dilinum].precost = edgeElem[firedge].Dis[1][k];//後で距離変数を変更
			dijklist[dilinum].waylist[0] = firedge;
			dijklist[dilinum].waylist[1] = edgeElem[firedge].ConnectEdge[1][k];
			dijklist[dilinum].point = edgeElem[firedge].ConnectPoint[1][k];
			dilinum++;
		}
	}
	if (dilinum == 0) {//最初からどこにもつながっていなかった場合
		//終了操作を行う

		fwhile = 1;
	}
	else {
		for (int i = 0; i < dilinum; ++i) {
			dijklist[i].walinum = 2;
		}
	}

	return 0;
}

int SScalc(int ored,int deed) {//i,jdijkstraの最後に計算
	//Choice
	//間のセグメントにcostを再計算して場合分けし、1加算したりしなかったり
	
	if(cost)
	//TotalDepth
	//2セグメントにcostを挿入
	TotalDepth[ored] += cost;
	TotalDepth[deed] += cost;
	//NodeCount
	//2セグメントに1加算
	++NodeCount[ored];
	++NodeCount[deed];
	//TotalSegmentLength
	//2セグメントに加算
	TotalSegmentLength[ored] += edgeElem[deed].dis;
	TotalSegmentLength[deed] += edgeElem[ored].dis;


	return 0;
}

int dijkstra(void) {

	//関数内の変数
	
	dijkflag.resize(shapenum);
	sdijkflag.resize(shapenum);
	for (int j = 1; j < ednum; ++j) {
		sdijkflag[j] = 0;
	}
	for (int i = 0; i < ednum - 1; ++i) {		
		dijkflag[i] = sdijkflag;
	}
	for (int i = 0; i < ednum; ++i) {
		choice[i] = 0;
		NodeCount[i] = 0;
		TotalDepth[i] = 0;
		TotalSegmentLength[i] = 0;
	}
	//始点と終点の組み合わせをすべて回す
	for (int i = 0; i < ednum - 1; ++i) {
		for (int j = i+1; j < ednum; ++j) {

			if (dijkflag[i][j] == 0) {//まだ最短経路が見つかっていなかったら
				//Start=i, End=j

				//初期化
				inidijk(i);

				while (fwhile == 0) {//探索ループ

					//最小のものを選択
					
					mincost = dijklist[0].precost;
					if (dilinum == 0) {//次のリストがなかったら
						//終了操作を行う
						
						break;
					}
					else if (dilinum == 1) {
						minedge = 0;
						minedgenum = dijklist[0].waylist[dijklist[0].walinum - 1];
					}
					else if (dilinum > 1) {
						for (int k = 1; k < dilinum; ++k) {
							if (mincost > dijklist[k].precost) {
								mincost = dijklist[k].precost;
								minedge = k;
								minedgenum = dijklist[minedge].waylist[dijklist[minedge].walinum - 1];
							}
						}
						
					}
					
					//それを削除し新エッジを追加、接続するエッジのリストの作成
					if (dijklist[minedge].point == 0) {
						nextlist(1);
					}
					else {
						nextlist(0);
					}


					//終了判定
					finornot(j);
					
					if (accessflag2 == 0) {//終了

						//dijklistにlastがjのルートのみ残す
						int dilinum3 = 0;
						while (dilinum3 < dilinum) {
							if (dijklist[dilinum3].waylist[dijklist[dilinum3].walinum-1] != j) {
								dijklist[dilinum3] = dijklist[dilinum - 1];
								--dilinum;
							}
							else {
								++dilinum3;
							}
						}

						//終了操作を行う
						SScalc(i, j);

						//同じセグメントを通る経路があるかどうか
						//複数経路があるなら絶対にwalinum>2
						//その間の経路も最短経路
						for (int l = 0; l < dilinum; ++l) {
							if (dijklist[l].walinum > 2) {

								for (int m = 0; m < dijklist[l].walinum - 1; ++m) {
									for (int n = m + 1; n < dijklist[l].walinum; ++n) {//始点m終点nの経路について

										samewayflag = 0;
										samewaynum = 0;
										if (dilinum > 1&& l < dilinum - 1) {
											for (int o = l + 1; o < dilinum; ++o) {
												for (int p = 0; p < dijklist[o].walinum; ++p) {
													if (dijklist[o].waylist[p] == dijklist[l].waylist[m]) {
														samewayflag = 1;
													}
													else if (samewayflag == 1 &&dijklist[o].waylist[p] == dijklist[l].waylist[n]) {
														for (int q = m; q < n + 1; ++q) {
															dijklist[dilinum+samewaynum].waylist[q-m] = dijklist[l].waylist[q];
														}
														dijklist[dilinum + samewaynum].walinum = n - m + 1;
														samewaynum++;
													}
												}
											}

										}
										dijkflag[l][n] = 1;
										//経路等の書き
										

										//終了操作を行う
										SScalc(m,n);
									}
								}
							}
						}
						break;
					}
				}
			}		
		}
	}
	return 0;
}

//nodeの作成
/*
int NodeCreate(void) {
	
	for (int i = 0; i < ednum; ++i) {

		if (nodeiniflag == 0) {
			nodeElem[0].nodex = edgeElem[i].x[0];
			nodeElem[0].nodey = edgeElem[i].y[0];
			nodeElem[0].ConnectEdge[0] = i;

			nodeElem[1].nodex = edgeElem[i].x[1];
			nodeElem[1].nodey = edgeElem[i].y[1];
			nodeElem[1].ConnectEdge[0] = i;

			nodeiniflag = 1;
		}
		else {

			for (int j = 0; j < nonum; ++j) {
				if (edgeElem[i].x[0] == nodeElem[j].nodex && edgeElem[i].y[0] == nodeElem[j].nodey) {

					nodeElem[j].ConnectEdge[nodeElem[j].connum] = i;
					nodeElem[j].connum++;
					nodeflag[0] == 1;

				}
				else if (edgeElem[i].x[1] == nodeElem[j].nodex && edgeElem[i].y[1] == nodeElem[j].nodey) {

					nodeElem[j].ConnectEdge[nodeElem[j].connum] = i;
					nodeElem[j].connum++;
					nodeflag[0] == 1;
				}
			}
			if (nodeflag[0] == 0) {
				nodeElem[nonum].nodex = edgeElem[i].x[0];
				nodeElem[nonum].nodey = edgeElem[i].y[0];
				nodeElem[nonum].ConnectEdge[0] = i;
				nonum++;
			}
			if (nodeflag[1] == 0) {
				nodeElem[nonum].nodex = edgeElem[i].x[1];
				nodeElem[nonum].nodey = edgeElem[i].y[1];
				nodeElem[nonum].ConnectEdge[0] = i;
				nonum++;
			}
			nodeflag[0] = 0;
			nodeflag[1] = 0;
		}
	}
	return 0;
}
*/
/*
int EdgeCalculate(void) {

	for (int i = 0; i < nonum; ++i) {

		if (edgeElem[i].connum[0] > 0) {
			for (int j = 0; j < nodeElem[i].connum - 1; ++j) {
				for (int k = j + 1; k < nodeElem[i].connum; ++k) {
					//Angular距離の計算
					//nodeElem[i].ADis[j][k] = AngDis(edgeElem[nodeElem[i].ConnectEdge[j]].x);
					//Euclidean距離の計算

				}

			}
		}		
	}

	return 0;
}
*/


/*int SHDBim(void) {

	SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
	cout << nEntities << "=nEntities\n" << nShapeType << "=nShapeType\n" << adfMinBound[0] << "<x<" << adfMaxBound[0] << "\n" << adfMinBound[1] << "<y<" << adfMaxBound[1] << "\n";
	cout << adfMinBound[2] << "<z<" << adfMaxBound[2] << "\n" << adfMinBound[3] << "<m<" << adfMaxBound[3] << "\n";
	nField = DBFGetFieldCount(hDBF);
	cout << "nfField:" << nField << "\n";

	for (int i = 0; i < nEntities; ++i) {
		psElem = SHPReadObject(hSHP, i);
		if (psElem->nVertices < 2) {
			cout << psElem->nVertices << ":Vertices\n";
		}
		if (maxver < psElem->nVertices) {
			maxver = psElem->nVertices;
		}
		if (i % testout == 0) {
			cout << "(x,y)=";
		}
		for (int j = 0; j < psElem->nVertices; ++j) {
			padfX[i][j] = psElem->padfX[j];
			padfY[i][j] = psElem->padfY[j];
			if (i%testout == 0) {
				cout << "(" << padfX[i][j] << "," << padfY[i][j] << "),";
			}
		}


		for (int j = 0; j < nField; ++j) {
			eType = DBFGetFieldInfo(hDBF, j, szTitle, &nWidth, &nDecimals);
			if (i % testout == 0) {
				switch (eType) {
				case FTString:
					cout << szTitle << ":" << DBFReadStringAttribute(hDBF, i, j);
					break;
				case FTInteger:
					printf("%s : %d ", szTitle, DBFReadIntegerAttribute(hDBF, i, j));
					break;
				case FTDouble:
					printf("%s : %f ", szTitle, DBFReadDoubleAttribute(hDBF, i, j));
					break;
				case FTLogical:
					cout << szTitle << ":" << DBFReadLogicalAttribute(hDBF, i, j);
					break;
				default:
					cout << szTitle << ":invalid";
					break;
				}
			}

		}

		SHPDestroyObject(psElem);

		if (i % testout == 0) {
			cout << "\n";
		}
	}
	cout << "maxver:" << maxver << "\n";
	return 0;
}*/



int main(void) {
	cout << "start\n";

	hSHP = SHPOpen("DKGSHP533975RdCL20180510000115_2.shp", "rb");
	cout << "SHPread\n";

	hDBF = DBFOpen("DKGSHP533975RdCL20180510000115_2.dbf", "rb");
	cout << "DBFread\n";

	hSHPou = SHPCreate("StraightLineWkLarge", SHPT_ARC);
	outfile.open("m12OS.csv");
	cout << "outputfileCreate\n";

	if (hSHP == NULL) {
		cout << "SHP=NULL";
	}
	else {
		//AorE, radiusの選択
		cout << "imput A or E for Analysis:";
		cin >> Anadistype;
		cout << "imput A or E for Radius:";
		cin >> Raddistype;
		cout << "imput radius(if n, use 0):";
		cin >> radius;
		if (radius == 0) {
			radius = radiusn;
		}

		//エッジの読み込みと出力エッジの選択
		EdgeRead(hSHP,hSHPou);

		//ノードの作成
		//NodeCreate();

		//エッジの接続関係の作成と距離の計算
		EdgeConnectivity();

		//ダイクストラ法
		dijkstra();

		//SpaceSyntax

	}
	SHPClose(hSHP);
	DBFClose(hDBF);
	SHPClose(hSHPou);
	outfile.close();

	return 0;
}