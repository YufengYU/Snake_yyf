#define ShouldDrawMap 1

#if ShouldDrawMap
#include "MapDraw.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include"jsoncpp/json.h"
using namespace std;

#ifndef const_values
#define const_values
const int maxn=20;
const int max_round = 600;
const int dx[4]={-1,0,1,0};
const int dy[4]={0,1,0,-1};
#endif

struct point
{
	int x,y;
	point(int _x=0,int _y=0)
	{
		x=_x;
		y=_y;
	}
};

int n,m,round_now;
int result_map[maxn][maxn]={0};
int backup_map[maxn][maxn]={0};
double weight_map[maxn][maxn]={0};
int belong_map[maxn][maxn]={0};
point point_list[max_round];

int map_size = sizeof(int)*maxn*maxn;
point snakes[2][max_round]; // 0表示自己的蛇，1表示对方的蛇

void yyfDrawMap(int result_map[maxn][maxn], int n, int m, int round1, int round2)
{
#if ShouldDrawMap
	DrawMap(result_map,n,m,round1,round2);
#endif
	return;
}

bool whetherGrow(int round_now)  //本回合是否生长
{
	if (round_now<=9) return true;
	if ((round_now-9)%3==0) return true;
	return false;
}

inline int calcLenByRound(const int& round_now)
{
	if (round_now<=10) return round_now+1;
	return (round_now-10)/3 + 11;
}

void move(int id, int round, int dire)  //编号为id的蛇朝向dire方向移动一步
{
	snakes[id][round+1].x = snakes[id][round].x + dx[dire];
	snakes[id][round+1].y = snakes[id][round].y + dy[dire];
	result_map[snakes[id][round+1].x][snakes[id][round+1].y] = (round+1)*2+id+1;
}

void moveBack(int id, int round)
{
	result_map[snakes[id][round].x][snakes[id][round].y] = 0;
	if (!whetherGrow(round-1))
	{
		int pose = round - calcLenByRound(round);
		result_map[snakes[id][pose].x][snakes[id][pose].y] = pose*2+id+1;
	}
}

inline bool checkValue(const int& v1, const int& v2)
{
    if (v1<0)
        return false;
	if (v1==0)
		return true;
    if ( v1 > ((v2-11)-(v2-11)/3)*2 )
        return false;
    return true;
}

int Rand(int p)   //随机生成一个0到p的数字
{
	return rand()%p;
}

double EvaluateWinRate(int result_map[maxn][maxn], int n, int m, int round)
{
	int i,j;
	point p,pn;
	double result1,result2;
	int num;

	memcpy(backup_map,result_map,map_size);
	memset(weight_map,0,sizeof(weight_map));
	memset(belong_map,0,sizeof(belong_map));


	// 计算权值
	for (i=0; i<=n; i++)
		for (j=1; j<=m; j++)
			weight_map[i][j] = 1;

	// 宽度优先搜索计算
	point_list[0] = snakes[0][round];
	point_list[1] = snakes[1][round];
	
	int l,r;
	int v1,v2;
	l = 0;
	r = 2;
	num = 0;

	while (l<r)
	{
		p = point_list[l++];
		if (belong_map[p.x][p.y]==3)
			continue;
		for (i=0; i<4; i++)
		{
			pn.x = p.x + dx[i];
			pn.y = p.y + dy[i];
			v1 = backup_map[pn.x][pn.y];
			v2 = backup_map[p.x][p.y]+2;
			if (v2%2==0)
			{
				if (v2-v1==1 && belong_map[pn.x][pn.y]==1)
				{
					belong_map[pn.x][pn.y] = 3;
					num--;
				}else if (checkValue(v1,(v2-1)/2))
				{
					backup_map[pn.x][pn.y] = backup_map[p.x][p.y]+2;
					belong_map[pn.x][pn.y] = 2;
					point_list[r++] = pn;
					num++;
				}
			}else
			{
				if (checkValue(v1,(v2-1)/2))
				{
					backup_map[pn.x][pn.y] = backup_map[p.x][p.y]+2;
					belong_map[pn.x][pn.y] = 1;
					point_list[r++] = pn;
					num++;
				}
			}
		}
	}
	
	/*
	cout << num << endl;
	for (j=1; j<=m; j++)
	{
		for (i=1; i<=n; i++)
			cout << setw(3) << belong_map[i][j];
		cout << endl;
	}*/

	int tmpsum = 0;
	result1 = result2 = 0;
	for (i=1; i<=n; i++)
		for (j=1; j<=m; j++)
			if (belong_map[i][j]==1)
				result1 += weight_map[i][j];
			else if (belong_map[i][j]==2)
				tmpsum++;
				result2 += weight_map[i][j];
	if (result1+result2==0)
		return 0;
	cout << result1 << " " << result2 << endl;
	return result1 / (result1+result2);
}

//double MiniMax(round, )


int main(int argc, char** argv)
{

	//重定义输入输出，测试时需要使用
	ifstream fin;
	ofstream fout;
	if (argc==3)
	{
		fin.open(argv[1]);
		fout.open(argv[2]);
		cin.rdbuf(fin.rdbuf());
		cout.rdbuf(fout.rdbuf());
	}else
	{
		fin.open("SampleAI_in.txt");
		//fout.open("SampleAI_out.txt");
		cin.rdbuf(fin.rdbuf());
		//cout.rdbuf(fout.rdbuf());
	}

	string str;
	string temp;
	while (getline(cin,temp))
		str+=temp;

	Json::Reader reader;
	Json::Value input;
	reader.parse(str,input);

	n=input["requests"][(Json::Value::UInt) 0]["height"].asInt();  //棋盘高度
	m=input["requests"][(Json::Value::UInt) 0]["width"].asInt();   //棋盘宽度

	//初始化地图
	for (int i=0; i<=n+1; i++)
	{
		for (int j=0; j<=m+1; j++)
		{
			if (i==0 || i==n+1 || j==0 || j==m+1)
				result_map[i][j] = -1;
			else
				result_map[i][j] = 0;
		}
	}

	int x=input["requests"][(Json::Value::UInt) 0]["x"].asInt();  //读蛇初始化的信息
	if (x==1)
	{
		snakes[0][0] = point(1,1);
		snakes[1][0] = point(n,m);
	}
	else
	{
		snakes[1][0] = point(1,1);
		snakes[0][0] = point(n,m);
	}

	//处理地图中的障碍物
	int obsCount=input["requests"][(Json::Value::UInt) 0]["obstacle"].size();
	for (int i=0;i<obsCount;i++)
	{
		int ox=input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["x"].asInt();
		int oy=input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["y"].asInt();
		result_map[ox][oy]=-1;
	}

	//根据历史信息恢复现场
	result_map[snakes[0][0].x][snakes[0][0].y] = 1;
	result_map[snakes[1][0].x][snakes[1][0].y] = 2;

	round_now=input["responses"].size();
	int dire;
	for (int i=0;i<round_now;i++)
	{
		dire=input["responses"][i]["direction"].asInt();
		move(0, i, dire);

		dire=input["requests"][i+1]["direction"].asInt();
		move(1, i, dire);
	}




	/*
	yyfDrawMap(result_map,n,m,round_now,round_now);
	while (round_now>0)
	{
		moveBack(0,round_now);
		moveBack(1,round_now);
		round_now--;
		yyfDrawMap(result_map,n,m,round_now, round_now);
	}

	const int moves_num = 22;
	int moves[moves_num] = {1,2,2,2,2,2,1,1,1,1,0,0,0,3,3,3,3,3,2,2,2,2};
	for (int i=0; i<moves_num; i++)
	{

		move(0,round_now,moves[round_now]);
		round_now++;
		yyfDrawMap(result_map,n,m,round_now,round_now);
	}

	while (round_now>0)
	{
		moveBack(0,round_now);
		round_now--;
		yyfDrawMap(result_map,n,m,round_now, round_now);
	}*/

yyfDrawMap(result_map,n,m,round_now, round_now);

	cout << EvaluateWinRate(result_map, n, m, round_now);

	yyfDrawMap(result_map,n,m,round_now, round_now);
	srand((unsigned)time(NULL));

	//随机做出一个决策
	Json::Value ret;
	ret["response"]["direction"]=0;

	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;
	fin.close();
	fout.close();
	return 0;
}