

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "Connect6Algo.h"

char info[] = { "TeamName:Lucky,Department:Jason Mraz" };








void myturn(int cnt) {

	int x[2], y[2];
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