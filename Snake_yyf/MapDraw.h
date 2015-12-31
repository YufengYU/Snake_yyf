#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#ifndef const_values
#define const_values
const int maxn=20;
const int max_round = 600;
const int dx[4]={-1,0,1,0};
const int dy[4]={0,1,0,-1};
#endif

bool DrawMap(int result_map[maxn][maxn], int n, int m, int round1, int round2);
inline int calcLenByRound(const int& round_now);