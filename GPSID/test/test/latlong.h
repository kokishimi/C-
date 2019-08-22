inline int xy(double LonDeg, double LatDeg);
extern double x, y;
inline int csvr(void);
#define MAXC 100
#define MAXT 3000
extern char strvec2[MAXC][MAXT];
inline int csv_read(long j, char F_[MAXC][MAXT], FILE *fp_);