

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include "Connect6Algo.h"

#define MY_STONE -1
#define OPPO_STONE -2
#define BLOCK_STONE -3
#define ABANDONED -4


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
FILE* ExpMax;
FILE* ValueLog;
FILE* DefenceLog;
FILE* SevenLog;

typedef struct expect
{
	int distance = 0;
	int expectMax = 0;
	int expectQuan = 0;
	int point[2] = { 0 };
	int expectBoard[19][19] = { 0 };
	
} expect;


char info[] = { "TeamName:1234567890,Department:AI부서[C]" };


void CloneBoard(int original[][19], int secondary[][19])
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (original[x][y] < 0)
				secondary[x][y] = original[x][y];
			else
				secondary[x][y] = 0;
		}
	}
}

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
	BoardLog = fopen("./BoardLog.txt", "a");
	SevenLog = fopen("./SevenLog.txt", "a");
	DefenceLog = fopen("./DefenceLog.txt", "a");
	EvaLog = fopen("./EvaLog.txt", "a");
	ScanLog = fopen("./ScanLog.txt", "a");

	ExpMax = fopen("./ExpMax.txt", "a");
	ValueLog = fopen("./ValueLog.txt", "a");

}

void TermLog()
{
	fclose(SevenLog);
	fclose(DefenceLog);
	fclose(ValueLog);
	fclose(EvaLog);
	fclose(ScanLog);
	fclose(BoardLog);
	fclose(ExpMax);
}
boolean OutOfBound(int x, int y)
{
	fprintf(ScanLog, "Out of Bound! : ( %d , %d ) \n", x, y);
	return (x < 0 || x>18 || y < 0 || y>18);
}
int Scan(int x, int y, int dx, int dy, int valueBoard[][19], int exceptStone, int howLong)
{
	int count = 0;

	fprintf(ScanLog, "Standard Point : ( %d , %d ) \n", x, y);
	for (int i = 1; i < howLong + 1; i++)
	{
		if (valueBoard[x + i * dx][y + i * dy] != exceptStone && valueBoard[x + i * dx][y + i * dy] < 0)
		{
			count++;
			fprintf(ScanLog, "Checked on with i : %d ! : ( %d , %d ) \n", i, x + i * dx, y + i * dy);

		}

		else if (valueBoard[x + i * dx][y + i * dy] == exceptStone)
		{
			count = 0;
			break;
		}
	}
	if (OutOfBound(x + 5 * dx, y + 5 * dy))
	{

		count = 0;
	}
	return count;
}
int SevenOnlyScan(int x, int y, int dx, int dy, int valueBoard[][19], int exceptStone, int howLong)
{
	int count = 0;

	fprintf(ScanLog, "Standard Point : ( %d , %d ) \n", x, y);
	for (int i = 1; i < howLong + 1; i++)
	{
		if (valueBoard[x + i * dx][y + i * dy] != exceptStone && valueBoard[x + i * dx][y + i * dy] < 0)
		{
			count++;
			fprintf(ScanLog, "Checked on with i : %d ! : ( %d , %d ) \n", i, x + i * dx, y + i * dy);

		}

		else if (valueBoard[x + i * dx][y + i * dy] == exceptStone || valueBoard[x + i * dx][y + i * dy] >= 0)
		{

			break;
		}
	}
	return count;
}

boolean isSevenInRow(int point[2], int valueBoard[][19], int exceptStone)
{
	//우측
	int countR = SevenOnlyScan(point[0], point[1], 1, 0, valueBoard, exceptStone, 6);
	//좌측
	int countL = SevenOnlyScan(point[0], point[1], -1, 0, valueBoard, exceptStone, 6);
	//상측
	int countU = SevenOnlyScan(point[0], point[1], 0, -1, valueBoard, exceptStone, 6);
	//하측
	int countD = SevenOnlyScan(point[0], point[1], 0, 1, valueBoard, exceptStone, 6);
	//우상측
	int countRU = SevenOnlyScan(point[0], point[1], 1, -1, valueBoard, exceptStone, 6);
	//우하측
	int countRD = SevenOnlyScan(point[0], point[1], 1, 1, valueBoard, exceptStone, 6);
	//좌상측
	int countLU = SevenOnlyScan(point[0], point[1], -1, -1, valueBoard, exceptStone, 6);
	//좌하측
	int countLD = SevenOnlyScan(point[0], point[1], -1, 1, valueBoard, exceptStone, 6);

	int countHor = countL + countR;
	int countVer = countU + countD;
	int countDia1 = countLU + countRD;
	int countDia2 = countLD + countRU;
	fprintf(SevenLog, "\nAT ( %d , %d ) Hor : %d , Ver : %d , Dia1 : %d , Dia2 : %d\n\n", point[0], point[1], countHor, countVer, countDia1, countDia2);

	if (countHor > 5 || countVer > 5 || countDia1 > 5 || countDia2 > 5)
	{
		return true;
	}
	else
		return false;
}



boolean isSequence(int x, int y, int valueBoard[][19])
{
	return ((valueBoard[x][y] >= 0) || valueBoard[x][y] == -1 || valueBoard[x][y] == -3) && !OutOfBound(x, y);
}

boolean isValidP(int x, int y, int valueBoard[][19])
{
	return !OutOfBound(x, y) && (valueBoard[x][y] >= 0);
}



int ScanFinish(int x, int y, int valueBoard[][19], int exceptStone)
{


	//우측
	int countR = Scan(x, y, 1, 0, valueBoard, exceptStone, 5);
	//좌측
	int countL = Scan(x, y, -1, 0, valueBoard, exceptStone, 5);
	//상측
	int countU = Scan(x, y, 0, -1, valueBoard, exceptStone, 5);
	//하측
	int countD = Scan(x, y, 0, 1, valueBoard, exceptStone, 5);
	//우상측
	int countRU = Scan(x, y, 1, -1, valueBoard, exceptStone, 5);
	//우하측
	int countRD = Scan(x, y, 1, 1, valueBoard, exceptStone, 5);
	//좌상측
	int countLU = Scan(x, y, -1, -1, valueBoard, exceptStone, 5);
	//좌하측
	int countLD = Scan(x, y, -1, 1, valueBoard, exceptStone, 5);

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
		if (valueBoard[x + i * dx][y + i * dy] >= 0 && isFree(x + i * dx, y + i * dy))
		{
			fprintf(ScanLog, "( %d , %d ) is %d \n", x + i * dx, y + i * dy, valueBoard[x + i * dx][y + i * dy]);
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
		if (isSequence(rx, y, valueBoard) && rAlive)
		{
			if (valueBoard[rx][y] >= 0 && valueBoard[rx][y] < 4)
				valueBoard[rx][y]++;
		}
		else
			rAlive = false;
		if (isSequence(rx, uy, valueBoard) && ruAlive)
		{
			if (valueBoard[rx][uy] >= 0 && valueBoard[rx][uy] < 4)
				valueBoard[rx][uy]++;
		}
		else
			ruAlive = false;
		if (isSequence(rx, dy, valueBoard) && rdAlive)
		{
			if (valueBoard[rx][dy] >= 0 && valueBoard[rx][dy] < 4)
				valueBoard[rx][dy]++;
		}
		else
			rdAlive = false;
		if (isSequence(x, uy, valueBoard) && uAlive)
		{
			if (valueBoard[x][uy] >= 0 && valueBoard[x][uy] < 4)
				valueBoard[x][uy]++;
		}
		else
			uAlive = false;
		if (isSequence(x, dy, valueBoard) && dAlive)
		{
			if (valueBoard[x][dy] >= 0 && valueBoard[x][dy] < 4)
				valueBoard[x][dy]++;
		}
		else
			dAlive = false;
		if (isSequence(lx, y, valueBoard) && lAlive)
		{
			if (valueBoard[lx][y] >= 0 && valueBoard[lx][y] < 4)
				valueBoard[lx][y]++;
		}
		else
			lAlive = false;
		if (isSequence(lx, dy, valueBoard) && ldAlive)
		{
			if (valueBoard[lx][dy] >= 0 && valueBoard[lx][dy] < 4)
				valueBoard[lx][dy]++;
		}
		else
			ldAlive = false;
		if (isSequence(lx, uy, valueBoard) && luAlive)
		{
			if (valueBoard[lx][uy] >= 0 && valueBoard[lx][uy] < 4)
				valueBoard[lx][uy]++;
		}
		else
			luAlive = false;
	}
}

int InitValue(int valueBoard[][19])
{
	int myNum = 0;

	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (showBoard(x, y) == 1)
			{
				myNum++;
				valueBoard[x][y] = MY_STONE;
			}
			else if (showBoard(x, y) == 2)
				valueBoard[x][y] = OPPO_STONE;
			else if (showBoard(x, y) == 3)
			{
				valueBoard[x][y] = BLOCK_STONE;
			}
			else
				valueBoard[x][y] = 0;
		}
	}

	return myNum;
}
void ValueSet(int valueBoard[][19])
{
	fprintf(ValueLog, "\nnew Value Setter\n");


	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] == MY_STONE || valueBoard[x][y] == BLOCK_STONE)
			{
				//점수를 분배한다.
				fprintf(ValueLog, "( %d , %d ) is Value Spreader\n", x, y);
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

					fprintf(DefenceLog, " point : ( %d , %d )\ndirection : ( %d , %d )\n", x, y, dx, dy);
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

int GetDistance(int x, int y, int toX, int toY)
{
	return (abs(x - toX)+abs(y - toY));
}

void Evaluate(int point[2], int valueBoard[][19])
{
	expect* lineUp;
	int MV = 0;
	int MQ = 0;
	int deployed = 0;
	MV = GetMV(valueBoard);
	MQ = GetMQ(MV, valueBoard);
	if (MV > 3)
		MQ += GetMQ(MV - 1, valueBoard);
	int maxPair[3] = { 0 };
	int iterMin = 0;
	int iterMax = 19;
	maxPair[2] = 100;
	if (MV == 0 && MQ > 81)
	{
		//내가 놓는 첫번째 돌이면.
		iterMin = 5;
		iterMax = 14;
		MQ = 81;
	}
	lineUp = (expect*)malloc(sizeof(expect)*MQ);
	fprintf(EvaLog, "\n\n----------------Expectation start----------------\n\n");
	fprintf(ExpMax, "\n\n-------------original Board-------------------\n\n");
	LogBoard(ExpMax, valueBoard);
	for (int x = iterMin; x < iterMax; x++)
	{
		for (int y = iterMin; y < iterMax; y++)
		{
			int target = valueBoard[x][y];
			if (target == MV || (target == MV - 1 && MV > 3))
			{


				lineUp[deployed].point[0] = x;
				lineUp[deployed].point[1] = y;
				lineUp[deployed].distance = GetDistance(x, y, 9, 9);
				CloneBoard(valueBoard, lineUp[deployed].expectBoard);
				lineUp[deployed].expectBoard[x][y] = MY_STONE; //여기 중요!!
				ValueSet(lineUp[deployed].expectBoard);
				fprintf(ExpMax, "\n\n-------------expectBoard of deployed = %d-------------------\n\n", deployed);
				LogBoard(ExpMax, lineUp[deployed].expectBoard);
				lineUp[deployed].expectMax = GetMV(lineUp[deployed].expectBoard);
				lineUp[deployed].expectQuan = GetMQ(lineUp[deployed].expectMax, lineUp[deployed].expectBoard);

				fprintf(ExpMax, "( %d , %d )\nexpectMax = %d\nexpectQuan = %d\n\n", x, y,
					lineUp[deployed].expectMax, lineUp[deployed].expectQuan);

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
					else if (maxPair[1] == lineUp[deployed].expectQuan && maxPair[2] > lineUp[deployed].distance)
					{
						maxPair[2] = lineUp[deployed].distance;
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

boolean isNextSeven(int firstpoint[2], int valueBoard[][19])
{
	return true;
}

void myturn(int cnt)
{


	int x[2], y[2];
	int myValue[19][19] = { 0 };
	int oppoValue[19][19] = { 0 };
	int finalValue[19][19] = { 0 };
	int myValue2[19][19] = { 0 };
	int oppoValue2[19][19] = { 0 };
	int firstPoint[2] = { 0 };
	int secondPoint[2] = { 0 };
	int APoint1[2] = { 0 };
	int APoint2[2] = { 0 };
	int DPoint1[2] = { 0 };
	int DPoint2[2] = { 0 };
	int VPoint1[2] = { 0 };
	int VPoint2[2] = { 0 };

	boolean firstOppoFin = false;
	boolean firstMyFin = false;
	boolean secondOppoFin = false;
	boolean secondMyFin = false;
	boolean seven = false;


	InitLog();

	if (!terminateAI)
	{
		//첫번째 보드 초기화
		InitValue(myValue);
		//점수를 주자
		ValueSet(myValue);

		//당장에 긴급상황 검사를 다 시행하고 저장
		firstMyFin = EvaluateFinish(APoint1, myValue, OPPO_STONE) == FIND_FINISH;
		firstOppoFin = EvaluateFinish(DPoint1, myValue, MY_STONE) == FIND_FINISH;

		//내 단독승리
		if (firstMyFin && !firstOppoFin)
		{
			seven = isSevenInRow(APoint1, myValue, OPPO_STONE);

			if (seven)
				goto PEACE;

			CloneBoard(myValue, myValue2);
			myValue2[APoint1[0]][APoint1[1]] = MY_STONE;
			ValueSet(myValue2);
			secondMyFin = EvaluateFinish(APoint2, myValue2, OPPO_STONE) == FIND_FINISH;
			//내 승리로 끝내는 수가 보이면

			seven = isSevenInRow(APoint2, myValue2, MY_STONE);
			if (seven)
				goto PEACE;

			x[0] = APoint1[0];
			x[1] = APoint2[0];
			y[0] = APoint1[1];
			y[1] = APoint2[1];
		}
		//상대 단독승리
		else if (!firstMyFin && firstOppoFin)
		{
			seven = isSevenInRow(DPoint1, myValue, OPPO_STONE);

			if (seven)
				goto PEACE;

			CloneBoard(myValue, myValue2);
			myValue2[DPoint1[0]][DPoint1[1]] = MY_STONE;
			ValueSet(myValue2);
			secondOppoFin = EvaluateFinish(DPoint2, myValue2, MY_STONE) == FIND_FINISH;
			//상대 단독 승리 상황 유지시....
			if (secondOppoFin)
			{
				seven = isSevenInRow(DPoint2, myValue2, MY_STONE);
				if (seven)
					goto PEACE;

				x[0] = DPoint1[0];
				x[1] = DPoint2[0];
				y[0] = DPoint1[1];
				y[1] = DPoint2[1];
			}
			//앞의 한 수로 방어 성공했으니 하고 싶은데로 해
			else
			{
				do {
					Evaluate(VPoint2, myValue2);
					seven = isSevenInRow(VPoint2, myValue2, OPPO_STONE);
					if (seven)
					{
						myValue2[VPoint2[0]][VPoint2[1]] = 0;
					}
				} while (seven);
				x[0] = DPoint1[0];
				x[1] = VPoint2[0];
				y[0] = DPoint1[1];
				y[1] = VPoint2[1];
			}
		}
		//양측 공동승리
		else if (firstMyFin && firstOppoFin)
		{
			boolean atkMod = true;
			boolean sevenA = false;
			boolean sevenD = false;
			sevenA = isSevenInRow(APoint1, myValue, OPPO_STONE);
			sevenD = isSevenInRow(DPoint1, myValue, OPPO_STONE);
			if (sevenA && sevenD)
				goto PEACE;
			else if (!sevenA)
			{
				atkMod = true;
			}
			else if (!sevenD)
			{
				atkMod == false;
			}

			CloneBoard(myValue, myValue2);

			if (atkMod)
			{
				myValue2[APoint1[0]][APoint1[1]] = MY_STONE;
				ValueSet(myValue);

				secondMyFin = EvaluateFinish(APoint2, myValue2, OPPO_STONE) == FIND_FINISH;;

				seven = isSevenInRow(APoint2, myValue2, OPPO_STONE);
				if (seven)
					goto DEFMOD;

				x[0] = APoint1[0];
				x[1] = APoint2[0];
				y[0] = APoint1[1];
				y[1] = APoint2[1];

			}

			else
			{
			DEFMOD:
				myValue2[DPoint1[0]][DPoint1[1]] = MY_STONE;
				ValueSet(myValue);

				secondOppoFin = EvaluateFinish(DPoint2, myValue2, OPPO_STONE) == FIND_FINISH;

				seven = isSevenInRow(DPoint2, myValue2, OPPO_STONE);
				//방어 후 추가 방어 필요
				if (secondOppoFin && !seven)
				{
					x[0] = DPoint1[0];
					x[1] = DPoint2[0];
					y[0] = DPoint1[1];
					y[1] = DPoint2[1];
				}
				//방어 하고 싶어도 못해
				else if (seven)
				{
					goto PEACE;
				}
				//이미 다 방어 했으니 하고 싶은데 해
				else
				{
					do {
						Evaluate(VPoint2, myValue2);
						seven = isSevenInRow(VPoint2, myValue2, OPPO_STONE);
						if (seven)
						{
							myValue2[VPoint2[0]][VPoint2[1]] = 0;
						}
					} while (seven);

					x[0] = DPoint1[0];
					x[1] = VPoint2[0];
					y[0] = DPoint1[1];
					y[1] = VPoint2[1];
				}
			}

		}
		//평화의 시대
		else
		{

		PEACE:
			do {
				Evaluate(VPoint1, myValue);
				seven = isSevenInRow(VPoint1, myValue, OPPO_STONE);
				if (seven)
				{
					myValue[VPoint1[0]][VPoint1[1]] = 0;
				}
			} while (seven);

			CloneBoard(myValue, myValue2);
			myValue2[VPoint1[0]][VPoint1[1]] = MY_STONE;
			ValueSet(myValue2);
			do {
				Evaluate(VPoint2, myValue2);
				seven = isSevenInRow(VPoint2, myValue2, OPPO_STONE);
				if (seven)
				{
					myValue2[VPoint2[0]][VPoint2[1]] = 0;
				}
			} while (seven);

			x[0] = VPoint1[0];
			x[1] = VPoint2[0];
			y[0] = VPoint1[1];
			y[1] = VPoint2[1];

		}
		/*
		긴급상황 검사를 한 점과 그 이후에 둘 두번째 점을 칠목 검사하자.
		우선 검사 순위는
		내승	내승	-> 공격		7목 발생시 아예 제외
		적승	양승	-> 방어		7목 발생시 아예 제외
		적승	적승	-> 방어		7목 발생시 아예 제외
		양승	양승	-> 공격		7목 발생시 방어
		이벨	내승	-> 무시		이벨	이벨로 이동
		이벨	이벨	-> 7목 발생시 재시도
		*/


	}

	fprintf(BoardLog, "\n\n-------------------myValue1--------------\n\n");
	LogBoard(BoardLog, myValue);

	fprintf(BoardLog, "\n\n-------------------myValue2--------------\n\n");
	LogBoard(BoardLog, myValue2);
	CloneBoard(myValue2, finalValue);
	finalValue[x[1]][y[1]] = MY_STONE;
	fprintf(BoardLog, "\n\n-------------------finalValue--------------\n\n");

	LogBoard(BoardLog, finalValue);


	TermLog();

	// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
	// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다



	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.


	domymove(x, y, cnt);
}
