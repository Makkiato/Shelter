

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



char info[] = { "TeamName:1234567890,Department:AI�μ�[C]" };


void InitLog()
{
	ScanLog = fopen("./ScanLog.txt", "a");
}

void TermLog()
{
	fclose(ScanLog);
}

boolean outOfBound(int x, int y)
{
	fprintf(ScanLog, "Out of Bound! : ( %d , %d ) \n", x, y);
	return (x < 0 || x>18 || y < 0 || y>18);
}

boolean isValidP(int x, int y, int valueBoard[][19])
{
	return isFree(x, y) && !outOfBound(x,y);
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


	//����
	int countR = Scan(x, y, 1, 0, valueBoard, exceptStone);
	//����
	int countL = Scan(x, y, -1, 0, valueBoard, exceptStone);
	//����
	int countU = Scan(x, y, 0, -1, valueBoard, exceptStone);
	//����
	int countD = Scan(x, y, 0, 1, valueBoard, exceptStone);
	//�����
	int countRU = Scan(x, y, 1, -1, valueBoard, exceptStone);
	//������
	int countRD = Scan(x, y, 1, 1, valueBoard, exceptStone);
	//�»���
	int countLU = Scan(x, y, -1, -1, valueBoard, exceptStone);
	//������
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
	//TODO ���߿��� �� �� �̻��� ����ִ� ������ ���ö� ���� ��ġ�� �� ū ���� Ȱ���ϵ���

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

void InitValue(int valueBoard[][19], int firstX, int firstY, int stoneValue)
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

void myturn(int cnt)
{


	int x[2], y[2];
	int myValue[19][19];
	int oppoValue[19][19];
	int myValue2[19][19];
	int oppoValue2[19][19];
	int firstPoint[2];
	int secondPoint[2];
	boolean firstOppoFin = false;
	boolean firstMyFin = false;

	InitLog();
	if (!terminateAI)
	{
		InitValue(myValue, 0, 0, 0);

		srand((unsigned)time(NULL));
		for (int i = 0; i < cnt; i++) {
			do {
				x[i] = rand() % width;
				y[i] = rand() % height;
				if (terminateAI) return;
			} while (!isFree(x[i], y[i]));
			if (x[1] == x[0] && y[1] == y[0]) i--;
		}
		firstMyFin = EvaluateFinish(firstPoint, myValue, OPPO_STONE) == FIND_FINISH;
		firstOppoFin = EvaluateFinish(firstPoint, myValue, MY_STONE) == FIND_FINISH;
		if (firstOppoFin)
		{

			x[0] = firstPoint[0];
			y[0] = firstPoint[1];

		}
		else if (firstMyFin)
		{
			x[0] = firstPoint[0];
			y[0] = firstPoint[1];
		}
		

		for (int x = 0; x < 19; x++)
		{
			for (int y = 0; y < 19; y++)
			{
				myValue2[x][y] = myValue[x][y];
			}
		}
		if (firstOppoFin || firstMyFin)
			myValue2[firstPoint[0]][firstPoint[1]] = -1;

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
		TermLog();

		// �� �κп��� �˰��� ���α׷�(AI)�� �ۼ��Ͻʽÿ�. �⺻ ������ �ڵ带 ���� �Ǵ� �����ϰ� ������ �ڵ带 ����Ͻø� �˴ϴ�.
		// ���� Sample code�� AI�� Random���� ���� ���� Algorithm�� �ۼ��Ǿ� �ֽ��ϴ�



		// �� �κп��� �ڽ��� ���� ���� ����Ͻʽÿ�.
		// �ʼ� �Լ� : domymove(x�迭,y�迭,�迭ũ��)
		// ���⼭ �迭ũ��(cnt)�� myturn()�� �Ķ���� cnt�� �״�� �־���մϴ�.


		domymove(x, y, cnt);
	}
}