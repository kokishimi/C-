#include "shapefil.h"
#include <iostream>
#include <string>

#define vernum 200
#define entnum 100000
#define testout 100000

using namespace std;

SHPHandle hSHP;
int nShapeType, nEntities;
double adfMinBound[4], adfMaxBound[4];

SHPObject *psElem;
double padfX[entnum][vernum], padfY[entnum][vernum],padfZ[entnum][vernum];

int nField;
DBFFieldType eType;
char szTitle[11];
int nWidth, nDecimals;
char *pszTypeName;

string szDBF;
DBFHandle hDBF;

int maxver=0;

SHPHandle hSHPou;
//DBFHandle hDBFou;

SHPObject *psOObj;
double fox[2], foy[2];

int sumvertices = 0;
int sum0ver = 0;

int SHDBim(void) {

	SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
	cout << nEntities << "=nEntities\n" << nShapeType << "=nShapeType\n" << adfMinBound[0] << "<x<" << adfMaxBound[0] << "\n" << adfMinBound[1] << "<y<" << adfMaxBound[1] << "\n";
	cout << adfMinBound[2] << "<z<" << adfMaxBound[2] << "\n" << adfMinBound[3] << "<m<" << adfMaxBound[3] << "\n";
	nField = DBFGetFieldCount(hDBF);
	cout << "nfField:" << nField << "\n";

	for (int i = 0; i < nEntities; ++i) {
		psElem = SHPReadObject(hSHP, i);
		sumvertices += psElem->nVertices;
		if (psElem->nVertices < 2) {
			if (psElem->nVertices == 1) {
				cout << psElem->nVertices << ":Vertices\n";
			}
			++sum0ver;

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
	cout << "few vertices number:" << sum0ver << "\n";
	cout << "maxver:" << maxver << "\n";
	cout << "sumvertices:" << sumvertices << "\n";
	cout << "for SS program:" << sumvertices - nEntities << "\n";
	return 0;
}

int SHline(SHPHandle ifname, SHPHandle ofname) {
	SHPGetInfo(ifname, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
	cout << nEntities;
	for (int i = 0; i < nEntities; ++i) {
		
		psElem = SHPReadObject(ifname, i);
		if (psElem->nVertices >= 2) {
			fox[0] = psElem->padfX[0];
			fox[1] = psElem->padfX[psElem->nVertices - 1];
			foy[0] = psElem->padfY[0];
			foy[1] = psElem->padfY[psElem->nVertices - 1];

			if (i % 5000 == 0) {
				cout << "(x,y)=(" << fox[0] << "," << foy[0] << "),(" << fox[1] << "," << foy[1] << ")\n";
			}

			psOObj = SHPCreateSimpleObject(SHPT_ARC, 2, fox, foy, NULL);
			SHPWriteObject(ofname, -1, psOObj);

			SHPDestroyObject(psOObj);
		}
		else {
			psOObj = SHPCreateSimpleObject(SHPT_ARC, 0, NULL, NULL, NULL);
			cout<<"vertices=0\n";
			SHPWriteObject(ofname, -1, psOObj);

			SHPDestroyObject(psOObj);
		}
		
	}
	return 0;
}

int main(void) {
	cout << "start\n";

	hSHP = SHPOpen("DKGSHP533975RdCL20180510000115_2.shp", "rb");
	cout << "SHPread\n";

	hDBF = DBFOpen("DKGSHP533975RdCL20180510000115_2.dbf", "rb");
	cout << "DBFread\n";

	//hSHPou = SHPCreate("StraightLineWkLargeStaonly", SHPT_ARC);

	if (hSHP == NULL) {
		cout << "SHP=NULL";
	}
	else {
		
		SHDBim();
		//SHline(hSHP,hSHPou);

	}
	SHPClose(hSHP);
	DBFClose(hDBF);
	//SHPClose(hSHPou);

	return 0;
}