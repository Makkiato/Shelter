

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "Connect6Algo.h"

char info[] = { "TeamName:Lucky,Department:Jason Mraz" };



#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "Connect6Algo.h"


#define MY_STONE -1
#define OPPO_STONE -2
#define BLOCK_STONE -3

#define ELE_R 2
#define ELE_L 5
#define ELE_U 13
#define ELE_D 7
#define ELE_RL0 3
#define ELE_UD0 11

/*
x+ : 2
x0 : 3
x- : 5

y+ : 7
y0 : 11
y- : 13
*/

#define DIR_R 22	//x+ y0
#define DIR_L 55	//x- y0
#define DIR_U 39	//x0 y-
#define DIR_D 21	//x0 y+
#define DIR_RU 26	//x+ y-
#define DIR_LD 35	//x- y+
#define DIR_RD 14	//x+ y+
#define DIR_LU 65	//x- y-

#define TRUE 1
#define FALSE 0




int GetEleByDistance(int x, int dx, int i)
{
	return x + (dx * i);
}

//각 방향 검사 후 리턴
int CheckSingleDir(int x, int dx, int y, int dy, int valueBoard[19][19])
{
	int count = 0;

	for (int i = 1; i < 6; i++)
	{
		if (valueBoard[x + i * dx][y + i * dy] != OPPO_STONE && valueBoard[x + i * dx][y + i * dy] < 0)
		{
			count++;
		}

		else if (valueBoard[x + i * dx][y + i * dy] == OPPO_STONE)
		{
			count = 0;
			break;
		}
	}

	return count;
}


int CheckEmpty(int x, int y, int dx, int dy,int valueBoard[19][19])
{

	for (int i = 1; i < 6; i++)
	{
		if (valueBoard[GetEleByDistance(x,dx,i)][GetEleByDistance(y,dy,i)] >= 0)
			return i;
		
	}
	return 0;
}

//8방향 검사 CheckSingleDir 호출
int CheckAllDir(int x, int y, int valueBoard[19][19])
{
	//fprintf(myCalLog, "At ( %d , %d )\n", x, y);
	//우측
	int countR = CheckSingleDir(x, y, 1, 0, valueBoard);
	//fprintf(myCalLog, "R Finish Checked...\n");
	//좌측
	int countL = CheckSingleDir(x, y, -1, 0, valueBoard);
	//fprintf(myCalLog, "L Finish Checked...\n");
	//상측
	int countU = CheckSingleDir(x, y, 0, -1, valueBoard);
	//fprintf(myCalLog, "U Finish Checked...\n");
	//하측
	int countD = CheckSingleDir(x, y, 0, 1, valueBoard);
	//fprintf(myCalLog, "D Finish Checked...\n");
	//우상측
	int countRU = CheckSingleDir(x, y, 1, -1, valueBoard);
	//fprintf(myCalLog, "RU Finish Checked...\n");
	//우하측
	int countRD = CheckSingleDir(x, y, 1, 1, valueBoard);
	//fprintf(myCalLog, "RD Finish Checked...\n");
	//좌상측
	int countLU = CheckSingleDir(x, y, -1, -1, valueBoard);
	//fprintf(myCalLog, "LU Finish Checked...\n");
	//좌하측
	int countLD = CheckSingleDir(x, y, -1, 1, valueBoard);
	//fprintf(myCalLog, "LD Finish Checked...\n");

	if (countR >= 3)
	{
		//fprintf(myCalLog, "R has problem...\n");
		return DIR_R;
	}
	if (countL >= 3)
	{
		//fprintf(myCalLog, "L has problem...\n");
		return DIR_L;
	}
	if (countU >= 3)
	{
		//fprintf(myCalLog, "U has problem...\n");
		return DIR_U;
	}
	if (countD >= 3)
	{
		//fprintf(myCalLog, "D has problem...\n");
		return DIR_D;
	}
	if (countRU >= 3)
	{
		//fprintf(myCalLog, "RU has problem...\n");
		return DIR_RU;
	}
	if (countRD >= 3)
	{
		//fprintf(myCalLog, "RD has problem...\n");
		return DIR_RD;
	}
	if (countLU >= 3)
	{
		//fprintf(myCalLog, "LU has problem...\n");
		return DIR_LU;
	}
	if (countLD >= 3)
	{
		//fprintf(myCalLog, "LD has problem...\n");
		return DIR_LD;
	}

	return 0;
}


//육목검사 CheckAlldir 호출
void FindFinish(int point[2], int valueBoard[19][19])
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] < 0 && valueBoard[x][y] != OPPO_STONE)
			{
				int dir = CheckAllDir(x, y, valueBoard);
				int dx = 0;
				int dy = 0;
				int dis = 0;

				if (dir)
				{
					if (dir % ELE_R == 0)
						dx = 1;
					else if (dir % ELE_L == 0)
						dx = -1;
					else
						dx = 0;

					if (dir % ELE_U == 0)
						dy = -1;
					else if (dir % ELE_D == 0)
						dy = +1;
					else
						dy = 0;


					dis = CheckEmpty(x, y, dx, dy, valueBoard);
					
					point[0] = GetEleByDistance(x, dx, dis);
					point[1] = GetEleByDistance(y, dy, dis);

				}
			}
		}
	}
}

//최초 계산용 보드 초기화
void InitBoard(int valueBoard[19][19])
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (showBoard(x, y) == 1)
			{
				valueBoard[x][y] == MY_STONE;
			}
			else if (showBoard(x, y) == 2)
			{
				valueBoard[x][y] == OPPO_STONE;
			}
			else if (showBoard(x, y) == 3)
			{
				valueBoard[x][y] == BLOCK_STONE;
			}
			else
			{
				valueBoard[x][y] == 0;
			}
			
		}
	}

}


void CloneBoard(int original[19][19], int brandNew[19][19])
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (original[x][y] < 0)
			{
				brandNew[x][y] = original[x][y];
			}
			else
				brandNew[x][y] = 0;
		}
	}
}

void myturn(int cnt) {

	int x[2], y[2];
	int valueBoard1[19][19] = { 0 };
	int valueBoard2[19][19] = { 0 };
	int point1[2] = { 0 };
	int point2[2] = { 0 };

	InitBoard(valueBoard1);
	FindFinish(point1, valueBoard1);
	CloneBoard(valueBoard1, valueBoard2);
	if (!(point1[0] == 0 && point1[1] == 0))
	{
		valueBoard2[point1[0]][point1[1]] = MY_STONE;
	}
	FindFinish(point2, valueBoard2);



	srand((unsigned)time(NULL));

	for (int i = 0; i < cnt; i++) {
		do {
			x[i] = rand() % width;
			y[i] = rand() % height;
			if (terminateAI) return;
		} while (!isFree(x[i], y[i]));

		if (x[1] == x[0] && y[1] == y[0]) i--;
	}

	domymove(x, y, cnt);
}