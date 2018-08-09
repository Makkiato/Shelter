

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include "Connect6Algo.h"
#define _CRT_SECURE_NO_WARNINGS
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
#define CONNECT_BONUS 3

#define FINDMYFINISH 100
#define FINDOPPOFINISH 200
#define FINDNOFINISH 300

FILE* ConnectLog;
FILE* ScanLog;
FILE* BoardLog;
FILE* EvaLog;
FILE* ExpMax;
FILE* ValueLog;
FILE* DefenceLog;
FILE* SevenLog;
FILE* PathLog;
FILE* JunkLog;
FILE* RecurseLog;

typedef struct expect
{
	int connect = 0;
	int distance = 0;
	int expectMax = 0;
	int expectQuan = 0;
	int subQ = 0;
	int point[2] = { 0 };
	int expectBoard[19][19] = { 0 };

} expect;

typedef struct expectRecurse
{
	int x[2] = { -1 };
	int y[2] = { -1 };
	int expectBoard[19][19];
	int finishScan = 0;
	boolean myWin = false;
	boolean finish = false;
} expectRecurse;

char info[] = { "TeamName:1234567890,Department:AI부서[C]" };

int GetOtherStone(int stone)
{
	if (stone == MY_STONE)
	{
		return OPPO_STONE;
	}
	else if (stone == OPPO_STONE)
	{
		return MY_STONE;
	}
	else if (stone == BLOCK_STONE)
	{
		return BLOCK_STONE;
	}
	else return 0;
}
void InitLog()
{
	RecurseLog = fopen("./RecurseLog.txt", "a");
	ConnectLog = fopen("./ConnectLog.txt", "a");
	ExpMax = fopen("./ExpMax.txt", "a");
	BoardLog = fopen("./BoardLog.txt", "a");
	SevenLog = fopen("./SevenLog.txt", "a");
	DefenceLog = fopen("./DefenceLog.txt", "a");
	EvaLog = fopen("./EvaLog.txt", "a");
	ScanLog = fopen("./ScanLog.txt", "a");
	PathLog = fopen("./PathLog.txt", "a");
	JunkLog = fopen("./JunkLog.txt", "a");
	ValueLog = fopen("./ValueLog.txt", "a");

}

void TermLog()
{
	fclose(RecurseLog);
	fclose(JunkLog);
	fclose(PathLog);
	fclose(ConnectLog);
	fclose(SevenLog);
	fclose(DefenceLog);
	fclose(ValueLog);
	fclose(EvaLog);
	fclose(ScanLog);
	fclose(BoardLog);
	fclose(ExpMax);
}

void ReverseCloneBoard(int original[][19], int secondary[][19])
{
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{

			switch (original[x][y])
			{
			case MY_STONE:
				secondary[x][y] = OPPO_STONE;
				break;
			case OPPO_STONE:
				secondary[x][y] = MY_STONE;
				break;
			default:
				secondary[x][y] = original[x][y];

			}
		}
	}
}

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
boolean IsAbandoned(int x, int y, int xAb[2], int yAb[2])
{
	boolean toReturn = false;
	for (int i = 0; i < 2; i++)
	{
		if (xAb[i] == x && yAb[i] == y)
			toReturn = true;
	}
	return toReturn;
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



boolean isSequence(int x, int y, int valueBoard[][19], int whatIsMine)
{
	return ((valueBoard[x][y] >= 0) || (valueBoard[x][y] == whatIsMine) || (valueBoard[x][y] == BLOCK_STONE))
		&& !OutOfBound(x, y);
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

	if (toReturn == 4)
	{
		if (valueBoard[x - 1 * dx][y - 1 * dy] == MY_STONE)
		{
			if (valueBoard[x + 4 * dx][y + 4 * dy] < valueBoard[x + 5 * dx][y + 5 * dy])
				toReturn = 5;
			else
				toReturn = 4;
		}
	}

	return toReturn;
}
void AddValue(int x, int y, int valueBoard[][19], int whatIsMine)
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
		if (isSequence(rx, y, valueBoard, whatIsMine) && rAlive)
		{
			if (valueBoard[rx][y] >= 0 && valueBoard[rx][y] < 4)
				valueBoard[rx][y]++;
		}
		else
			rAlive = false;
		if (isSequence(rx, uy, valueBoard, whatIsMine) && ruAlive)
		{
			if (valueBoard[rx][uy] >= 0 && valueBoard[rx][uy] < 4)
				valueBoard[rx][uy]++;
		}
		else
			ruAlive = false;
		if (isSequence(rx, dy, valueBoard, whatIsMine) && rdAlive)
		{
			if (valueBoard[rx][dy] >= 0 && valueBoard[rx][dy] < 4)
				valueBoard[rx][dy]++;
		}
		else
			rdAlive = false;
		if (isSequence(x, uy, valueBoard, whatIsMine) && uAlive)
		{
			if (valueBoard[x][uy] >= 0 && valueBoard[x][uy] < 4)
				valueBoard[x][uy]++;
		}
		else
			uAlive = false;
		if (isSequence(x, dy, valueBoard, whatIsMine) && dAlive)
		{
			if (valueBoard[x][dy] >= 0 && valueBoard[x][dy] < 4)
				valueBoard[x][dy]++;
		}
		else
			dAlive = false;
		if (isSequence(lx, y, valueBoard, whatIsMine) && lAlive)
		{
			if (valueBoard[lx][y] >= 0 && valueBoard[lx][y] < 4)
				valueBoard[lx][y]++;
		}
		else
			lAlive = false;
		if (isSequence(lx, dy, valueBoard, whatIsMine) && ldAlive)
		{
			if (valueBoard[lx][dy] >= 0 && valueBoard[lx][dy] < 4)
				valueBoard[lx][dy]++;
		}
		else
			ldAlive = false;
		if (isSequence(lx, uy, valueBoard, whatIsMine) && luAlive)
		{
			if (valueBoard[lx][uy] >= 0 && valueBoard[lx][uy] < 4)
				valueBoard[lx][uy]++;
		}
		else
			luAlive = false;
	}
}

int InitValue(int valueBoard[][19],int whatIsMine, int whatIsOppo)
{
	int myNum = 0;

	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (showBoard(x, y) == -whatIsMine)
			{
				myNum++;
				valueBoard[x][y] = MY_STONE;
			}
			else if (showBoard(x, y) == -whatIsOppo)
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
int ScanJunk(int x, int y, int dx, int dy, int valueBoard[][19], int exceptStone, int howLong)
{
	int count = 0;
	boolean atLeastEmpty = true;

	fprintf(ScanLog, "Standard Point : ( %d , %d ) \n", x, y);
	for (int i = 1; i < howLong + 1; i++)

	{
		if (valueBoard[x + i * dx][y + i * dy] == exceptStone || OutOfBound(x + i * dx, y + i * dy))
		{
			atLeastEmpty = false;
			break;
		}
		else
		{
			count++;
			fprintf(ScanLog, "Checked on with i : %d ! : ( %d , %d ) \n", i, x + i * dx, y + i * dy);

			if (valueBoard[x + i * dx][y + i * dy] < 0)
			{
				atLeastEmpty = false;
			}
		}

	}
	if (atLeastEmpty)
	{
		return 10;
	}

	return count;
}


void ValueSet(int valueBoard[][19], int whatIsMine)
{
	fprintf(ValueLog, "\nnew Value Setter\n");
	int whatIsYours = GetOtherStone(whatIsMine);
	int rJunk = 0;
	int lJunk = 0;
	int uJunk = 0;
	int dJunk = 0;
	int ruJunk = 0;
	int rdJunk = 0;
	int luJunk = 0;
	int ldJunk = 0;

	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] == whatIsMine || valueBoard[x][y] == BLOCK_STONE)
			{
				//점수를 분배한다.
				fprintf(ValueLog, "( %d , %d ) is Value Spreader\n", x, y);
				AddValue(x, y, valueBoard, whatIsMine);



			}

		}
	}
	fprintf(BoardLog, "\n---------------- in Value Set ------------------\n");
	LogBoard(BoardLog, valueBoard);
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] > 2)
			{
				boolean horDead = false;
				boolean verDead = false;
				boolean dia1Dead = false;
				boolean dia2Dead = false;


				rJunk = ScanJunk(x, y, 1, 0, valueBoard, whatIsYours, 5);
				lJunk = ScanJunk(x, y, -1, 0, valueBoard, whatIsYours, 5);
				uJunk = ScanJunk(x, y, 0, -1, valueBoard, whatIsYours, 5);
				dJunk = ScanJunk(x, y, 0, 1, valueBoard, whatIsYours, 5);
				ruJunk = ScanJunk(x, y, 1, -1, valueBoard, whatIsYours, 5);
				rdJunk = ScanJunk(x, y, 1, 1, valueBoard, whatIsYours, 5);
				luJunk = ScanJunk(x, y, -1, -1, valueBoard, whatIsYours, 5);
				ldJunk = ScanJunk(x, y, -1, 1, valueBoard, whatIsYours, 5);


				fprintf(JunkLog, "\nJunk of ( %d , %d )\n\t%d\t%d\t%d\n\t%d\t\t%d\n\t%d\t%d\t%d\n", x, y, luJunk, uJunk, ruJunk, lJunk, rJunk, ldJunk, dJunk, rdJunk);

				if ((rJunk + lJunk < 5) || (rJunk == 10 && lJunk == 10))
				{
					horDead = true;
				}
				if ((uJunk + dJunk < 5) || (uJunk == 10 && dJunk == 10))
				{
					verDead = true;
				}
				if ((ruJunk + ldJunk < 5) || (ruJunk == 10 && ldJunk == 10))
				{
					dia1Dead = true;
				}
				if ((rdJunk + luJunk < 5) || (rdJunk == 10 && luJunk == 10))
				{
					dia2Dead = true;
				}

				if (horDead && verDead && dia1Dead && dia2Dead)

					valueBoard[x][y] = 0;

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
					if (isValidP(x + 4 * dx, y + 4 * dy, valueBoard) && valueBoard[x + 5 * dx][y + 5 * dy] == MY_STONE)
					{
						return 0;
					}
					else if (isValidP(x + distance * dx, y + distance * dy, valueBoard))
					{

						point[0] = x + (dx * distance);
						point[1] = y + (dy * distance);

						return FIND_FINISH;
					}
				}
			}
		}
	}
	return 0;
}


int GetMV(int valueBoard[][19],int xAb[2],int yAb[2])
{
	int maximum = 0;
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			int target = valueBoard[x][y];
			if (target > maximum && !IsAbandoned(x,y,xAb,yAb))
				maximum = target;
		}
	}
	return maximum;
}

int GetMQ(int maximum, int valueBoard[][19],int xAb[2], int yAb[2])
{
	int quantity = 0;
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			int target = valueBoard[x][y];
			if (target == maximum && !IsAbandoned(x,y,xAb,yAb))
			{
				quantity++;
				fprintf(EvaLog, "( %d , %d ) is %d and Q is now %d\n", x, y, target, quantity);
			}

		}
	}
	fprintf(EvaLog, "I gonna return : %d", quantity);
	return quantity;
}

int GetDistance(int x, int y, int toX, int toY)
{
	return (abs(x - toX) + abs(y - toY));
}
int GetConnect(expect point, int valueBoard[][19], int whatIsMine)
{

	boolean rAlive = true;
	boolean lAlive = true;
	boolean uAlive = true;
	boolean dAlive = true;
	boolean ruAlive = true;
	boolean rdAlive = true;
	boolean luAlive = true;
	boolean ldAlive = true;
	int x = point.point[0];
	int y = point.point[1];
	point.connect = 0;
	int RC = 0;
	int LC = 0;
	int UC = 0;
	int DC = 0;
	int RUC = 0;
	int RDC = 0;
	int LUC = 0;
	int LDC = 0;
	int Hor = 0;
	int Ver = 0;
	int Dia1 = 0;
	int Dia2 = 0;
	int r = 1;
	int l = 1;
	int ru = 1;
	int rd = 1;
	int u = 1;
	int d = 1;
	int lu = 1;
	int ld = 1;
	int r4 = 0;
	int l4 = 0;
	int u4 = 0;
	int d4 = 0;
	int ru4 = 0;
	int rd4 = 0;
	int lu4 = 0;
	int ld4 = 0;



	for (int i = 1; i < 6; i++)
	{

		int rx = x + i;
		int lx = x - i;
		int uy = y - i;
		int dy = y + i;



		if (isSequence(rx, y, valueBoard, whatIsMine))
		{
			if (valueBoard[rx][y] == MY_STONE || valueBoard[rx][y] == BLOCK_STONE)
			{
				RC += r;
				r4++;
				if (rAlive)
					r += CONNECT_BONUS;
				if (r4 >= 4)
					RC = -10;
			}
			else
			{
				r = 1;
				rAlive = false;
			}
		}
		else
		{
			//RC = 0;
			rAlive = false;
		}
		if (isSequence(rx, uy, valueBoard, whatIsMine))
		{
			if (valueBoard[rx][uy] == MY_STONE || valueBoard[rx][uy] == BLOCK_STONE)
			{
				RUC += ru;
				ru4++;
				if (ruAlive)
					ru += CONNECT_BONUS;
				if (ru4 >= 4)
					RUC = -10;
			}
			else
			{
				ru = 1;
				ruAlive = false;
			}
		}
		else
		{
			//RUC = 0;
			ruAlive = false;
		}
		if (isSequence(rx, dy, valueBoard, whatIsMine))
		{
			if (valueBoard[rx][dy] == MY_STONE || valueBoard[rx][dy] == BLOCK_STONE)
			{
				RDC += rd;
				rd4++;
				if (rdAlive)
					rd += CONNECT_BONUS;
				if (rd4 >= 4)
					RDC = -10;
			}
			else
			{
				rd = 1;
				rdAlive = false;
			}
		}
		else
		{
			//RDC = 0;
			rdAlive = false;
		}
		if (isSequence(x, uy, valueBoard, whatIsMine))
		{
			if (valueBoard[x][uy] == MY_STONE || valueBoard[x][uy] == BLOCK_STONE)
			{
				UC += u;
				u4++;
				if (uAlive)
					u += CONNECT_BONUS;
				if (u4 >= 4)
					UC = -10;
			}
			else
			{
				u = 1;
				uAlive = false;
			}
		}
		else
		{
			//UC = 0;
			uAlive = false;
		}
		if (isSequence(x, dy, valueBoard, whatIsMine))
		{
			if (valueBoard[x][dy] == MY_STONE || valueBoard[x][dy] == BLOCK_STONE)
			{
				DC += d;
				d4++;
				if (dAlive)
					d += CONNECT_BONUS;
				if (d4 >= 4)
					DC = -10;
			}
			else
			{
				d = 1;
				dAlive = false;
			}
		}
		else
		{
			//DC = 0;
			dAlive = false;
		}
		if (isSequence(lx, y, valueBoard, whatIsMine))
		{
			if (valueBoard[lx][y] == MY_STONE || valueBoard[lx][y] == BLOCK_STONE)
			{
				LC += l;
				l4++;
				if (lAlive)
					l += CONNECT_BONUS;
				if (l4 >= 4)
					LC = -10;
			}
			else
			{
				l = 1;
				lAlive = false;
			}
		}
		else
		{
			//LC = 0;
			lAlive = false;
		}
		if (isSequence(lx, dy, valueBoard, whatIsMine))
		{
			if (valueBoard[lx][dy] == MY_STONE || valueBoard[lx][dy] == BLOCK_STONE)
			{
				LDC += ld;
				ld4++;
				if (ldAlive)
					ld += CONNECT_BONUS;
				if (ld4 >= 4)
					LDC = -10;
			}
			else
			{
				ld = 1;
				ldAlive = false;
			}
		}
		else
		{
			//LDC = 0;
			ldAlive = false;
		}
		if (isSequence(lx, uy, valueBoard, whatIsMine))
		{
			if (valueBoard[lx][uy] == MY_STONE || valueBoard[lx][uy] == BLOCK_STONE)
			{
				LUC += lu;
				if (luAlive)
					lu4 += CONNECT_BONUS;
				if (lu4 >= 4)
					LUC = -10;
			}
			else
			{
				lu = 1;
				luAlive = false;
			}
		}
		else
		{
			//LUC = 0;
			luAlive = false;
		}

	}

	if ((valueBoard[x + 1][y] == MY_STONE || valueBoard[x + 1][y] == BLOCK_STONE) && (valueBoard[x - 1][y] == MY_STONE || valueBoard[x - 1][y] == BLOCK_STONE))
		Hor += CONNECT_BONUS;

	if ((valueBoard[x + 1][y + 1] == MY_STONE || valueBoard[x + 1][y + 1] == BLOCK_STONE) && (valueBoard[x - 1][y - 1] == MY_STONE || valueBoard[x - 1][y - 1] == BLOCK_STONE))
		Dia1 += CONNECT_BONUS;

	if ((valueBoard[x + 1][y - 1] == MY_STONE || valueBoard[x + 1][y - 1] == BLOCK_STONE) && (valueBoard[x - 1][y + 1] == MY_STONE || valueBoard[x - 1][y + 1] == BLOCK_STONE))
		Dia2 += CONNECT_BONUS;

	if ((valueBoard[x][y + 1] == MY_STONE || valueBoard[x][y + 1] == BLOCK_STONE) && (valueBoard[x - 1][y - 1] == MY_STONE || valueBoard[x - 1][y - 1] == BLOCK_STONE))
		Ver += CONNECT_BONUS;

	Hor += LC + RC;
	Ver += UC + DC;
	Dia1 += LUC + RDC;
	Dia2 += RUC + LDC;


	fprintf(ConnectLog, "\nConnect of ( %d , %d )\n\t%d\t%d\t%d\n\t%d\t\t%d\n\t%d\t%d\t%d\n", x, y, LUC, UC, RUC, LC, RC, LDC, DC, RDC);

	return max((Hor + Ver + Dia1 + Dia2), max(Hor, max(Ver, max(Dia1, Dia2))));
}
void Evaluate(int point[2], int valueBoard[][19], int whatIsMine,int xAb[2],int yAb[2])
{
	expect* lineUp;
	int MV = 0;
	int MQ = 0;
	int deployed = 0;
	int subQ = 0;
	MV = GetMV(valueBoard,xAb,yAb);
	fprintf(EvaLog, "HERE!!\n");
	MQ = GetMQ(MV, valueBoard,xAb,yAb);
	fprintf(EvaLog, "\nafter Get MQ : %d\n", MQ);
	if (MV > 3)
		subQ = GetMQ(MV - 1, valueBoard,xAb,yAb);
	int maxPair[4] = { 0 };
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
	fprintf(EvaLog, "\nbefore malloc MQ : %d\n", MQ);
	lineUp = (expect*)malloc(sizeof(expect)*(MQ + subQ));
	fprintf(EvaLog, "\n\n----------------Expectation start----------------\n\n");
	fprintf(ExpMax, "\n\n-------------original Board-------------------\n\n");
	fprintf(EvaLog, "now MV : %d , now MQ : %d , subQ : %d\n\n", MV, MQ, subQ);
	LogBoard(ExpMax, valueBoard);
	for (int x = iterMin; x < iterMax; x++)
	{
		for (int y = iterMin; y < iterMax; y++)
		{
			int target = valueBoard[x][y];
			if (!IsAbandoned(x,y,xAb,yAb) && (target == MV || (target == MV - 1 && MV > 3)))
			{
				fprintf(EvaLog, "\nMQ : %d , deployed : %d\nAbout ( %d , %d )\n", MQ, deployed, x, y);

				lineUp[deployed].point[0] = x;
				lineUp[deployed].point[1] = y;
				lineUp[deployed].distance = GetDistance(x, y, 9, 9);
				CloneBoard(valueBoard, lineUp[deployed].expectBoard);
				lineUp[deployed].expectBoard[x][y] = whatIsMine; //여기 중요!!
				ValueSet(lineUp[deployed].expectBoard, whatIsMine);
				lineUp[deployed].connect = GetConnect(lineUp[deployed], lineUp[deployed].expectBoard, MY_STONE);
				fprintf(ExpMax, "\n\n-------------expectBoard of deployed = %d-------------------\n\n", deployed);
				LogBoard(ExpMax, lineUp[deployed].expectBoard);
				lineUp[deployed].expectMax = GetMV(lineUp[deployed].expectBoard,xAb,yAb);
				lineUp[deployed].expectQuan = GetMQ(lineUp[deployed].expectMax, lineUp[deployed].expectBoard,xAb,yAb);
				if (lineUp[deployed].expectMax > 3)
					lineUp[deployed].subQ = GetMQ(lineUp[deployed].expectMax - 1, lineUp[deployed].expectBoard,xAb,yAb);
				fprintf(ExpMax, "( %d , %d )\nexpectMax = %d\nexpectQuan = %d\n\n", x, y,lineUp[deployed].expectMax, lineUp[deployed].expectQuan);

				fprintf(EvaLog, "( %d , %d )\nexpectMax = %d\nexpectQuan = %d\nconnect = %d\n\n ", x, y,lineUp[deployed].expectMax, lineUp[deployed].expectQuan, lineUp[deployed].connect);

				fprintf(ConnectLog, "\nmaxPair[3] = %d , lineUp[deployed].connect = %d\n", maxPair[3], lineUp[deployed].connect);
				if (maxPair[0] < lineUp[deployed].expectMax)
				{
					/*
					maxPair
					0 = MV
					1 = MQ
					2 = distance
					3 = connect
					*/
					maxPair[0] = lineUp[deployed].expectMax;
					maxPair[1] = lineUp[deployed].expectQuan;
					maxPair[2] = lineUp[deployed].distance;
					point[0] = x;
					point[1] = y;
					maxPair[3] = lineUp[deployed].connect;

				}
				else if (maxPair[0] == lineUp[deployed].expectMax)
				{

					if (maxPair[3] < lineUp[deployed].connect)
					{
						maxPair[1] = lineUp[deployed].expectQuan;
						maxPair[2] = lineUp[deployed].distance;
						maxPair[3] = lineUp[deployed].connect;
						point[0] = x;
						point[1] = y;
					}
					else if (maxPair[3] == lineUp[deployed].connect)
					{
						if (maxPair[1] < lineUp[deployed].expectQuan)
						{

							maxPair[1] = lineUp[deployed].expectQuan;
							maxPair[2] = lineUp[deployed].distance;
							point[0] = x;
							point[1] = y;
						}
						else if (maxPair[1] == lineUp[deployed].expectQuan)
						{
							if (maxPair[2] > lineUp[deployed].distance)
							{

								maxPair[2] = lineUp[deployed].distance;
								point[0] = x;
								point[1] = y;

							}
						}
					}
				}

				deployed++;

			}
			if (deployed >= MQ + subQ)
				break;
		}

		if (deployed >= MQ + subQ)
			break;
	}

	free(lineUp);
}

int GetTotalPoint(int valueBoard[][19], int atLeast)
{
	int total = 0;
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (valueBoard[x][y] >= atLeast)
			{
				total += valueBoard[x][y];
			}
		}
	}
	return total;
}

void Compare(int point[2], int myBoard[][19], int oppoBoard[][19],int xAb[2], int yAb[2],int whatIsMine,int whatIsOppo)
{
	expect AtkPoint;
	expect DefPoint;
	int myBefore = 0;
	int oppoBefore = 0;
	int AtkTotal = 0;
	int DefTotal = 0;
	int myMV = 0;
	int oppoMV = 0;
	int AtkD = 0;
	int DefD = 0;
	myMV = GetMV(myBoard, xAb, yAb);
	oppoMV = GetMV(oppoBoard, xAb, yAb);
	myBefore = GetTotalPoint(myBoard, myMV);
	oppoBefore = GetTotalPoint(oppoBoard, oppoMV);
	if (myMV > 3)
	{
		myBefore += GetTotalPoint(myBoard, myMV - 1);
	}
	if (oppoMV > 3)
	{
		oppoBefore += GetTotalPoint(oppoBoard, myMV - 1);
	}
	Evaluate(AtkPoint.point, myBoard, whatIsMine,xAb,yAb);
	Evaluate(DefPoint.point, oppoBoard, whatIsOppo,xAb,yAb);
	CloneBoard(myBoard, AtkPoint.expectBoard);
	CloneBoard(oppoBoard, DefPoint.expectBoard);
	AtkPoint.expectBoard[AtkPoint.point[0]][AtkPoint.point[1]] = whatIsMine;
	DefPoint.expectBoard[DefPoint.point[0]][DefPoint.point[1]] = whatIsOppo;
	ValueSet(AtkPoint.expectBoard, whatIsMine);
	ValueSet(DefPoint.expectBoard, whatIsOppo);
	AtkPoint.expectMax = GetMV(AtkPoint.expectBoard,xAb,yAb);
	DefPoint.expectMax = GetMV(DefPoint.expectBoard,xAb,yAb);

	AtkTotal = GetTotalPoint(AtkPoint.expectBoard, AtkPoint.expectMax);
	DefTotal = GetTotalPoint(DefPoint.expectBoard, DefPoint.expectMax);
	AtkPoint.distance = GetDistance(AtkPoint.point[0], AtkPoint.point[1], 9, 9);
	DefPoint.distance = GetDistance(DefPoint.point[0], DefPoint.point[1], 9, 9);
	if (AtkPoint.expectMax < DefPoint.expectMax)
	{
		point[0] = DefPoint.point[0];
		point[1] = DefPoint.point[1];
	}
	else if (AtkPoint.expectMax == DefPoint.expectMax)
	{
		if (AtkPoint.expectQuan < DefPoint.expectQuan)
		{
			if (AtkPoint.connect >= DefPoint.connect)
			{
				point[0] = AtkPoint.point[0];
				point[1] = AtkPoint.point[1];
			}
			else
			{
				point[0] = DefPoint.point[0];
				point[1] = DefPoint.point[1];
			}


		}
		else
		{
			point[0] = AtkPoint.point[0];
			point[1] = AtkPoint.point[1];

		}
	}
	else
	{
		point[0] = AtkPoint.point[0];
		point[1] = AtkPoint.point[1];
	}
}
int Basic(int x[2], int y[2], int except1x[2],int except1y[2],int whatIsMine,int whatIsOppo,int initBoard[][19])
{
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
	int xAb[2] = { 0 };
	int yAb[2] = { 0 };
	for (int i = 0; i < 2; i++)
	{
		xAb[i] = except1x[i];
		yAb[i] = except1y[i];
		
	}
	
	
	fprintf(PathLog, "\nTURN START\n");
	if (!terminateAI)
	{
		//첫번째 보드 초기화
		CloneBoard(initBoard, myValue);
		CloneBoard(initBoard, oppoValue);
		
		//점수를 주자
		ValueSet(myValue, whatIsMine);
		ValueSet(oppoValue, whatIsOppo);
		LogBoard(BoardLog, myValue);
		//당장에 긴급상황 검사를 다 시행하고 저장
		firstMyFin = EvaluateFinish(APoint1, myValue, whatIsOppo) == FIND_FINISH;
		firstOppoFin = EvaluateFinish(DPoint1, myValue, whatIsMine) == FIND_FINISH;
		fprintf(PathLog, "DPoint1 = ( %d , %d )\n", DPoint1[0], DPoint1[1]);
		//내 단독승리
		if (firstMyFin && !firstOppoFin)
		{
			fprintf(PathLog, "I FOUND MY ONLY WIN\n");
			seven = isSevenInRow(APoint1, myValue, whatIsOppo);

			if (seven)
			{
				fprintf(PathLog, "BUT IT WAS SEVEN\n");
				goto PEACE;
			}
			CloneBoard(myValue, myValue2);
			myValue2[APoint1[0]][APoint1[1]] = whatIsMine;
			CloneBoard(oppoValue, oppoValue2);
			oppoValue2[APoint1[0]][APoint1[1]] = whatIsMine;
			ValueSet(oppoValue, whatIsOppo);
			ValueSet(myValue2, whatIsMine);
			secondMyFin = EvaluateFinish(APoint2, myValue2, whatIsOppo) == FIND_FINISH;
			//내 승리로 끝내는 수가 보이면

			seven = isSevenInRow(APoint2, myValue2, whatIsOppo);
			fprintf(PathLog, "I FOUND MY ONLY WIN\n");
			if (seven)
			{
				fprintf(PathLog, "BUT IT WAS SEVEN\n");
				goto PEACE;
			}
			fprintf(PathLog, "WE WIN\n");
			x[0] = APoint1[0];
			x[1] = APoint2[0];
			y[0] = APoint1[1];
			y[1] = APoint2[1];
			return FINDMYFINISH;

		}
		//상대 단독승리
		else if (!firstMyFin && firstOppoFin)
		{
			seven = isSevenInRow(DPoint1, myValue, whatIsOppo);
			fprintf(PathLog, "I FOUND ONLY OPPO WIN\n");
			if (seven)
			{
				fprintf(PathLog, "BUT IT WAS SEVEN\n");
				goto PEACE;
			}

			CloneBoard(myValue, myValue2);
			myValue2[DPoint1[0]][DPoint1[1]] = whatIsMine;
			LogBoard(PathLog, myValue2);
			ValueSet(myValue2, whatIsMine);
			CloneBoard(oppoValue, oppoValue2);
			oppoValue2[DPoint1[0]][DPoint1[1]] = whatIsMine;
			ValueSet(oppoValue, whatIsOppo);
			LogBoard(PathLog, myValue2);
			fprintf(PathLog, "DPoint1 = ( %d , %d )\n", DPoint1[0], DPoint1[1]);
			secondOppoFin = EvaluateFinish(DPoint2, myValue2, whatIsMine) == FIND_FINISH;
			//상대 단독 승리 상황 유지시....
			if (secondOppoFin)
			{
				fprintf(PathLog, "I FOUND ONLY OPPO WIN\n");
				seven = isSevenInRow(DPoint2, myValue2, whatIsMine);
				if (seven)
				{
					fprintf(PathLog, "BUT IT WAS SEVEN\n");
					goto PEACE;
				}
				fprintf(PathLog, "TWO DEFENCE POINT\n");
				x[0] = DPoint1[0];
				x[1] = DPoint2[0];
				y[0] = DPoint1[1];
				y[1] = DPoint2[1];
				return FINDOPPOFINISH;
			}
			//앞의 한 수로 방어 성공했으니 하고 싶은데로 해
			else
			{
				fprintf(PathLog, "ONE DEFENCE POINT\n");
				do {
					//Evaluate(VPoint2, myValue2,);
					//Evaluate(VPoint2, myValue2, whatIsMine);
					Compare(VPoint2, myValue2, oppoValue2, xAb, yAb, whatIsMine, whatIsOppo);
					seven = isSevenInRow(VPoint2, myValue2, whatIsOppo);
					if (seven)
					{

						myValue2[VPoint2[0]][VPoint2[1]] = 0;
					}
				} while (seven);
				x[0] = DPoint1[0];
				x[1] = VPoint2[0];
				y[0] = DPoint1[1];
				y[1] = VPoint2[1];
				fprintf(ScanLog, "\n\n( %d , %d ) , ( %d , %d )\n\n", x[0], y[0], x[1], y[1]);
			}
		}
		//양측 공동승리
		else if (firstMyFin && firstOppoFin)
		{
			fprintf(PathLog, "BOTH SIDE ON FINISH\n");
			boolean atkMod = true;
			boolean sevenA = false;
			boolean sevenD = false;
			sevenA = isSevenInRow(APoint1, myValue, whatIsOppo);
			sevenD = isSevenInRow(DPoint1, myValue, whatIsOppo);
			if (sevenA && sevenD)
			{
				fprintf(PathLog, "BUT IT WAS SEVEN\n");
				goto PEACE;
			}
			else if (!sevenA)
			{
				fprintf(PathLog, "MINE WAS SEVEN\n");
				atkMod = true;
			}
			else if (!sevenD)
			{
				fprintf(PathLog, "OPPO WAS SEVEN\n");
				atkMod = false;
			}

			CloneBoard(myValue, myValue2);
			CloneBoard(oppoValue, oppoValue2);


			if (atkMod)
			{
				fprintf(PathLog, "WE SHOULD STRIKE\n");
				myValue2[APoint1[0]][APoint1[1]] = whatIsMine;
				oppoValue2[APoint1[0]][APoint1[1]] = whatIsMine;
				ValueSet(myValue, whatIsMine);
				ValueSet(oppoValue, whatIsOppo);
				secondMyFin = EvaluateFinish(APoint2, myValue2, whatIsOppo) == FIND_FINISH;;

				seven = isSevenInRow(APoint2, myValue2, whatIsOppo);
				if (seven)
				{
					fprintf(PathLog, "BUT IT WAS SEVEN\n");
					goto DEFMOD;
				}


				fprintf(PathLog, "WE WIN\n");

				x[0] = APoint1[0];
				x[1] = APoint2[0];
				y[0] = APoint1[1];
				y[1] = APoint2[1];
				return FINDMYFINISH;

			}

			else
			{
			DEFMOD:
				fprintf(PathLog, "WE SHOULD DEFENCE\n");
				myValue2[DPoint1[0]][DPoint1[1]] = whatIsMine;
				ValueSet(myValue, whatIsMine);
				oppoValue2[DPoint1[0]][DPoint1[1]] = whatIsMine;
				ValueSet(oppoValue, whatIsOppo);

				secondOppoFin = EvaluateFinish(DPoint2, myValue2, whatIsOppo) == FIND_FINISH;

				seven = isSevenInRow(DPoint2, myValue2, whatIsOppo);
				//방어 후 추가 방어 필요
				if (secondOppoFin && !seven)
				{
					x[0] = DPoint1[0];
					x[1] = DPoint2[0];
					y[0] = DPoint1[1];
					y[1] = DPoint2[1];
					return FINDOPPOFINISH;
				}
				//방어 하고 싶어도 못해
				else if (seven)
				{
					fprintf(PathLog, "BUT IT WAS SEVEN\n");
					goto PEACE;
				}
				//이미 다 방어 했으니 하고 싶은데 해
				else
				{

					fprintf(PathLog, "DEFENCE FINISHED\n");

					do {
						//Evaluate(VPoint2, myValue2);
						//Evaluate(VPoint2, myValue2, whatIsMine);
						Compare(VPoint2, myValue2, oppoValue2, xAb, yAb, whatIsMine, whatIsOppo);
						seven = isSevenInRow(VPoint2, myValue2, whatIsOppo);
						if (seven)
						{
							myValue2[VPoint2[0]][VPoint2[1]] = 0;
						}
					} while (seven);

					x[0] = DPoint1[0];
					x[1] = VPoint2[0];
					y[0] = DPoint1[1];
					y[1] = VPoint2[1];
					
					
					return FINDNOFINISH;
				}
			}

		}
		//평화의 시대
		else
		{

		PEACE:

			fprintf(PathLog, "WE GO PEACE\n");

			do {
				//Evaluate(VPoint1, myValue);
				//Evaluate(VPoint1, myValue, whatIsMine);
				Compare(VPoint1, myValue, oppoValue, xAb, yAb, whatIsMine, whatIsOppo);
				seven = isSevenInRow(VPoint1, myValue, whatIsOppo);
				if (seven)
				{
					myValue[VPoint1[0]][VPoint1[1]] = 0;
				}
			} while (seven);

			CloneBoard(myValue, myValue2);
			myValue2[VPoint1[0]][VPoint1[1]] = whatIsMine;
			ValueSet(myValue2, whatIsMine);
			CloneBoard(oppoValue, oppoValue2);
			oppoValue2[VPoint1[0]][VPoint1[1]] = whatIsMine;
			ValueSet(oppoValue2, whatIsOppo);
			do {
				//Evaluate(VPoint2, myValue2);
				//
				//Evaluate(VPoint2, myValue2, whatIsMine);
				Compare(VPoint2, myValue2, oppoValue2, xAb, yAb,whatIsMine,whatIsOppo);
				seven = isSevenInRow(VPoint2, myValue2, whatIsOppo);
				if (seven)
				{
					myValue2[VPoint2[0]][VPoint2[1]] = 0;
				}
			} while (seven);
			
			x[0] = VPoint1[0];
			x[1] = VPoint2[0];
			y[0] = VPoint1[1];
			y[1] = VPoint2[1];
			return FINDNOFINISH;
			
		}
		/*
		긴급상황 검사를 한 점과 그 이후에 둘 두번째 점을 칠목 검사하자.
		우선 검사 순위는
		내승   내승   -> 공격      7목 발생시 아예 제외
		적승   양승   -> 방어      7목 발생시 아예 제외
		적승   적승   -> 방어      7목 발생시 아예 제외
		양승   양승   -> 공격      7목 발생시 방어
		이벨   내승   -> 무시      이벨   이벨로 이동
		이벨   이벨   -> 7목 발생시 재시도

		LogBoard(BoardLog, myValue);

		LogBoard(BoardLog, myValue2);
		*/


	}

	return 0;
}

expectRecurse* CallRecurseMine(int depth, expectRecurse* seed, boolean lastMyF, boolean lastOppoF);
expectRecurse* CallRecurseOppo(int depth, expectRecurse* seed, boolean lastMyF, boolean lastOppoF);


boolean isFirstTurn()
{
	int count = 0;
	for (int x = 0; x < 19; x++)
	{
		for (int y = 0; y < 19; y++)
		{
			if (isFree(x, y))
				count++;
		}
	}

	return count >= 360;
}

void myturn(int cnt)
{


	int x[2] = { 0 };
	int y[2] = { 0 };
	expectRecurse* start;
	expectRecurse* result;
	InitLog();
	fprintf(RecurseLog, "---------------RECURSE ROOT---------------\n");
	if (!isFirstTurn())
	{
		start = (expectRecurse*)malloc(sizeof(expectRecurse));
		InitValue(start->expectBoard, MY_STONE, OPPO_STONE);
		fprintf(RecurseLog, "---------------RECURSE ROOT---------------\n");
		result = CallRecurseMine(1, start, false, false);


		x[0] = result->x[0];
		x[1] = result->x[1];
		y[0] = result->y[0];
		y[1] = result->y[1];
		free(result);
		free(start);
	}
	else
	{
		int tmp1[2] = { -1 };
		int tmp2[2] = { -1 };
		int valueBoard[19][19] = { 0 };
		InitValue(valueBoard, MY_STONE, OPPO_STONE);
		Basic(x, y, tmp1, tmp2, MY_STONE, OPPO_STONE, valueBoard);
	}
		
	
	// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
	// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다

	

	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
	TermLog();

	domymove(x, y, cnt);
}
expectRecurse* CallRecurseMine(int depth, expectRecurse* seed, boolean lastMyF, boolean lastOppoF)
{

	//베이직 함수에서 점2 점3의 좌표를 결과에서 제외할 것. 
	//둘이 아닌 차선책 배출 done

	//후보군 3개에 대한 평가 후 리턴
	int point1x[2] = { -1 };
	int point1y[2] = { -1 };
	int point2x[2] = { -1 };
	int point2y[2] = { -1 };

	expectRecurse* num1;
	expectRecurse* num2;
	expectRecurse* get1;
	expectRecurse* get2;
	fprintf(RecurseLog, "depth left : %d\n",depth);
	num1 = (expectRecurse*)malloc(sizeof(expectRecurse));
	num2 = (expectRecurse*)malloc(sizeof(expectRecurse));
	CloneBoard(seed->expectBoard, num1->expectBoard);
	CloneBoard(seed->expectBoard, num2->expectBoard);

	num1->finishScan = Basic(point1x, point1y, point2x, point2y, MY_STONE, OPPO_STONE, seed->expectBoard);
	num1->x[0] = point1x[0];
	num1->x[1] = point1x[1];
	num1->y[0] = point1y[0];
	num1->y[1] = point1y[1];

	num2->finishScan = Basic(point2x, point2y, point1x, point1y, MY_STONE, OPPO_STONE, seed->expectBoard);
	num2->x[0] = point2x[0];
	num2->x[1] = point2x[1];
	num2->y[0] = point2y[0];
	num2->y[1] = point2y[1];
	if (num1->finishScan == FINDMYFINISH && lastMyF)
	{
		fprintf(RecurseLog, "MY FINISH X 2");
		num1->myWin = true;
		num1->finish = true;
		free(num2);
		return num1;
	}
	else if (num2->finishScan == FINDMYFINISH && lastMyF)
	{
		fprintf(RecurseLog, "MY FINISH X 2");
		num2->myWin = true;
		num2->finish = true;
		free(num1);
		return num2;
	}
	
	else
	{
		//클론보드 후 선정한 점 반영한 뒤 진행


		CloneBoard(seed->expectBoard, num1->expectBoard);
		num1->expectBoard[num1->x[0]][num1->y[0]] = MY_STONE;
		num1->expectBoard[num1->x[1]][num1->y[1]] = MY_STONE;
		fprintf(RecurseLog, "Call baby 1 of Depth : %d\n\n",depth);
		get1 = CallRecurseOppo(depth, num1, num1->finishScan == FINDMYFINISH, lastOppoF);

		CloneBoard(seed->expectBoard, num2->expectBoard);
		num2->expectBoard[num2->x[0]][num2->y[0]] = MY_STONE;
		num2->expectBoard[num2->x[1]][num2->y[1]] = MY_STONE;
		fprintf(RecurseLog, "Call baby 2 of Depth : %d\n\n", depth);
		get2 = CallRecurseOppo(depth, num2, num2->finishScan == FINDMYFINISH, lastOppoF);
	
		
		
		if (get1->myWin)
		{
			free(get1);
			free(get2);
			free(num2);
			num1->myWin = true;
			return num1;
		}
		else if (get2->myWin)
		{
			free(get1);
			free(num1);
			free(get2);
			num2->myWin = true;
			return num2;
		}
		else
		{
			free(num2);
			free(get1);
			free(get2);
			return num1;
		}
		

	}
}


expectRecurse* CallRecurseOppo(int depth, expectRecurse* seed, boolean lastMyF, boolean lastOppoF)
{
	//베이직 함수에서 점2 점3의 좌표를 결과에서 제외할 것. 
	//둘이 아닌 차선책 배출 done

	//후보군 3개에 대한 평가 후 리턴
	int point1x[2] = { -1 };
	int point1y[2] = { -1 };
	int point2x[2] = { -1 };
	int point2y[2] = { -1 };

	expectRecurse* num1;
	expectRecurse* num2;
	expectRecurse* get1;
	expectRecurse* get2;
	fprintf(RecurseLog, "depth left : %d in oppo\n", depth);
	num1 = (expectRecurse*)malloc(sizeof(expectRecurse));
	num2 = (expectRecurse*)malloc(sizeof(expectRecurse));
	ReverseCloneBoard(seed->expectBoard, num1->expectBoard);
	ReverseCloneBoard(seed->expectBoard, num2->expectBoard);

	num1->finishScan = Basic(point1x, point1y, point2x, point2y, MY_STONE, OPPO_STONE, seed->expectBoard);
	num1->x[0] = point1x[0];
	num1->x[1] = point1x[1];
	num1->y[0] = point1y[0];
	num1->y[1] = point1y[1];

	num2->finishScan = Basic(point2x, point2y, point1x, point1y, MY_STONE, OPPO_STONE, seed->expectBoard);
	num2->x[0] = point2x[0];
	num2->x[1] = point2x[1];
	num2->y[0] = point2y[0];
	num2->y[1] = point2y[1];
	if (num1->finishScan == FINDMYFINISH && lastOppoF && num2->finishScan == FINDMYFINISH && lastOppoF)
	{
		fprintf(RecurseLog, "OPPO FINISH X 2");
		num1->myWin = false;
		num1->finish = true;
		free(num2);
		return num1;
	}
	
	
	else
	{
		//클론보드 후 선정한 점 반영한 뒤 진행


		ReverseCloneBoard(seed->expectBoard, num1->expectBoard);
		num1->expectBoard[num1->x[0]][num1->y[0]] = OPPO_STONE;
		num1->expectBoard[num1->x[1]][num1->y[1]] = OPPO_STONE;
		if (depth > 1)
		{
			fprintf(RecurseLog, "Call baby 1 of Depth : %d in oppo\n\n", depth);
			get1 = CallRecurseMine(depth - 1, num1, lastMyF, num1->finishScan == FINDMYFINISH);
		}
		ReverseCloneBoard(seed->expectBoard, num2->expectBoard);
		num2->expectBoard[num2->x[0]][num2->y[0]] = OPPO_STONE;
		num2->expectBoard[num2->x[1]][num2->y[1]] = OPPO_STONE;
		{
			fprintf(RecurseLog, "Call baby 1 of Depth : %d in oppo\n\n", depth);
			if (depth > 1)
				get2 = CallRecurseMine(depth - 1, num2, lastMyF, num2->finishScan == FINDMYFINISH);
		}

		if (get1 == NULL && get2 == NULL)
		{
			fprintf(RecurseLog, "\nDEAD END\n\n", depth);
			free(num2);
			return num1;
		}
		else
		{
			if (get1!= NULL && get1->myWin)
			{
				free(get1);
				free(get2);
				free(num2);
				num1->myWin = true;
				return num1;
			}
			else if (get2 != NULL && get2->myWin)
			{
				free(get1);
				free(num1);
				free(get2);
				num2->myWin = true;
				return num2;
			}
			else
			{
				free(num2);
				free(get1);
				free(get2);
				return num1;
			}
		}


	}
}