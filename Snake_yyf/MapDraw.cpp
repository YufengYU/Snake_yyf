#include "MapDraw.h"


const int BlockSize = 20;
const int CellSize = 15;
const CvScalar colors[6] = {cvScalar(0,0,128),cvScalar(128,0,0),cvScalar(0,0,255),cvScalar(255,0,0),cvScalar(0,0,50),cvScalar(50,0,0)};
CvScalar now_color;
const char WindowName[] = "snake";
bool windowOpened = false;
int waitTime = 0;


int calcColor(int round, int len, int value)
{
	int offset = (value-1)%2;

	value = (value-1)/2;
	if (value<=round-len)
		return 0+offset;
	if (value<=round)
		return 2+offset;
	return 4+offset;
}

bool DrawMap(int result_map[maxn][maxn], int n, int m, int round1, int round2)
{
	if (!windowOpened)
		cvNamedWindow(WindowName);
	int width = BlockSize*2*(m+2);
	int height = BlockSize*2*(n+2);
	int i,j,k;
	int len[2];
	int round[2];
	round[0] = round1;
	round[1] = round2;
	len[0] = calcLenByRound(round[0]);
	len[1] = calcLenByRound(round[0]);
	IplImage* img = cvCreateImage(cvSize(height,width),IPL_DEPTH_8U,3);
	cvZero(img);

	for (j=1; j<=m; j++)
	{
		for (i=1; i<=n; i++)
		{
			cout << setw(3) << result_map[i][j];
		}
		cout << endl;
	}

	int x,y,xx,yy;

	for (j=1; j<=m+1; j++)
	{
		x = 0;
		y = (j*2)*BlockSize;
		xx = ((n+1)*2)*BlockSize;
		yy = y;
		cvLine(img,cvPoint(x,y),cvPoint(xx,yy),cvScalar(128,128,128),2,4);
	}

	for (i=1; i<=n+1; i++)
	{
		x = (i*2)*BlockSize;
		y = 0;
		xx = x;
		yy = ((m+1)*2)*BlockSize;
		cvLine(img,cvPoint(x,y),cvPoint(xx,yy),cvScalar(128,128,128),2,4);
	}


	for (j=0; j<=m+1; j++)
		for (i=0; i<=n+1; i++)
			if (result_map[i][j]<0)
			{
				x = (i*2)*BlockSize;
				y = (j*2)*BlockSize;
				xx = x + BlockSize*2;
				yy = y + BlockSize*2;
				cvRectangle(img,cvPoint(x,y),cvPoint(xx,yy),cvScalar(128,128,128),-1);
			}
	


	// 画不足蛇尾部分
	for (i=1; i<=n; i++)
	{
		for (j=1; j<=m; j++)
		{
			if (result_map[i][j]>0)
			{
				int tmp = (result_map[i][j]+1)%2;
				int now = calcColor(round[tmp],len[tmp],result_map[i][j]);
				if (now>=2)
					continue;

				x = (i*2+1)*BlockSize;// - dx[(result_map-1)%4]*CellSize;
				y = (j*2+1)*BlockSize;// - dy[(result_map-1)%4]*CellSize;
				for (k=0; k<4; k++)
				{
					xx = x + dx[k];
					yy = y + dy[k];
					if (result_map[i+dx[k]][j+dy[k]]==result_map[i][j]+2)
					{
						xx = ((i+dx[k])*2+1)*BlockSize;
						yy = ((j+dy[k])*2+1)*BlockSize;
						cvLine(img,cvPoint(x,y),cvPoint(xx,yy),colors[now],BlockSize);
						break;
					}
				}
				if (k>=4)
				{
					int tmp = (result_map[i][j]+1)%2;
					cvCircle(img,cvPoint(x,y), CellSize, colors[now], 5);
				}
			}
		}
	}

	// 画蛇身
	for (i=1; i<=n; i++)
	{
		for (j=1; j<=m; j++)
		{
			if (result_map[i][j]>0)
			{
				int tmp = (result_map[i][j]+1)%2;
				int now = calcColor(round[tmp],len[tmp],result_map[i][j]);
				if (now<2 || now>=4)
					continue;
				x = (i*2+1)*BlockSize;// - dx[(result_map-1)%4]*CellSize;
				y = (j*2+1)*BlockSize;// - dy[(result_map-1)%4]*CellSize;
				for (k=0; k<4; k++)
				{
					xx = x + dx[k];
					yy = y + dy[k];
					if (result_map[i+dx[k]][j+dy[k]]==result_map[i][j]+2)
					{
						xx = ((i+dx[k])*2+1)*BlockSize;
						yy = ((j+dy[k])*2+1)*BlockSize;
						cvLine(img,cvPoint(x,y),cvPoint(xx,yy),colors[now],BlockSize);
						break;
					}
				}
				if (k>=4)
				{
					int tmp = (result_map[i][j]+1)%2;
					cvCircle(img,cvPoint(x,y), CellSize, colors[now], 5);
				}
			}
		}
	}

	// 画超出蛇头部分
	for (i=1; i<=n; i++)
	{
		for (j=1; j<=m; j++)
		{
			if (result_map[i][j]>0)
			{
				int tmp = (result_map[i][j]+1)%2;
				int now = calcColor(round[tmp],len[tmp],result_map[i][j]);
				if (now<4)
					continue;
				x = (i*2+1)*BlockSize;// - dx[(result_map-1)%4]*CellSize;
				y = (j*2+1)*BlockSize;// - dy[(result_map-1)%4]*CellSize;
				for (k=0; k<4; k++)
				{
					xx = x + dx[k];
					yy = y + dy[k];
					if (result_map[i+dx[k]][j+dy[k]]==result_map[i][j]+2)
					{
						xx = ((i+dx[k])*2+1)*BlockSize;
						yy = ((j+dy[k])*2+1)*BlockSize;
						cvLine(img,cvPoint(x,y),cvPoint(xx,yy),colors[now],BlockSize);
						break;
					}
				}
				if (k>=4)
				{
					int tmp = (result_map[i][j]+1)%2;
					cvCircle(img,cvPoint(x,y), CellSize, colors[now], 5);
				}
			}
		}
	}

	cvShowImage(WindowName,img);
	char key = cvWaitKey(waitTime);
	if (key>='0' && key<='9')
	{
		waitTime = (key-'0')*500;
	}
	cvReleaseImage(&img);
	return true;
}