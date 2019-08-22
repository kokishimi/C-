/////////////////////////////////////////////
//���������^�C�v�ƕ��͋����^�C�v�𕪂���/////
//integration�n�͌������a�����邩�m�F���K�v//
//choice�̌v�Z���@���l����///////////////////
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

ofstream outfile;//�o��CSV�p

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



//edge�֐��p
struct edge {//�G�b�W�̏��
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

//�o�͔���p
double medianx, mediany;

//node�֐��p
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


//dijkstra�p
struct stdijk {
	double precost=0;
	int waylist[waynum];
	int walinum = 2;//waylistnum:�ʂ����Z�O�����g��
	int point;
};

vector<vector<int>> dijkflag;//dijkstra���I�����g���ǂ���
vector<int> sdijkflag;

stdijk dijklist[listnum];//���ɒT������Z�O�����g�ԍ����X�g
double cost;//�ŏI�I�ȋ����R�X�g
int dilinum = 0;//dijklist�ɍڂ��Ă���Z�O�����g�̐�

double maxcost;//�ő�R�X�g
double radius;//�������a
string Anadistype;//���͋����^�C�v
string Raddistype;//�������a�����^�C�v

//�ŏ��R�X�g�T���p
double mincost;
int minedge;
int minedgenum;

//���łɒʉ߂����n�_�̔��ʗp
int passededge[nodenum];
int passededgeflag = 0;

int replaceflag;

int fwhile;//while���ɓ��邩�ǂ����̃t���O

int accessflag = 0;//1��ł����X�g�ɏI�_����������ǂ���
int accessflag2 = 0;

int samewayflag;//�����Z�O�����g���g���Ă��邩1
int samewaynum;//���R�X�g�̌o�H�����{���邩


//SpaceSyntax�p
double choice[shapenum];
double integration;//��Ōv�Z
int NodeCount[shapenum];
double MeanDepth;//��Ōv�Z
double TotalDepth[shapenum];
double TotalSegmentLength[shapenum];
//connectivity
//A_connectivity
//StepDepth
//SLW�̌v�Z���@


//�����̌v�ZAngular,Euclidean
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


//Edge�̓ǂݍ��݂Əo�̓G�b�W�̑I��
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

//Edge�̐ڑ��֌W�̍쐬�Ƌ����̌v�Z
int EdgeConnectivity(void) {
	for (int i = 0; i < ednum-1; ++i) {
		for (int j = i + 1; j < ednum; ++j) {
			for (int k = 0; k < 2; ++k) {
				for (int l = 0; l < 2; ++l) {
					if (edgeElem[i].x[k] == edgeElem[j].x[l] && edgeElem[i].y[k] == edgeElem[j].y[l]) {
						//�����̌v�Z
						edgeElem[i].ADis[k][edgeElem[i].connum[k]] = AngDis(i, j, k, l);
						edgeElem[j].ADis[l][edgeElem[j].connum[l]] = edgeElem[i].ADis[k][edgeElem[i].connum[k]];
						edgeElem[i].EDis[k][edgeElem[i].connum[k]] = EucDis(i, j);
						edgeElem[j].EDis[l][edgeElem[j].connum[l]] = edgeElem[i].EDis[k][edgeElem[i].connum[k]];

						//�ڑ��֌W
						edgeElem[i].ConnectEdge[k][edgeElem[i].connum[k]] = j;//�Z�O�����g�ԍ�
						edgeElem[i].ConnectPoint[k][edgeElem[i].connum[k]] = l;//�ǂ���̒[�_��0,1
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


//dijklist�̍X�V
int nextlist(int point) {//�V�Z�O�����g�̒ǉ��ƑO�̃Z�O�����g�̍폜
	//������
	passededgeflag = 0;//�ʂ����ꏊ�����Ȃ��������ǂ���
	replaceflag = 0;//���O�̃Z�O�����g�̏ꏊ�Ɏ��̈�ڂ̃Z�O�����g����ꂽ���ǂ���
	double dicobuf;

	if (edgeElem[minedgenum].connum[point] == 0) {//�s���~�܂�̂Ƃ�
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

int finornot(int finedge) {//�I������
	accessflag2 = 0;
	for (int k = 0; k < dilinum; ++k) {
		if (cost > dijklist[k].precost) {//��ł��R�X�g��菬����������1�ɂȂ�
			accessflag2 = 1;
		}

		if (dijklist[k].waylist[dijklist[k].walinum - 1] == finedge) {
			//�R�X�g���v�Z���A�����菬�����Z�O�����g�Ń_�C�N�X�g���𑱂���
			if (accessflag == 0) {
				dilichange(k, finedge);
				break;
			}
			else if (accessflag == 1) {
				//�R�X�g���ׂ�
				if (cost > dijklist[k].precost) {//��Ɍ��������̂��Z��
					dilichange(k, finedge);
					break;
				}
				else if (cost == dijklist[k].precost) {//2�ڈȏ�
					dijklist[k].precost += 1;
				}
			}
		}
	}
	return 0;
}

int inidijk(int firedge) {//������

	//�n�_�I�_�����肵�Ă��钆�ł̕ϐ�
	fwhile = 0;//dijkstra�ɓ��邩�ǂ����̃t���O

	accessflag = 0;

	maxcost = radius;

	//passededge�̏������ʉ߂��ĂȂ����Ƃɂ���
	for (int k = 0; k < ednum; ++k) {
		passededge[k] = 0;
	}
	passededge[firedge] = 1;

	//dijklist������������
	dilinum = 0;
	if (edgeElem[firedge].connum[0] > 0) {
		for (int k = 0; k < edgeElem[firedge].connum[0]; ++k) {
			dijklist[k].precost = edgeElem[firedge].Dis[0][k];//��ŋ����ϐ���ύX
			dijklist[k].waylist[0] = firedge;
			dijklist[k].waylist[1] = edgeElem[firedge].ConnectEdge[0][k];
			dijklist[k].point = edgeElem[firedge].ConnectPoint[0][k];
			dilinum++;
		}
	}
	if (edgeElem[firedge].connum[1] > 0) {
		for (int k = 0; k < edgeElem[firedge].connum[1]; ++k) {
			dijklist[dilinum].precost = edgeElem[firedge].Dis[1][k];//��ŋ����ϐ���ύX
			dijklist[dilinum].waylist[0] = firedge;
			dijklist[dilinum].waylist[1] = edgeElem[firedge].ConnectEdge[1][k];
			dijklist[dilinum].point = edgeElem[firedge].ConnectPoint[1][k];
			dilinum++;
		}
	}
	if (dilinum == 0) {//�ŏ�����ǂ��ɂ��Ȃ����Ă��Ȃ������ꍇ
		//�I��������s��

		fwhile = 1;
	}
	else {
		for (int i = 0; i < dilinum; ++i) {
			dijklist[i].walinum = 2;
		}
	}

	return 0;
}

int SScalc(int ored,int deed) {//i,jdijkstra�̍Ō�Ɍv�Z
	//Choice
	//�Ԃ̃Z�O�����g��cost���Čv�Z���ďꍇ�������A1���Z�����肵�Ȃ�������
	
	if(cost)
	//TotalDepth
	//2�Z�O�����g��cost��}��
	TotalDepth[ored] += cost;
	TotalDepth[deed] += cost;
	//NodeCount
	//2�Z�O�����g��1���Z
	++NodeCount[ored];
	++NodeCount[deed];
	//TotalSegmentLength
	//2�Z�O�����g�ɉ��Z
	TotalSegmentLength[ored] += edgeElem[deed].dis;
	TotalSegmentLength[deed] += edgeElem[ored].dis;


	return 0;
}

int dijkstra(void) {

	//�֐����̕ϐ�
	
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
	//�n�_�ƏI�_�̑g�ݍ��킹�����ׂĉ�
	for (int i = 0; i < ednum - 1; ++i) {
		for (int j = i+1; j < ednum; ++j) {

			if (dijkflag[i][j] == 0) {//�܂��ŒZ�o�H���������Ă��Ȃ�������
				//Start=i, End=j

				//������
				inidijk(i);

				while (fwhile == 0) {//�T�����[�v

					//�ŏ��̂��̂�I��
					
					mincost = dijklist[0].precost;
					if (dilinum == 0) {//���̃��X�g���Ȃ�������
						//�I��������s��
						
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
					
					//������폜���V�G�b�W��ǉ��A�ڑ�����G�b�W�̃��X�g�̍쐬
					if (dijklist[minedge].point == 0) {
						nextlist(1);
					}
					else {
						nextlist(0);
					}


					//�I������
					finornot(j);
					
					if (accessflag2 == 0) {//�I��

						//dijklist��last��j�̃��[�g�̂ݎc��
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

						//�I��������s��
						SScalc(i, j);

						//�����Z�O�����g��ʂ�o�H�����邩�ǂ���
						//�����o�H������Ȃ��΂�walinum>2
						//���̊Ԃ̌o�H���ŒZ�o�H
						for (int l = 0; l < dilinum; ++l) {
							if (dijklist[l].walinum > 2) {

								for (int m = 0; m < dijklist[l].walinum - 1; ++m) {
									for (int n = m + 1; n < dijklist[l].walinum; ++n) {//�n�_m�I�_n�̌o�H�ɂ���

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
										//�o�H���̏���
										

										//�I��������s��
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

//node�̍쐬
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
					//Angular�����̌v�Z
					//nodeElem[i].ADis[j][k] = AngDis(edgeElem[nodeElem[i].ConnectEdge[j]].x);
					//Euclidean�����̌v�Z

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
		//AorE, radius�̑I��
		cout << "imput A or E for Analysis:";
		cin >> Anadistype;
		cout << "imput A or E for Radius:";
		cin >> Raddistype;
		cout << "imput radius(if n, use 0):";
		cin >> radius;
		if (radius == 0) {
			radius = radiusn;
		}

		//�G�b�W�̓ǂݍ��݂Əo�̓G�b�W�̑I��
		EdgeRead(hSHP,hSHPou);

		//�m�[�h�̍쐬
		//NodeCreate();

		//�G�b�W�̐ڑ��֌W�̍쐬�Ƌ����̌v�Z
		EdgeConnectivity();

		//�_�C�N�X�g���@
		dijkstra();

		//SpaceSyntax

	}
	SHPClose(hSHP);
	DBFClose(hDBF);
	SHPClose(hSHPou);
	outfile.close();

	return 0;
}