#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "resource.h"

#define xDivision 9
#define yDivision 9

#define wLine1 7	//width of line outside the paintting part
#define wLine2 5	//width of outest line in window
#define wLine3 3	//width of line between displaying area and painting area
#define DisArea 60	//heigth of displaying area
#define sizeBox 40	//length of side in every small box
#define wPerson 200	//width in king and challenger area
#define id1 1		//the id controlling bounding ball
#define time1 800  //the time of id

//red, pink, purple, blue, green, yellow, strange and this color is used to draw the ball
COLORREF color[7] = { RGB(210, 0, 0), RGB(240, 100, 200), RGB(130, 60, 200), 
					RGB(10, 20, 250), RGB(30, 190, 40), RGB(250, 250, 90), RGB(110, 240, 240) };
HBRUSH colorBrush[7];
// this brush and pen ***********
HPEN grayPen1 = CreatePen(PS_SOLID, 1, RGB(70, 70, 70));
HPEN grayPen2 = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
HPEN pen = CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
HBRUSH grayBrush = CreateSolidBrush(RGB(230, 230, 230));
HBRUSH brush = CreateSolidBrush(RGB(190, 190, 190));

//cxStart and cyStart is beginning point of paintting part
//xWin and yWin represent the width and height respectively
//state is which color ball a box of matrix has
//selectedPoint shows which box our mouse have pressed
//next[] shows three coming color ball
int cxStart;
int cyStart;
int xWin, yWin;
int state[xDivision][yDivision];
int next[3], oldNext[3];
POINT nextPoint[3];
int score = 0;
int goal = 200;

struct Node
{
	int x;
	int y;
	Node *p;
};
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("MyWinlinez");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hbrBackground = grayBrush;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows 10!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	srand((unsigned)time(NULL));

	hwnd = CreateWindow(szAppName,                  // window class name
		TEXT("Winlinez--*陈天霸"), // window caption
					  //WS_OVERLAPPEDWINDOW,
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		CW_USEDEFAULT,              // initial x size
		CW_USEDEFAULT,              // initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL);                     // creation parameters

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void drawRect(HWND hwnd, int cxbegin, int cybegin, int width, int height, int wPen, bool isWhite) {
	static HDC hdc;

	hdc = GetDC(hwnd);
	SelectObject(hdc, brush);
	Rectangle(hdc, cxbegin, cybegin, cxbegin + width, cybegin + height);

	if (isWhite) {
		SelectObject(hdc, GetStockObject(WHITE_PEN));
	}
	else
	{
		SelectObject(hdc, grayPen2);
	}
	for (int i = 0; i < wPen; ++i) {
		MoveToEx(hdc, cxbegin + i, cybegin + i, NULL);
		LineTo(hdc, cxbegin + width - i, cybegin + i);
		MoveToEx(hdc, cxbegin + i, cybegin + i, NULL);
		LineTo(hdc, cxbegin + i, cybegin + height - i);
	}

	if (isWhite) {
		SelectObject(hdc, grayPen2);
	}
	else
	{
		SelectObject(hdc, GetStockObject(WHITE_PEN));
	}
	for (int i = 0; i < wPen; ++i) {
		MoveToEx(hdc, cxbegin + width - 1 - i, cybegin + height - 1 - i, NULL);
		LineTo(hdc, cxbegin + width - 1 - i, cybegin + i);
		MoveToEx(hdc, cxbegin + width - 1 - i, cybegin + height - 1 - i, NULL);
		LineTo(hdc, cxbegin + i, cybegin + height - 1 - i);
	}

	ReleaseDC(hwnd, hdc);
	return;
}

bool inRect(int x, int y)
{
	if ((x >= 0) && (x < xDivision) && (y >= 0) && (y < yDivision))
	{
		return true;
	}
	return false;
}

bool isFull()
{
	for (int i = 0; i < xDivision; i++)
	{
		for (int j = 0; j < yDivision; j++)
		{
			if (state[i][j] == -1)
			{
				return false;
			}
		}
	}
	return true;
}

int oneDerection(int x, int y, int xStep, int yStep, int *xstart, int *ystart, int *xend, int *yend)
{
	int i = x, j = y;
	int count = 0;
	while (inRect(i, j) && (state[i][j] == state[x][y]))
	{
		count++;
		(*xstart) = i;
		(*ystart) = j;
		i -= xStep;
		j -= yStep;
	}
	i = x, j = y;
	count--;
	while (inRect(i, j) && (state[i][j] == state[x][y]))
	{
		count++;
		(*xend) = i;
		(*yend) = j;
		i += xStep;
		j += yStep;
	}
	if (count < 5)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void initiate()
{
	cxStart = wLine2 + wPerson + wLine1;
	cyStart = DisArea + wLine3 + wLine1;
	xWin = cxStart * 2 + xDivision*sizeBox;
	yWin = cyStart + yDivision*sizeBox + wLine1 + wLine2;
	score = 0;
	for (int i = 0; i < xDivision; i++)
	{
		for (int j = 0; j < yDivision; j++)
		{
			state[i][j] = -1;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		int t1, t2;
		t1 = rand() % xDivision;
		t2 = rand() % yDivision;
		if (i<5)
		{
			while (state[t1][t2] != -1)
			{
				t1 = rand() % xDivision;
				t2 = rand() % yDivision;
			}
			state[t1][t2] = (rand() % 7);
		}
		else
		{
			next[i - 5] = (rand() % 7);
		}
	}
}

void boundBall(HWND hwnd, RECT rect, POINT selectedPoint, bool btnSelected)
{
	static HDC hdc;
	//bound the ball
	if ((selectedPoint.x != -1) && (selectedPoint.y != -1))
	{
		if ((state[selectedPoint.x][selectedPoint.y] != -1) && btnSelected)
		{
			hdc = GetDC(hwnd);
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			SelectObject(hdc, colorBrush[state[selectedPoint.x][selectedPoint.y]]);
			Ellipse(hdc, rect.left, rect.top + sizeBox / 4, rect.right, rect.bottom);

			Sleep(time1 / 5);
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			SelectObject(hdc, colorBrush[state[selectedPoint.x][selectedPoint.y]]);
			Ellipse(hdc, rect.left, rect.top + sizeBox / 8, rect.right, rect.bottom);

			Sleep(time1 / 5);
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			SelectObject(hdc, colorBrush[state[selectedPoint.x][selectedPoint.y]]);
			Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

			ReleaseDC(hwnd, hdc);
		}
	}
	return;
}

bool isEliminating(HWND hwnd, int x, int y)
{
	static HDC hdc;
	int direction = 0;  //1 is vertical, 2 is horizontal, 3 is forward slash and 4 is backward slash
	int xstart = -1, ystart = -1, xend = -1, yend = -1;
	int count = 0;
	//horizontal elimination
	if (oneDerection(x, y, 1, 0, &xstart, &ystart, &xend, &yend))
	{
		hdc = GetDC(hwnd);
		for (int i = xstart; i <= xend; i++)
		{
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, cxStart + sizeBox*i + 2, cyStart + sizeBox*y + 2,
				cxStart + sizeBox*(1 + i) - 2, cyStart + sizeBox*(1 + y) - 2);
			state[i][y] = -1;
		}
		ReleaseDC(hwnd, hdc);
		return true;
	}
	//vertical elimination
	if (oneDerection(x, y, 0, 1, &xstart, &ystart, &xend, &yend))
	{
		hdc = GetDC(hwnd);
		for (int j = ystart; j <= yend; j++)
		{
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, cxStart + sizeBox*x + 2, cyStart + sizeBox*j + 2,
				cxStart + sizeBox*(1 + x) - 2, cyStart + sizeBox*(1 + j) - 2);
			state[x][j] = -1;
		}
		ReleaseDC(hwnd, hdc);
		return true;
	}
	//forward elimination
	if (oneDerection(x, y, 1, -1, &xstart, &ystart, &xend, &yend))
	{
		hdc = GetDC(hwnd);
		for (int i=xstart, j = ystart; i <= xend; j--, i++)
		{
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, cxStart + sizeBox*i + 2, cyStart + sizeBox*j + 2,
				cxStart + sizeBox*(1 + i) - 2, cyStart + sizeBox*(1 + j) - 2);
			state[i][j] = -1;
		}
		ReleaseDC(hwnd, hdc);
		return true;
	}
	// backward elimination
	if (oneDerection(x, y, 1, 1, &xstart, &ystart, &xend, &yend))
	{
		hdc = GetDC(hwnd);
		for (int i = xstart, j = ystart; j <= yend; j++, i++)
		{
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, cxStart + sizeBox*i + 2, cyStart + sizeBox*j + 2,
				cxStart + sizeBox*(1 + i) - 2, cyStart + sizeBox*(1 + j) - 2);
			state[i][j] = -1;
		}
		ReleaseDC(hwnd, hdc);
		return true;
	}
	return false;
}

void updateScore(HWND hwnd)
{
	static HDC hdc;
	static TCHAR buffer[100];

	hdc = GetDC(hwnd);
	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	Rectangle(hdc, wLine2 + sizeBox, DisArea / 2 - sizeBox / 3, wLine2 + sizeBox * 3, DisArea / 2 + sizeBox / 3);
	Rectangle(hdc, xWin - wLine2 - sizeBox * 3 + 1, DisArea / 2 - sizeBox / 3, xWin - wLine2 - sizeBox + 1, DisArea / 2 + sizeBox / 3);
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(0, 255, 255));
	TextOut(hdc, wLine2 + sizeBox * 3 / 2, DisArea / 2 - sizeBox / 6, buffer, wsprintf(buffer, "%d", goal));
	SetTextColor(hdc, RGB(255, 0, 0));
	TextOut(hdc, xWin - wLine2 - sizeBox * 2 + 1, DisArea / 2 - sizeBox / 6, buffer, wsprintf(buffer, "%d", score));
	SetBkColor(hdc, RGB(230, 230, 230));
	SetTextColor(hdc, RGB(0, 0, 0));
	TextOut(hdc, cxStart + sizeBox*(xDivision / 2 - 2), DisArea / 2 - sizeBox / 6, buffer, wsprintf(buffer, "下组"));
	TextOut(hdc, cxStart + (int)sizeBox*(xDivision / 2 + 2 + 0.3), DisArea / 2 - sizeBox / 6, buffer, wsprintf(buffer, "颜色"));
	ReleaseDC(hwnd, hdc);
	if (score >= goal)
	{
		MessageBox(hwnd, TEXT("You are winner!\n And you can continue to chanllenge!"), TEXT("Congratulations"), MB_ICONINFORMATION);
	}
}

void calculateNext(HWND hwnd)
{
	for (int i = 0; i < 3; i++)
	{
		int t1, t2;
		if (isFull())
		{
			MessageBox(hwnd, TEXT("The boxes are FULL!"), TEXT("FULL TIP"), MB_ICONERROR);
			return;
		}
		t1 = rand() % xDivision;
		t2 = rand() % yDivision;
		while (state[t1][t2] != -1)
		{
			t1 = rand() % xDivision;
			t2 = rand() % yDivision;
		}
		oldNext[i] = next[i];
		nextPoint[i].x = t1;
		nextPoint[i].y = t2;
		state[t1][t2] = next[i];
		next[i] = rand() % 7;
	}
}

void fillNext(HWND hwnd)
{
	static HDC hdc;
	hdc = GetDC(hwnd);
	//draw the next ball
	SelectObject(hdc, pen);
	for (int i = 0; i < 3; i++)
	{
		SelectObject(hdc, colorBrush[next[i]]);
		Ellipse(hdc, cxStart + sizeBox*(xDivision / 2 - 1 + i) + 2, (DisArea - sizeBox) / 2 + 2,
			cxStart + sizeBox*(xDivision / 2 + i) - 4, (DisArea + sizeBox) / 2 - 4);
	}
	ReleaseDC(hwnd, hdc);
}

void findRoute(HWND hwnd, POINT begin, POINT end)
{
	static HDC hdc;
	Node mat[xDivision][yDivision], *t1, *t2;
	Node* q[xDivision*yDivision];
	int count = 0;
	Node *stack[xDivision*yDivision];
	int top = 0;
	bool in[xDivision][yDivision] = { false };
	TCHAR buffer[100];

	for (int i = 0; i < xDivision; i++)
	{
		for (int j = 0; j < yDivision; j++)
		{
			mat[i][j].x = i;
			mat[i][j].y = j;
			mat[i][j].p = NULL;
		}
	}
	bool flag = true;
	q[count++] = &mat[begin.x][begin.y];
	while (flag&&(count>=0))
	{
		t1 = q[0];
		count--;
		for (int i = 0; i < count; i++)
		{
			q[i] = q[i + 1];
		}
		if (!in[t1->x][t1->y])
		{
			if ((t1->x == end.x) && (t1->y == end.y))
			{
				flag = false;
			}
			else
			{
				in[t1->x][t1->y] = true;
				if ((t1->x >= 1) && (state[t1->x - 1][t1->y] == -1) && (!in[t1->x - 1][t1->y]))
				{
					mat[t1->x - 1][t1->y].x = t1->x - 1;
					mat[t1->x - 1][t1->y].y = t1->y;
					mat[t1->x - 1][t1->y].p = t1;
					q[count++] = &mat[t1->x - 1][t1->y];
				}
				if ((t1->x < xDivision - 1) && (state[t1->x + 1][t1->y] == -1) && (!in[t1->x + 1][t1->y]))
				{
					mat[t1->x + 1][t1->y].x = t1->x + 1;
					mat[t1->x + 1][t1->y].y = t1->y;
					mat[t1->x + 1][t1->y].p = t1;
					q[count++] = &mat[t1->x + 1][t1->y];
				}
				if ((t1->y >= 1) && (state[t1->x][t1->y - 1] == -1) && (!in[t1->x][t1->y - 1]))
				{
					mat[t1->x][t1->y - 1].x = t1->x;
					mat[t1->x][t1->y - 1].y = t1->y - 1;
					mat[t1->x][t1->y - 1].p = t1;
					q[count++] = &mat[t1->x][t1->y - 1];
				}
				if ((t1->y < yDivision - 1) && (state[t1->x][t1->y + 1] == -1) && (!in[t1->x][t1->y + 1]))
				{
					mat[t1->x][t1->y + 1].x = t1->x;
					mat[t1->x][t1->y + 1].y = t1->y + 1;
					mat[t1->x][t1->y + 1].p = t1;
					q[count++] = &mat[t1->x][t1->y + 1];
				}
			}
		}

	}

	if (count!=-1)
	{
		hdc = GetDC(hwnd);
		t1 = &(mat[end.x][end.y]);
		while (t1 != NULL)
		{
			stack[top++] = t1;
			t1 = t1->p;
		}
		for (int i = top - 1; i > 0; i--)
		{
			wsprintf(buffer, "(%d, %d) ", stack[i]->x, stack[i]->y);
			OutputDebugString(buffer);
			SelectObject(hdc, brush);
			SelectObject(hdc, pen);
			Rectangle(hdc, cxStart + sizeBox*stack[i]->x + 2, cyStart + sizeBox*stack[i]->y + 2,
				cxStart + sizeBox*(1 + stack[i]->x) - 2, cyStart + sizeBox*(1 + stack[i]->y) - 2);
			SelectObject(hdc, colorBrush[state[stack[top - 1]->x][stack[top - 1]->y]]);
			Ellipse(hdc, cxStart + sizeBox*stack[i - 1]->x + 2, cyStart + sizeBox*stack[i - 1]->y + 2,
				cxStart + sizeBox*(1 + stack[i - 1]->x) - 2, cyStart + sizeBox*(1 + stack[i - 1]->y) - 2);
			Sleep(100);
		}
		state[end.x][end.y] = state[begin.x][begin.y];
		state[begin.x][begin.y] = -1;

		if (!isEliminating(hwnd, end.x, end.y))
		{
			calculateNext(hwnd);
			for (int i = 0; i < 3; i++)
			{
				SelectObject(hdc, brush);
				SelectObject(hdc, pen);
				Rectangle(hdc, cxStart + sizeBox*nextPoint[i].x + 2, cyStart + sizeBox*nextPoint[i].y + 2,
					cxStart + sizeBox*(1 + nextPoint[i].x) - 2, cyStart + sizeBox*(1 + nextPoint[i].y) - 2);
				SelectObject(hdc, colorBrush[oldNext[i]]);
				Ellipse(hdc, cxStart + sizeBox*nextPoint[i].x + 2, cyStart + sizeBox*nextPoint[i].y + 2,
					cxStart + sizeBox*(1 + nextPoint[i].x) - 2, cyStart + sizeBox*(1 + nextPoint[i].y) - 2);
				Sleep(50);
				if (isEliminating(hwnd, nextPoint[i].x, nextPoint[i].y))
				{
					score += 10;
					updateScore(hwnd);
				}
			}
			fillNext(hwnd);
		}
		else
		{
			score += 10;
			updateScore(hwnd);
		}
		

		ReleaseDC(hwnd, hdc);
	}
	
	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	TCHAR buffer[100];
	static bool trigger;
	static bool btnSelected;
	static bool isInitial;
	static POINT selectedPoint, desPoint;
	int t1=0, t2=0;

	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hwnd, id1, time1, NULL);
		for (int i = 0; i < 7; i++)
		{
			colorBrush[i] = CreateSolidBrush(color[i]);
		}
		selectedPoint.x = -1;
		selectedPoint.y = -1;
		initiate();
		MoveWindow(hwnd, 500, 100, xWin + 17, yWin + 40 + 20, true);
		trigger = true;
		btnSelected = false;
		isInitial = true;

		return 0;
	}
		
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case ID_START:
			selectedPoint.x = -1;
			selectedPoint.y = -1;
			initiate();
			trigger = true;
			btnSelected = false;
			isInitial = true;
			InvalidateRect(hwnd, NULL, true);
			break;
		case ID_QUIT:
			PostQuitMessage(0);
			break;
		case ID_HELP:
			MessageBox(hwnd, TEXT("中文名：五子彩色球、五子连珠等（名字很多，貌似这两个最符合本游戏）\n\n一款很小的游戏，规则简单，有点像综合了五子棋和俄罗斯方块，每一轮棋盘上会在随机位置出现3颗棋子，而每一轮你可以移动1颗，无论上下左右或斜着，把同色的棋子5颗连成一线就可以消去（同时该轮就不会产生新棋子），最后当棋子填满棋盘你就输了。游戏意思看似简单，想打高分却不是件容易事。"), TEXT("帮助"), MB_ICONINFORMATION);
			break;
		case ID_ABOUT:
			MessageBox(hwnd, TEXT("这是*天霸的Windows程序设计作业做的一个小玩意，\n可能有一些bug，希望大家指正！"), TEXT("关于"), MB_ICONINFORMATION);
			break;
		default:
			break;
		}
		return 0;
	}

	case WM_LBUTTONDOWN:
		if ((LOWORD(lParam) >= cxStart) && (LOWORD(lParam) <= cxStart + sizeBox*xDivision) 
			&& (HIWORD(lParam) >= cyStart) && (HIWORD(lParam) <= cyStart + sizeBox*yDivision))
		{
			t1 = (LOWORD(lParam) - cxStart) / sizeBox;
			t2 = (HIWORD(lParam) - cyStart) / sizeBox;
			if ((t1 == selectedPoint.x) && (t2 == selectedPoint.y))
			{
				btnSelected ^= 1;
				trigger ^= 1;
			}
			else
			{				
				if (btnSelected)
				{
					if (state[t1][t2] == -1)
					{
						//t1, t2 is destinated point
						//then find the right route to destination
						trigger = false;
						desPoint.x = t1;
						desPoint.y = t2;
						findRoute(hwnd, selectedPoint, desPoint);
						selectedPoint.x = -1;
						selectedPoint.y = -1;
						trigger = true;
						btnSelected = false;
					}
					else
					{
						selectedPoint.x = t1;
						selectedPoint.y = t2;
						btnSelected = true;
						trigger = true;
					}
				}
				else
				{
					if (state[t1][t2] != -1)
					{
						selectedPoint.x = t1;
						selectedPoint.y = t2;
						btnSelected = true;
						trigger = true;
					}
				}
				
			}
			OutputDebugString(TEXT("In WM_LBUTTONDOWN.\n"));
		}
		
		return 0;

	case WM_TIMER:
	{
		if ((selectedPoint.x != -1) && (selectedPoint.y != -1)&&trigger)
		{
			rect.left = cxStart + selectedPoint.x*sizeBox + 2;
			rect.top = cyStart + selectedPoint.y*sizeBox + 2;
			rect.right = cxStart + (selectedPoint.x + 1)*sizeBox - 4;
			rect.bottom = cyStart + (selectedPoint.y + 1)*sizeBox - 4;
			boundBall(hwnd, rect, selectedPoint, btnSelected);
			wsprintf(buffer, "rect=%d %d %d %d \n", rect.left, rect.top, rect.right, rect.bottom);
			OutputDebugString(buffer);
			wsprintf(buffer, "btnSelected= %d\n", btnSelected);
			OutputDebugString(buffer);
		}
		return 0;
	}

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);

		if (isInitial)
		{			
			isInitial = false;
		}
		//draw the outest side of window
		for (int i = 0; i < wLine2; i++)
		{
			SelectObject(hdc, GetStockObject(WHITE_PEN));
			MoveToEx(hdc, i, 0, NULL);
			LineTo(hdc, i, yWin - i);
			SelectObject(hdc, GetStockObject(BLACK_PEN));
			LineTo(hdc, xWin - i, yWin - i);
			SelectObject(hdc, GetStockObject(WHITE_PEN));
			LineTo(hdc, xWin - i, 0);
		}
		//draw the part between displaying area and painting area
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		for (int i = 0; i < wLine3; i++)
		{
			MoveToEx(hdc, wLine2, DisArea + i, NULL);
			LineTo(hdc, xWin - wLine2 + 1, DisArea + i);
		}
		//draw the king and challenger area
		{
			SelectObject(hdc, GetStockObject(DKGRAY_BRUSH));
			SelectObject(hdc, grayPen1);
			Rectangle(hdc, wLine2, DisArea + wLine3, wLine2 + wPerson, yWin - wLine2 + 1);
			Rectangle(hdc, xWin - wLine2 - wPerson + 1, DisArea + wLine3, xWin - wLine2 + 1, yWin - wLine2 + 1);
		}		
		//draw the part ourside painting area
		drawRect(hwnd, cxStart - wLine1 + 1, cyStart - wLine1 + 1, xDivision*sizeBox + wLine1 * 2, yDivision*sizeBox + wLine1 * 2, wLine1, true);
		//draw the displaying area
		{
			drawRect(hwnd, cxStart + sizeBox*(xDivision / 2 - 1), (DisArea - sizeBox) / 2, sizeBox, sizeBox, 1, true);
			drawRect(hwnd, cxStart + sizeBox*(xDivision / 2), (DisArea - sizeBox) / 2, sizeBox, sizeBox, 1, true);
			drawRect(hwnd, cxStart + sizeBox*(xDivision / 2 + 1), (DisArea - sizeBox) / 2, sizeBox, sizeBox, 1, true);
			updateScore(hwnd);
		}
		//draw the boxes
		//draw the ball
		SelectObject(hdc, pen);
		for (int i = 0; i < xDivision; i++)
		{
			for (int j = 0; j < yDivision; j++)
			{
				drawRect(hwnd, cxStart + i*sizeBox, cyStart + j*sizeBox, sizeBox, sizeBox, 2, true);
				if (state[i][j] != -1)
				{
					SelectObject(hdc, colorBrush[state[i][j]]);
					Ellipse(hdc, cxStart + i*sizeBox + 2, cyStart + j*sizeBox + 2, cxStart + (i + 1)*sizeBox - 4, cyStart + (j + 1)*sizeBox - 4);
				}
			}
		}
		//fill the next boxes
		fillNext(hwnd);
		for (int i = 0; i < xDivision; i++)
		{
			for (int j = 0; j < yDivision; j++)
			{
				wsprintf(buffer, "state[%d][%d]= %d\n", i, j, state[i][j]);
				OutputDebugString(buffer);
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	}
		
	case WM_CLOSE:
		if (IDYES==MessageBox(hwnd, TEXT("你真的要忍心离开我吗(T_T)？"), TEXT("Quit"), MB_ICONEXCLAMATION | MB_YESNO))
		{
			SendMessage(hwnd, WM_DESTROY, wParam, lParam);
		}
		return 0;

	case WM_DESTROY:
	{
		for (int i = 0; i < 7; i++)
		{
			DeleteObject(colorBrush[i]);
		}
		DeleteObject(grayPen1);
		DeleteObject(grayPen2);
		DeleteObject(pen);
		DeleteObject(brush);
		DeleteObject(grayBrush);
		PostQuitMessage(0);
		return 0;
	}
		
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}