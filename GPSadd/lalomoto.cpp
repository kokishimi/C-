#pragma once
#include "stdio.h"
#include "conio.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

double  PI = 3.1415926535897932384626;

double S; //赤道からの子午線弧長
double S0; //原点の赤道からの子午線弧長
double SS; //赤道からの子午線弧長
double SS0; //原点の赤道からの子午線弧長

double V, W; //楕円体の諸公式で利用
double e;//：第一離心率
double et;//：第二離心率
double M;//：子午線曲率半径
double N;//：卯酉線曲率半径
double c;//：極での曲率半径

double m0 = 0.99990000;// ：座標系の原点における縮尺係数
double m;     //：座標(x,y)の点における縮尺係数
double a = 6378137; //長半径
double f = 1.0 / 298.257222101;  //扁平率

double n = f / (2 - f); //一般式 n

double B1, B2, B3, B4, B5, B6, B7, B8, B9;
double A, B, C, D, E, F, G, H, I;

double Lon0[20] = {
	0,129.5,131.0, 132.166666,133.5,134.333333,136.0,137.1666,138.5,139.83333,140.8333,//0～10系
	140.25, 142.25,144.25,142,127.5,124,131,136,154 };  //11～19系                                           //11系～19系

double Lat0[20] = { 0.0,33.0,33.0,36.0,33.0,36.0,36.0,36.0,36.0,36.0,40.0,//0～10系(0はダミー)
44.0,44.0,44.0,26.0,26.0,26.0,26.0,20.0,26.0 }; //11～19系

int k;//平面直角座標系の番号（原点）

double t;//tanLat
double eta2; //et*et*cosLat*cosLat;
double dLon;
double Lon, Lat, Lon0Rad, Lat0Rad;
double x, y;

/*******************************************************************/
double calc_ABC(void) {//変更
	A = 1.0 + 3.0*pow(e, 2.0) / 4.0 + 45.0 / 64.0*pow(e, 4.0) + 175.0 / 256.0*pow(e, 6.0) + 11025.0 / 16384.0*pow(e, 8.0) + 43659.0 / 65536.0*pow(e, 10.0) + 693693.0 / 1048576.0*pow(e, 12.0)
		+ 19324305.0 / 29360128.0*pow(e, 14.0) + 4927697775.0 / 7516192768.0*pow(e, 16.0);
	B = 3.0 / 4.0*pow(e, 2.0) + 15.0 / 16.0*pow(e, 4.0) + 525.0 / 512.0*pow(e, 6.0) + 2205.0 / 2048.0*pow(e, 8.0) + 72765.0 / 65536.0*pow(e, 10.0) + 297297.0 / 262144.0*pow(e, 12.0)
		+ 135270135.0 / 117440512.0*pow(e, 14.0) + 547521975.0 / 469762048.0*pow(e, 16.0);
	C = 15.0 / 64.0*pow(e, 4.0) + 105.0 / 256.0*pow(e, 6.0) + 2205.0 / 4096.0*pow(e, 8.0) + 10395.0 / 16384.0*pow(e, 10.0) + 1486485.0 / 2097152.0*pow(e, 12.0) + 45090045.0 / 58720256.0*pow(e, 14.0) + 766530765.0 / 939524096.0*pow(e, 16.0);
	D = 35.0 / 512.0*pow(e, 6.0) + 315.0 / 2048.0*pow(e, 8.0) + 31185.0 / 131072.0*pow(e, 10.0) + 165165.0 / 524288.0*pow(e, 12.0) + 45090045.0 / 117440512.0*pow(e, 14.0) + 209053845.0 / 469762048.0*pow(e, 16.0);
	E = 315.0 / 16384.0*pow(e, 8.0) + 3465.0 / 65536.0*pow(e, 10.0) + 99099.0 / 1048576.0*pow(e, 12.0) + 4099095.0 / 29360128.0*pow(e, 14.0) + 348423075.0 / 1879048192.0*pow(e, 16.0);
	F = 693.0 / 131072.0*pow(e, 10.0) + 9009.0 / 524288.0*pow(e, 12.0) + 4099095.0 / 117440512.0*pow(e, 14.0) + 26801775.0 / 469762048.0*pow(e, 16.0);
	G = 3003.0 / 2097152.0*pow(e, 12.0) + 315315.0 / 58720256.0*pow(e, 14.0) + 11486475.0 / 939524096.0*pow(e, 16.0);
	H = 45045117440512.0*pow(e, 14.0) / +765765.0 / 469762048.0*pow(e, 16.0);
	I = 765765.0 / 7516192768.0*pow(e, 16.0);

	B1 = a * (1.0 - e * e)*A;
	B2 = a * (1.0 - e * e)*(-B / 2.0);
	B3 = a * (1.0 - e * e)*(C / 4.0);
	B4 = a * (1.0 - e * e)*(-D / 6.0);
	B5 = a * (1.0 - e * e)*(E / 8.0);
	B6 = a * (1.0 - e * e)*(-F / 10.0);
	B7 = a * (1.0 - e * e)*(G / 12.0);
	B8 = a * (1.0 - e * e)*(-H / 14.0);
	B9 = a * (1.0 - e * e)*(I / 16.0);
	return 0;//変更
}

/********************************************************************/
/* 赤道から緯度 Lat までの子午線弧長 S を求める */
double calc_S(double Lat) {
	double x;
	//	x = B1*Lat - B2*sin(2*Lat) + B3*sin(4*Lat) - B4*sin(6*Lat) + B5*sin(8*Lat) - B6*sin(10*Lat);

	x = B1 * Lat + B2 * sin(2.0*Lat) + B3 * sin(4.0*Lat) + B4 * sin(6.0*Lat) + B5 * sin(8.0*Lat) + B6 * sin(10.0*Lat) + B7 * sin(12.0*Lat) + B8 * sin(14.0*Lat) + B9 * sin(16.0*Lat);
	return x;
}
/********************************************************************/
/* εの計算
double calc_eps(int i){
double eps;
eps = 3*n/(2*i-n);
return eps;
}
double calc_epsk(int j){
int k;
double product1 = 1;
for(k=1; k<=j; k++){
product1 = product1 * calc_eps(k);
}
return product1;
}
double calc_epsm(int l, int j){
int m;
double q;
double product2 = 1;
for(m=1; m<=l; m++){
q = j+pow(-1,m);
if(q>m/2){
q = m/2;
}
product2 = product2 * pow(calc_eps(q),pow(-1,m));
}
return product2;
}
/*******************************************************************
/* sin項の計算
double calc_sins(int j, double Lat){
int l;
double sigma2 = 0;
for(l=1; l<=2*j; l++){
sigma2 = sigma2 + (1/l-4*l)*sin(2*l)*Lat*calc_epsm(l, j);
}
return sigma2;
}
/*******************************************************************
/* 赤道から緯度 Lat までの子午線弧長 S を求める（一般式）
double calc_SS(double Lat) {
double x, sigma1;
int j;
int jMax = 4;

sigma1 = 0;
for(j=1; j<jMax; j++){
sigma1 = sigma1 + pow(calc_epsk(j),2.0)*(Lat + calc_sins(j,Lat));
}


x = a/(1+n) * sigma1;

return x;
}
/********************************************************************/
int xy(double LonDeg, double LatDeg) {

	k = 9;


	Lon = LonDeg * PI / 180.0;

	Lat = LatDeg * PI / 180.0;

	e = sqrt(2.0*f - f * f);

	et = sqrt(2.0*(1.0 / f) - 1.0) / ((1.0 / f) - 1.0);

	W = sqrt(1.0 - e * e*sin(Lat)*sin(Lat));

	V = sqrt(1.0 + et * et*cos(Lat)*cos(Lat));

	c = a / (1 - f);

	N = c / V;

	M = c / V / V / V;

	t = tan(Lat);

	eta2 = et * et*cos(Lat)*cos(Lat);

	calc_ABC();

	S = calc_S(Lat);

	Lat0Rad = Lat0[k] * PI / 180.0;
	S0 = calc_S(Lat0Rad);

	Lon0Rad = Lon0[k] * PI / 180.0;
	dLon = Lon - Lon0Rad;

	/* x 座標 */
	x = (
		(S - S0) + N * 1.0 / 2.0*cos(Lat)*cos(Lat)*t*dLon*dLon
		+ 1.0 / 24.0*N*pow(cos(Lat), 4.0)*t*(5.0 - t * t + 9.0*eta2 + 4.0*eta2*eta2)*pow(dLon, 4.0)
		- 1.0 / 720.0*N*pow(cos(Lat), 6.0)*t*(-61.0 + 58.0*t*t - pow(t, 4.0) - 270.0*eta2 + 330.0*t*t*eta2)*pow(dLon, 6.0)
		- 1.0 / 40320.0*N*pow(cos(Lat), 8.0)*t*(-1385.0 + 3111.0*t*t - 543.0*pow(t, 4.0) + pow(t, 6.0))*pow(dLon, 8.0)
		)*m0;

	/* y 座標 */
	y = (
		N*cos(Lat)*dLon
		- 1.0 / 6.0*N*pow(cos(Lat), 3.0)*(-1.0 + t * t - eta2)*pow(dLon, 3.0)
		- 1.0 / 120.0*N*pow(cos(Lat), 5.0)*(-5.0 + 18.0*t*t - pow(t, 4.0) - 14.0*eta2 + 58.0*t*t*eta2)*pow(dLon, 5.0)
		- 1.0 / 5040.0*N*pow(cos(Lat), 7.0)*(-61.0 + 479.0*t*t - 179.0*pow(t, 4.0) + pow(t, 6.0))*pow(dLon, 7.0)
		)*m0;


	return 0;
}