#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include"jsoncpp/json.h"
using namespace std;

const int maxn=25+2;
const int dx[4]={-1,0,1,0};
const int dy[4]={0,1,0,-1};
struct point
{
	int x,y;
	point(){};
	point(int _x,int _y)
	{
		x=_x;
		y=_y;
	}
};

int n,m,len,round_now;
vector< vector<int> > result_map(maxn,vector<int>(maxn,0));
vector <point> snakes[2]; // 0表示自己的蛇，1表示对方的蛇


bool whetherGrow(int round_now)  //本回合是否生长
{
	if (round_now<=9) return true;
	if ((round_now-9)%3==0) return true;
	return false;
}

void move(vector<point>& snake,int dire)  //编号为id的蛇朝向dire方向移动一步
{
	point p=*(snake.rbegin());
	int x=p.x+dx[dire];
	int y=p.y+dy[dire];
	snake.push_back(point(x,y));
}

void outputSnakeBody(const vector<point>& snake, const int& len)    //调试语句
{
    int n = snake.size();
    for (int i=n-1; i+len>=n; i--)
    {
        //cout << i << endl;
        cout << snake[i].x << "\t" << snake[i].y << endl;
    }
}

void outputSnakeBody(int id, const int& len)
{
   	cout<<"Snake No."<<id<<endl;
   	//cout << snakes[id].size() << " " << len << endl;
    outputSnakeBody(snakes[id],len);
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


double CalcWeakRate(int steps, double range)
{
	const double factor = 0.1;
	double ret = 0;
	for (int i=0; i<steps; i++)
		ret = (1-ret)*factor + ret;
	return ret*range;
}

double CalcRealFactor(double x)
{
	return x;
	return 1 - (1-x)*(1-x);
}

double EvaluateWinRateMonteCarlo(const vector< vector<int> > &result_map, vector<point> snakes[2])
{
    const int sample_num = 2000;
    double win = 0;

    vector< vector<int> > tmp_map = result_map;
    int total;
    int min_val;
    point p[2];
    int dire[2][5];
	int direCount[2];
	int choose;
	int steps;
	double tmp;

	int steps_sum = 0;
	int i;
    for (i=0; i<sample_num && steps_sum<100000; i++)
    {
		steps = 0;
        total = snakes[0].size();
		for (int x=1; x<=n; x++)
			for (int y=1; y<=m; y++)
				if (tmp_map[x][y]>(total-1)*2)
					tmp_map[x][y] = 0;

        p[0] = snakes[0][total-1];
        p[1] = snakes[1][total-1];
        while ( (p[0].x!=p[1].x || p[0].y!=p[1].y)
               && checkValue(tmp_map[p[0].x][p[0].y], total)
               && checkValue(tmp_map[p[1].x][p[1].y], total) )
        {
			for (int j=0; j<2; j++)
				tmp_map[p[j].x][p[j].y] = (total-1)*2+j+1;
			
            for (int j=0; j<2; j++)
            {
				direCount[j]=0;
				for (int k=0; k<4; k++)
				{
					if (checkValue(tmp_map[p[j].x+dx[k]][p[j].y+dy[k]], total+1))
						dire[j][direCount[j]++] = k;
				}
				if (direCount[j]==0)
					dire[j][direCount[j]++] = 0;
				choose = rand()%direCount[j];

                p[j].x += dx[dire[j][choose]];
                p[j].y += dy[dire[j][choose]];
            }
            total++;
			steps++;
        }

		steps_sum+=steps+1;
        //计算成功率
        if (p[0].x==p[1].x && p[0].y==p[1].y)
        {
            tmp = 0.5 + CalcWeakRate(steps,0.2);
            continue;
        }
        if (checkValue(tmp_map[p[0].x][p[0].y], total))
        {
            tmp = 1.0 - 0.2 + CalcWeakRate(steps,0.2);
        }else if (!checkValue(tmp_map[p[1].x][p[1].y], total))
		{
            tmp = 0.5 - 0.2 + CalcWeakRate(steps,0.2);
		}else
			tmp= 0.0 + CalcWeakRate(steps,0.2);
		
		win += CalcRealFactor(tmp);
    }

	if (i==0)
		return 0;
    return win/i;
}

double EvaluateWinRate(const vector< vector<int> > &result_map, vector<point> snakes[2])
{
    return EvaluateWinRateMonteCarlo(result_map, snakes);
}

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
		//fin.open("SampleAI_in_1.txt");
		//fout.open("SampleAI_out.txt");
		//cin.rdbuf(fin.rdbuf());
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
		snakes[0].push_back(point(1,1));
		snakes[1].push_back(point(n,m));
	}
	else
	{
		snakes[1].push_back(point(1,1));
		snakes[0].push_back(point(n,m));
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
	round_now=input["responses"].size();
	len = 1;
	int dire;
	for (int i=0;i<round_now;i++)
	{
		dire=input["responses"][i]["direction"].asInt();
		move(snakes[0],dire);

		dire=input["requests"][i+1]["direction"].asInt();
		move(snakes[1],dire);

		if (whetherGrow(i))
			len++;
	}

	// 根据蛇的位置恢复地图
	for (int i=0; i<2; i++)
		for (int j=snakes[i].size()-len; j<snakes[i].size(); j++)
			result_map[snakes[i][j].x][snakes[i][j].y] = j*2+i+1;

	//outputSnakeBody(0,len);
	//outputSnakeBody(1,len);

	srand((unsigned)time(NULL));

	double min_best,max_best,tmp_best;
	int best_dire;

	max_best = -1;
	for (int i=0; i<4; i++)
	{
		move(snakes[0],i);
		min_best = 2;
		for (int j=0; j<4; j++)
		{
			move(snakes[1],j);
			if (i==1 && j==3)
				j = 3;
			tmp_best = EvaluateWinRate(result_map, snakes);
			snakes[1].pop_back();

			if (tmp_best < min_best)
				min_best = tmp_best;
		}
		snakes[0].pop_back();

		if (min_best > max_best)
		{
			max_best = min_best;
			best_dire = i;
		}
	}

	//随机做出一个决策
	Json::Value ret;
	ret["response"]["direction"]=best_dire;

	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;
	fin.close();
	fout.close();
	return 0;
}