#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>

enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FLAG,
	ENTER,
	ESCAPE
};

char defaultControls[7] = {'w', 's', 'a', 'd', 'e', ' ', 'q'};
char currentControls[7] = {'w', 's', 'a', 'd', 'e', ' ', 'q'};

BOOL isCharInArray(char target, char array[], int size) {
	for (int i = 0; i < size; i++) {
		if (array[i] == target) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL isControls(int C) {
	return isCharInArray(C, currentControls, sizeof(currentControls));
}

void GotoXY(int x, int y) {
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void SetConsoleWindow(int cols, int lines) {
	char command[40] = { 0 };
	sprintf(command, "mode con:cols=%d lines=%d", cols, lines);
	system(command);
}

void CursorView(BOOL visible) {
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 1;
	cci.bVisible = visible;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

void textColor(int colorNum) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorNum);
}

int getkey() {
	int key = _getch();
	if (isupper(key)) {
		key = tolower(key);
	}
	else if (key == 224 || key == 0) {
		key = _getch();
	}
	return key;
}

int getcontrols() {
	int key;
	do {
		key = getkey();
	} while (key != 27 && !isControls(key));
	return key;
}

void drawTitle() {
	textColor(14);
	GotoXY(0, 4);
	printf("           @@   @@  @@@  @   @  @@@@@   @@@@   @     @  @@@@@  @@@@@  @@@@   @@@@@  @@@@            ");
	GotoXY(0, 5);
	printf("           @ @ @ @   @   @@  @  @      @       @     @  @      @      @   @  @      @   @           ");
	GotoXY(0, 6);
	printf("           @  @  @   @   @ @ @  @@@@@   @@@@   @  @  @  @@@@@  @@@@@  @@@@   @@@@@  @@@@            ");
	GotoXY(0, 7);
	printf("           @     @   @   @  @@  @           @  @ @ @ @  @      @      @      @      @  @            ");
	GotoXY(0, 8);
	printf("           @     @  @@@  @   @  @@@@@   @@@@    @   @   @@@@@  @@@@@  @      @@@@@  @   @           ");
}

typedef struct ConsoleBuffer {
	CHAR_INFO* buffer;
	COORD bufferSize;
} ConsoleBuffer;

ConsoleBuffer saveConsoleBuffer(COORD bufferSize) {
	CHAR_INFO* buffer = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * bufferSize.X * bufferSize.Y);
	if (buffer == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	COORD bufferCoord = { 0, 0 };
	SMALL_RECT readRegion = { 0, 0, bufferSize.X - 1, bufferSize.Y - 1 };

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	ReadConsoleOutput(console, buffer, bufferSize, bufferCoord, &readRegion);

	ConsoleBuffer consoleBuffer = { buffer, bufferSize };
	return consoleBuffer;
}

void restoreConsoleBuffer(ConsoleBuffer consoleBuffer) {
	if (consoleBuffer.buffer == NULL) {
		return;
	}
	SetConsoleWindow(consoleBuffer.bufferSize.X, consoleBuffer.bufferSize.Y);

	COORD bufferCoord = { 0, 0 };
	SMALL_RECT writeRegion = { 0, 0, consoleBuffer.bufferSize.X - 1, consoleBuffer.bufferSize.Y - 1 };

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	WriteConsoleOutput(console, consoleBuffer.buffer, consoleBuffer.bufferSize, bufferCoord, &writeRegion);
}

void freeConsoleBuffer(ConsoleBuffer* consoleBuffer) {
	if (consoleBuffer->buffer == NULL) {
		return;
	}
	free(consoleBuffer->buffer);
	consoleBuffer->buffer = NULL;
}

void printSpecial(char c) {
	if (isdigit(c)) {
		printf("%c%c", -93, -80 + c - '0');
	}
	else if (islower(c)) printf("%c%c", -93, -63 + c - 'a');
	else if (c == 80) printf("%c%c", -95, -23);
	else if (c == 72) printf("%c%c", -95, -24);
	else if (c == 75) printf("%c%c", -95, -25);
	else if (c == 77) printf("%c%c", -95, -26);
	else if (c == '!') printf("ㄐ");
	else if (c == '?') printf("ˋ");
	else if (c == ',') printf("ㄛ");
	else if (c == '.') printf("ㄝ");
	else if (c == ':') printf("ㄩ");
	else if (c == ';') printf("˙");
	else if (c == '\'') printf("ㄖ");
	else if (c == '\"') printf("ㄑ");
	else if (c == '(') printf("ㄗ");
	else if (c == ')') printf("ㄘ");
	else if (c == '[') printf("��");
	else if (c == ']') printf("��");
	else if (c == '{') printf("��");
	else if (c == '}') printf("��");
	else if (c == '<') printf("●");
	else if (c == '>') printf("△");
	else if (c == '+') printf("ㄚ");
	else if (c == '-') printf("ㄜ");
	else if (c == '=') printf("ˊ");
	else if (c == '$') printf("ㄓ");
	else if (c == '%') printf("ㄔ");
	else if (c == '#') printf("ㄒ");
	else if (c == '&') printf("ㄕ");
	else if (c == '*') printf("ㄙ");
	else if (c == '@') printf("��");
	else if (c == '^') printf("��");
	else if (c == '|') printf("��");
	else if (c >= 1 && c <= 26) printf("^%c", c + 'A' - 1);
	else if (c == 32) printf("╡");
	else printf("%c ", c);
}

void drawSquareBox(COORD pos, COORD size, int plusSizeX, BOOL clearContents) {
	GotoXY(pos.X, pos.Y);
	printf("忙式");
	for (int i = 0; i < size.X - 2; i++) {
		printf("式式");
	}
	for (int i = 0; i < plusSizeX; i++) {
		printf("式");
	}
	printf("忖 ");
	for (int i = 1; i <= size.Y - 2; i++) {
		GotoXY(pos.X, pos.Y + i);
		printf("弛 ");
		if (clearContents) {
			for (int j = 0; j < size.X - 2; j++) {
				printf("  ");
			}
			for (int j = 0; j < plusSizeX; j++) {
				printf(" ");
			}
		}
		GotoXY(pos.X + (size.X - 1) * 2 + plusSizeX, pos.Y + i);
		printf("弛 ");
	}
	GotoXY(pos.X, pos.Y + size.Y - 1);
	printf("戌式");
	for (int i = 0; i < size.X - 2; i++) {
		printf("式式");
	}
	for (int i = 0; i < plusSizeX; i++) {
		printf("式");
	}
	printf("戎 ");
}

int quitKeySettings() {
	textColor(15);
	COORD boxPos = { 29, 12 };
	COORD boxSize = { 21, 11 };
	drawSquareBox(boxPos, boxSize, 0, TRUE);
	GotoXY(42, 15);
	printf("Quit with saving?");

	int selection = 0;
	while (1) {
		textColor(15);
		GotoXY(41, 19);
		printf("Yes    No    Cancle");
		textColor(11);
		switch (selection) {
		case 0:
			GotoXY(41, 19);
			printf("Yes");
			break;

		case 1:
			GotoXY(48, 19);
			printf("No");
			break;

		case 2:
			GotoXY(54, 19);
			printf("Cancle");
			break;
		}

		int key = getkey();
		if (key == 'y') {
			if (selection == 0) {
				return 0;
			}
			else {
				selection = 0;
			}
		}
		else if (key == 'n') {
			if (selection == 1) {
				return 1;
			}
			else {
				selection = 1;
			}
		}
		else if (key == 'c') {
			if (selection == 2) {
				return 2;
			}
			else {
				selection = 2;
			}
		}
		else if (key == currentControls[ENTER]) {
			break;
		}
		else if (key == currentControls[ESCAPE] || key == 27) {
			if (selection == 2) {
				break;
			}
			else {
				selection = 2;
			}
		}
		else if (key == currentControls[LEFT] && selection > 0) {
			selection--;
		}
		else if (key == currentControls[RIGHT] && selection < 2) {
			selection++;
		}
	}
	return selection;
}

enum {
	RESET = 7,
	QUIT = 8
};

void keySettings() {
	GotoXY(0, 9);
	for (int i = 0; i < 100 * 21; i++) {
		_putch(' ');
	}

	char tempControls[7];
	memcpy(tempControls, currentControls, sizeof(currentControls));
	COORD squareSize = { 3, 3 };

	COORD posArray[9] = {
		{ 25, 16 },
		{ 25, 19 },
		{ 19, 19 },
		{ 31, 19 },
		{ 53, 18 },
		{ 64, 18 },
		{ 75, 18 },
		{ 44, 26 },
		{ 52, 26 }
	};

	textColor(15);
	GotoXY(posArray[UP].X - 1, posArray[UP].Y - 2);
	printf("CONTROLS");

	GotoXY(posArray[FLAG].X + 1, posArray[FLAG].Y - 2);
	printf("FLAG");

	GotoXY(posArray[ENTER].X - 2, posArray[ENTER].Y - 2);
	printf("ENTER/OPEN");

	GotoXY(posArray[ESCAPE].X, posArray[ESCAPE].Y - 2);
	printf("ESCAPE");

	GotoXY(posArray[RESET].X, posArray[RESET].Y);
	printf("RESET");

	GotoXY(posArray[QUIT].X, posArray[QUIT].Y);
	printf("QUIT");

	for (int i = 0; i < 7; i++) {
		GotoXY(posArray[i].X + 2, posArray[i].Y + 1);
		printSpecial(tempControls[i]);
	}

	textColor(7);
	for (int i = 1; i < 7; i++) {
		drawSquareBox(posArray[i], squareSize, 0, FALSE);
	}

	GotoXY((posArray[RESET].X + 4 + posArray[QUIT].X) / 2, posArray[RESET].Y);
	printf("/");

	textColor(11);
	drawSquareBox(posArray[UP], squareSize, 0, FALSE);

	int current = UP;

	while (1) {
		int temp = current;
		int key = getcontrols();

		if (key == currentControls[ENTER] || key == currentControls[ESCAPE] || key == 27) {
			if (current == QUIT) {
				COORD consoleSize = { 100, 30 };
				ConsoleBuffer tempBuffer = saveConsoleBuffer(consoleSize);

				int selection = quitKeySettings();
				if (selection == 2) {
					restoreConsoleBuffer(tempBuffer);
					freeConsoleBuffer(&tempBuffer);
				}
				else {
					freeConsoleBuffer(&tempBuffer);
					if (selection == 0) {
						memcpy(currentControls, tempControls, sizeof(tempControls));
					}
					break;
				}
			}
			else {
				if (key == currentControls[ENTER]) {
					if (current == RESET) {
						memcpy(tempControls, defaultControls, sizeof(defaultControls));
						textColor(15);
						for (int i = 0; i < 7; i++) {
							GotoXY(posArray[i].X + 2, posArray[i].Y + 1);
							printSpecial(tempControls[i]);
						}
					}
					else {
						textColor(4);
						drawSquareBox(posArray[current], squareSize, 0, FALSE);

						int key;
						do {
							key = getkey();
						} while (key != 27 && key != tempControls[current] && isCharInArray(key, tempControls, sizeof(tempControls)));
						if (key != 27) {
							tempControls[current] = key;
						}

						textColor(15);
						GotoXY(posArray[current].X + 2, posArray[current].Y + 1);
						printSpecial(tempControls[current]);

						textColor(11);
						drawSquareBox(posArray[current], squareSize, 0, FALSE);
					}
				}
				else {
					current = QUIT;
				}
			}
		}
		else if (key == currentControls[UP]) {
			if (current == DOWN || current == LEFT || current == RIGHT) {
				current = UP;
			}
			else if (current == RESET) {
				current = RIGHT;
			}
			else if (current == QUIT) {
				current = FLAG;
			}
		}
		else if (key == currentControls[DOWN]) {
			if (current == UP) {
				current = DOWN;
			}
			else if (current == DOWN || current == LEFT || current == RIGHT) {
				current = RESET;
			}
			else if (current == FLAG || current == ENTER || current == ESCAPE) {
				current = QUIT;
			}
		}
		else if (key == currentControls[LEFT]) {
			if (current == FLAG || current == ENTER ||
				current == ESCAPE || current == QUIT) {
				current--;
			}
			else if (current == UP || current == DOWN) {
				current = LEFT;
			}
			else if (current == RIGHT) {
				current = DOWN;
			}
		}
		else if (key == currentControls[RIGHT]) {
			if (current == RIGHT || current == FLAG ||
				current == ENTER || current == RESET) {
				current++;
			}
			else if (current == UP || current == DOWN) {
				current = RIGHT;
			}
			else if (current == LEFT) {
				current = DOWN;
			}
		}

		if (current != temp) {
			textColor(11);
			if (current < 7) {
				drawSquareBox(posArray[current], squareSize, 0, FALSE);
			}
			else {
				GotoXY(posArray[current].X, posArray[current].Y);
				printf("%s", current == RESET ? "RESET" : "QUIT");
			}

			textColor(15);
			if (temp < 7) {
				textColor(7);
				drawSquareBox(posArray[temp], squareSize, 0, FALSE);
			}
			else {
				GotoXY(posArray[temp].X, posArray[temp].Y);
				printf("%s", temp == RESET ? "RESET" : "QUIT");
			}
			GotoXY(0, 0);
		}
	}
}

void soundSettings() {
	GotoXY(0, 9);
	for (int i = 0; i < 100 * 21; i++) {
		_putch(' ');
	}

	getkey();
}

void options() {
	GotoXY(0, 9);
	for (int i = 0; i < 100 * 21; i++) {
		_putch(' ');
	}

	COORD pos[3] = {
		{ 42, 18 },
		{ 42, 21 },
		{ 42, 24 }
	};
	COORD boxSize = { 7, 3 };

	textColor(15);
	for (int i = 0; i < 3; i++) {
		drawSquareBox(pos[i], boxSize, 1, FALSE);
	}

	int selection = 0;

	while (1) {
		textColor(15);
		GotoXY(pos[0].X + 6, pos[0].Y + 1);
		printf("Key");
		GotoXY(pos[1].X + 5, pos[1].Y + 1);
		printf("Sound");
		GotoXY(pos[2].X + 5, pos[2].Y + 1);
		printf("Exits");
		for (int i = 0; i < 3; i++) {
			drawSquareBox(pos[i], boxSize, 1, FALSE);
		}

		GotoXY(0, 14);
		for (int i = 0; i < 100; i++) {
			_putch(' ');
		}

		textColor(11);
		drawSquareBox(pos[selection], boxSize, 1, FALSE);
		switch (selection) {
		case 0:
			GotoXY(pos[0].X + 6, pos[0].Y + 1);
			printf("Key");
			textColor(8);
			GotoXY(41, 14);
			printf("edit binding keys");
			break;

		case 1:
			GotoXY(pos[1].X + 5, pos[1].Y + 1);
			printf("Sound");
			textColor(8);
			GotoXY(39, 14);
			printf("change sound settings");
			break;

		case 2:
			GotoXY(pos[2].X + 5, pos[2].Y + 1);
			printf("Exits");
			textColor(8);
			GotoXY(43, 14);
			printf("exit to title");
			break;
		}

		int key = getcontrols();

		if (key == currentControls[ENTER]) {
			if (selection == 2) {
				break;
			}
			else {
				COORD consoleSize = { 100, 30 };
				ConsoleBuffer tempBuffer = saveConsoleBuffer(consoleSize);
				if (selection == 0) {
					keySettings();
				}
				else {
					soundSettings();
				}
				restoreConsoleBuffer(tempBuffer);
				freeConsoleBuffer(&tempBuffer);
			}
		}
		else if (key == currentControls[ESCAPE] || key == 27) {
			if (selection == 2) {
				break;
			}
			else {
				selection = 2;
			}
		}
		else if (key == currentControls[UP]) {
			selection = (selection + 2) % 3;
		}
		else if (key == currentControls[DOWN]) {
			selection = (selection + 1) % 3;
		}
	}
}

void selectLevel() {

}

typedef enum MenuSelection {
	PLAY,
	OPTIONS,
	EXIT
} MenuSelection;

int main() {
	COORD consoleSize = { 100, 30 };

	system("title Minesweeper");
	CursorView(FALSE);
	SetConsoleWindow(consoleSize.X, consoleSize.Y);
	system("cls");
	drawTitle();

	MenuSelection currentSelection = PLAY;

	while (1) {
		textColor(15);
		GotoXY(45, 19);
		printf("Play Game");
		drawSquareBox((COORD) { 42, 18 }, (COORD) { 7, 3 }, 1, FALSE);

		GotoXY(46, 22);
		printf("Options");
		drawSquareBox((COORD) { 42, 21 }, (COORD) { 7, 3 }, 1, FALSE);

		GotoXY(45, 25);
		printf("Exit Game");
		drawSquareBox((COORD) { 42, 24 }, (COORD) { 7, 3 }, 1, FALSE);

		GotoXY(0, 14);
		for (int i = 0; i < 100; i++) {
			_putch(' ');
		}

		switch (currentSelection) {
		case PLAY:
			textColor(8);
			GotoXY(37, 14);
			printf("select level & start game");
			textColor(11);
			GotoXY(45, 19);
			printf("Play Game");
			drawSquareBox((COORD) { 42, 18 }, (COORD) { 7, 3 }, 1, FALSE);
			break;

		case OPTIONS:
			textColor(8);
			GotoXY(42, 14);
			printf("change settings");
			textColor(11);
			GotoXY(46, 22);
			printf("Options");
			drawSquareBox((COORD) { 42, 21 }, (COORD) { 7, 3 }, 1, FALSE);
			break;

		case EXIT:
			textColor(8);
			GotoXY(42, 14);
			printf("exit to desktop");
			textColor(11);
			GotoXY(45, 25);
			printf("Exit Game");
			drawSquareBox((COORD) { 42, 24 }, (COORD) { 7, 3 }, 1, FALSE);
			break;
		}

		int key = getcontrols();

		if (key == currentControls[ENTER]) {
			if (currentSelection == EXIT) {
				break;
			}
			else {
				ConsoleBuffer tempBuffer = saveConsoleBuffer(consoleSize);
				if (currentSelection == PLAY) {
					selectLevel();
				}
				else {
					options();
				}
				restoreConsoleBuffer(tempBuffer);
				freeConsoleBuffer(&tempBuffer);
			}
		}
		else if (key == currentControls[ESCAPE] || key == 27) {
			if (currentSelection == EXIT) {
				break;
			}
			else {
				currentSelection = EXIT;
			}
		}
		else if (key == currentControls[UP]) {
			currentSelection = (currentSelection + 2) % 3;
		}
		else if (key == currentControls[DOWN]) {
			currentSelection = (currentSelection + 1) % 3;
		}
	}

	return 0;
}