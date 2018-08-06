

//TODO expectMax 제대로 계산 못함

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

#define DIR_R 22   //x+ y0
#define DIR_L 55   //x- y0
#define DIR_U 39   //x0 y-
#define DIR_D 21   //x0 y+
#define DIR_RU 26   //x+ y-
#define DIR_LD 35   //x- y+
#define DIR_RD 14   //x+ y+
#define DIR_LU 65   //x- y-

#define FIND_FINISH 1

FILE* ScanLog;
FILE* BoardLog;
FILE* EvaLog;

typedef struct expect
{
	int expectMax = 0;
	int expectQuan = 0;
	int point[2] = { 0 };
	int expectBoard[19][19] = { 0 };
} expect;


char info[] = { "TeamName:1234567890,Department:AI부서[C]" };

void LogBoard(FILE* logFP, int valueBoard[][19])
{


	fprintf(logFP, "\n-----------Board Loaded---------\n");

	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			fprintf(logFP, "\t%d", valueBoard[y][x]);
		}
		fprintf(logFP, "\n");
	}
}
void InitLog()
{
	EvaLog = fopen("./EvaLog.txt", "a");
	ScanLog = fopen("./ScanLog.txt", "a");
	BoardLog = fopen("./BoardLog.txt", "a");
}

void TermLog()
{
	fclose(EvaLog);
	fclose(ScanLog);
	fclose(BoardLog);
}

boolean outOfBound(int x, int y)
{
	fprintf(ScanLog, "Out of Bound! : ( %d , %d ) \n", x, y);
	return (x < 0 || x>18 || y < 0 || y>18);
}

boolean isValidP(int x, int y, int valueBoard[][19])
{
	return !outOfBound(x, y) && (valueBoard[x][y] >= 0);
}

int Scan(int x, int y, int dx, int dy, int valueBoard[][19], int exceptStone)
{
	int count = 0;
	int heading = 0;
	fprintf(ScanLog, "Standard Point : ( %d , %d ) \n", x , y );
	for (int i = 1; i < 6; i++)
	{
		if (valueBoard[x + i * dx][y + i * dy] != exceptStone && valueBoard[x + i * dx][y + i * dy] < 0)
		{
			count++;
			fprintf(ScanLog, "Checked on with i : %d ! : ( %d , %d ) \n",i, x + i * dx, y + i * dy);
			
		}

		else if (valueBoard[x + i * dx][y + i * dy] == exceptStone)
		{
			count = 0;
			
		}
	}
	if ( outOfBound(x + 5 * dx, y + 5 * dy))
	{

		count = 0;
	}
	return count;
}

int ScanFinish(int x, int y, int valueBoard[][19], int exceptStone)
{


	//우측
	int countR = Scan(x, y, 1, 0, valueBoard, exceptStone);
	//좌측
	int countL = Scan(x, y, -1, 0, valueBoard, exceptStone);
	//상측
	int countU = Scan(x, y, 0, -1, valueBoard, exceptStone);
	//하측
	int countD = Scan(x, y, 0, 1, valueBoard, exceptStone);
	//우상측
	int countRU = Scan(x, y, 1, -1, valueBoard, exceptStone);
	//우하측
	int countRD = Scan(x, y, 1, 1, valueBoard, exceptStone);
	//좌상측
	int countLU = Scan(x, y, -1, -1, valueBoard, exceptStone);
	//좌하측
	int countLD = Scan(x, y, -1, 1, valueBoard, exceptStone);

	if (countR >= 3)
	{
		return DIR_R;
	}
	if (countL >= 3)
	{
		return DIR_L;
	}
	if (countU >= 3)
	{
		return DIR_U;
	}
	if (countD >= 3)
	{
		return DIR_D;
	}
	if (countRU >= 3)
	{
		return DIR_RU;
	}
	if (countRD >= 3)
	{
		return DIR_RD;
	}
	if (countLU >= 3)
	{
		return DIR_LU;
	}
	if (countLD >= 3)
	{
		return DIR_LD;
	}

	return 0;
}

int ScanEmpty(int x, int y, int dx, int dy, int valueBoard[][19])
{
	//TODO 나중에는 두 점 이상이 비어있는 것으로 나올때 둘중 가치가 더 큰 점을 활용하도록

	int toReturn = 0;
	for (int i = 1; i < 6; i++)
	{
		if (valueBoard[x + i * dx][y + i * dy] >= 0 && isFree(x+i*dx,y+i*dy))
		{
			fprintf(ScanLog, "( %d , %d ) is %d \n", x + i * dx, y + i * dy,valueBoard[x+i*dx][y+i*dy]);
			toReturn = i;
			break;
		}
	}

	return toReturn;
}

void AddValue(int x, int y, int valueBoard[][19])
{
	boolean rAlive = true;
	boolean lAlive = true;
	boolean uAlive = true;
	boolean dAlive = true;
	boolean ruAlive = true;
	boolean rdAlive = true;
	boolean luAlive = true;
	boolean ldAlive = true;


	for (int i = 1; i < 6; i++)
	{
		int rx = x + i;
		int lx = x - i;
		int uy = y - i;
		int dy = y + i;
		if (isValidP(rx, y, valueBoard) && rAlive)
			valueBoard[rx][y]++;
		else
			rAlive = false;
		if (isValidP(rx, uy, valueBoard) && ruAlive)
			valueBoard[rx][uy]++;
		else
			ruAlive = false;
		if (isValidP(rx, dy, valueBoard) && rdAlive)
			valueBoard[rx][dy]++;
		else
			rdAlive = false;
		if (isValidP(x, uy, valueBoard) && uAlive)
			valueBoard[x][uy]++;
		else
			uAlive = false;
		if (isValidP(x, dy, valueBoard) && dAlive)
			valueBoard[x][dy]++;
		else
			dAlive = false;
		if (isValidP(lx, y, valueBoard) && lAlive)
			valueBoard[lx][y]++;
		else
			lAlive = false;
		if (isValidP(lx, dy, valueBoard) && ldAlive)
			valueBoard[lx][dy]++;
		else
			ldAlive = false;
		if (isValidP(lx, uy, valueBoard) && luAlive)
			valueBoard[lx][uy]++;
		else
			luAlive = false;
	}
}

void InitValue(int valueBoard[][19])
{

	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (showBoard(x, y) == 1)
				valueBoard[x][y] = MY_STONE;
			else if (showBoard(x, y) == 2)
				valueBoard[x][y] = OPPO_STONE;
			else if (showBoard(x, y) == 3)
				valueBoard[x][y] = BLOCK_STONE;
			else
				valueBoard[x][y] = 0;
		}
	}
}
void ValueSet(int valueBoard[][19])
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] == MY_STONE || valueBoard[x][y] == BLOCK_STONE)
			{
				//점수를 분배한다.
				AddValue(x, y, valueBoard);
			}
		}
	}

}

int EvaluateFinish(int point[2], int valueBoard[][19], int exceptStone)
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] < 0 && valueBoard[x][y] != exceptStone)
			{
				int dir = ScanFinish(x, y, valueBoard, exceptStone);

				if (dir == 0)
					continue;
				else
				{
					int distance;
					int dx;
					int dy;

					if (dir % ELE_R == 0)
						dx = 1;
					else if (dir%ELE_L == 0)
						dx = -1;
					else
						dx = 0;

					if (dir % ELE_U == 0)
						dy = -1;
					else if (dir % ELE_D == 0)
						dy = 1;
					else
						dy = 0;

					distance = ScanEmpty(x, y, dx, dy, valueBoard);
					if (isValidP(x + distance * dx, y + distance * dy, valueBoard))
					{
						point[0] = x + (dx * distance);
						point[1] = y + (dy * distance);

						return FIND_FINISH;
					}
				}
			}
		}
	}
}


int GetMV(int valueBoard[][19])
{
	int maximum = 0;
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			int target = valueBoard[x][y];
			if (target > maximum)
				maximum = target;
		}
	}
	return maximum;
}

int GetMQ(int maximum, int valueBoard[][19])
{
	int quantity = 0;
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			int target = valueBoard[x][y];
			if (target == maximum)
				quantity++;

			
		}
	}

	return quantity;
}

void Evaluate(int point[2], int valueBoard[][19])
{
	expect* lineUp;
	int MV = 0;
	int MQ = 0;
	int deployed = 0;
	MV = GetMV(valueBoard);
	MQ = GetMQ(MV, valueBoard);
	int maxPair[2] = { 0 };
	//여기까지 이상 없음
	
	lineUp = (expect*)malloc(sizeof(expect)*MQ);
	fprintf(EvaLog, "\n\n----------------Expectation start----------------\n\n");
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			int target = valueBoard[x][y];
			if (target == MV)
			{				

				
				lineUp[deployed].point[0] = x;
				lineUp[deployed].point[1] = y;
				InitValue(lineUp[deployed].expectBoard);
				lineUp[deployed].expectBoard[x][y] = MY_STONE; //여기 중요!!
				ValueSet(lineUp[deployed].expectBoard);
				lineUp[deployed].expectMax = GetMV(lineUp[deployed].expectBoard);
				lineUp[deployed].expectQuan = GetMQ(lineUp[deployed].expectMax, lineUp[deployed].expectBoard);

				fprintf(EvaLog, "( %d , %d )\nexpectMax = %d\nexpectQuan = %d\n\n", x, y,
					lineUp[deployed].expectMax, lineUp[deployed].expectQuan);
				if (maxPair[0] < lineUp[deployed].expectMax)
				{
					maxPair[0] = lineUp[deployed].expectMax;
					maxPair[1] = lineUp[deployed].expectQuan;
					point[0] = x;
					point[1] = y;
									
				}
				else if (maxPair[0] == lineUp[deployed].expectMax)
				{
					
					if (maxPair[1] < lineUp[deployed].expectQuan)
					{
						maxPair[1] = lineUp[deployed].expectQuan;
						point[0] = x;
						point[1] = y;
					}
				}

				deployed++;

			}
			if (deployed >= MQ)
				break;
		}

		if (deployed >= MQ)
			break;
	}

	



	free(lineUp);


}



void myturn(int cnt)
{


	int x[2], y[2];
	int myValue[19][19];
	int oppoValue[19][19];
	int finalValue[19][19];
	int myValue2[19][19];
	int oppoValue2[19][19];
	int firstPoint[2];
	int secondPoint[2];
	boolean firstOppoFin = false;
	boolean firstMyFin = false;

	InitLog();
	if (!terminateAI)
	{
		InitValue(myValue);
		ValueSet(myValue);
		LogBoard(BoardLog, myValue);
		/*
		srand((unsigned)time(NULL));
		for (int i = 0; i < cnt; i++) {
			do {
				x[i] = rand() % width;
				y[i] = rand() % height;
				if (terminateAI) return;
			} while (!isFree(x[i], y[i]));
			if (x[1] == x[0] && y[1] == y[0]) i--;
		}*/
		firstMyFin = EvaluateFinish(firstPoint, myValue, OPPO_STONE) == FIND_FINISH;
		firstOppoFin = EvaluateFinish(firstPoint, myValue, MY_STONE) == FIND_FINISH;
		if (firstMyFin)
		{

			x[0] = firstPoint[0];
			y[0] = firstPoint[1];

		}
		else if (firstOppoFin)
		{
			x[0] = firstPoint[0];
			y[0] = firstPoint[1];
		}
		else
		{

			Evaluate(firstPoint, myValue);
			x[0] = firstPoint[0];
			y[0] = firstPoint[1];
		}
		
		
		for (int x = 0; x < 19; x++)
		{
			for (int y = 0; y < 19; y++)
			{
				if (myValue[x][y] < 0)
					myValue2[x][y] = myValue[x][y];
				else
					myValue2[x][y] = 0;
			}
		}

	

		myValue2[firstPoint[0]][firstPoint[1]] = -1;

		ValueSet(myValue2);
		
		/*
		else if (firstMyFin)
			myValue2[firstPoint[0]][firstPoint[1]] = -2;
			*/


		if (EvaluateFinish(secondPoint, myValue2, OPPO_STONE) == FIND_FINISH)
		{
			x[1] = secondPoint[0];
			y[1] = secondPoint[1];
		}
		else if (EvaluateFinish(secondPoint, myValue2, MY_STONE) == FIND_FINISH)
		{
			x[1] = secondPoint[0];
			y[1] = secondPoint[1];
		}
		else
		{
			Evaluate(secondPoint, myValue2);
			x[1] = secondPoint[0];
			y[1] = secondPoint[1];
		}

		for (int x = 0; x < 19; x++)
		{
			for (int y = 0; y < 19; y++)
			{
				if (myValue2[x][y] < 0)
					finalValue[x][y] = myValue2[x][y];
				else
					finalValue[x][y] = 0;
			}
		}
		finalValue[secondPoint[0]][secondPoint[1]] = -1;

		ValueSet(finalValue);


		fprintf(BoardLog, "\n\n-------------------myValue1--------------\n\n");
		LogBoard(BoardLog, myValue);

		fprintf(BoardLog, "\n\n-------------------myValue2--------------\n\n");
		LogBoard(BoardLog, myValue2);
		fprintf(BoardLog, "\n\n-------------------finalValue2--------------\n\n");
		LogBoard(BoardLog, finalValue);





		TermLog();

		// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
		// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다



		// 이 부분에서 자신이 놓을 돌을 출력하십시오.
		// 필수 함수 : domymove(x배열,y배열,배열크기)
		// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.


		domymove(x, y, cnt);
	}
}