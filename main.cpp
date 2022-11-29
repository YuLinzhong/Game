#include <stdio.h>
#include <graphics.h>//图形库
#include "tools.h"

#define WINDOW_LENGHTH 1012
#define WINDOW_WIDTH 396

IMAGE imgbgs[3];//后中前背景全局变量
int bgx[3] = { 0 };//背景图片x坐标
int bgspeed[3] = { 2,3,5 };//后中前背景滚动速度

/// <summary>
/// 游戏初始化
/// </summary>
void init() 
{
	//创建窗口
	initgraph(WINDOW_LENGHTH, WINDOW_WIDTH);
	//加载背景
	char name[64] = {};
	for (int i = 0; i < 3; i++)
	{
		sprintf_s(name, "res/bg%03d.png", i + 1);//存filename
		loadimage(&imgbgs[i],name);
	}
}

/// 游戏背景滚动
void bgroll()
{
	for (int i = 0; i < 3; i++)
	{
		bgx[i] -= bgspeed[i];
		if (bgx[i] < -WINDOW_LENGHTH)	
			bgx[i] = 0;
	}
}


/// 渲染游戏背景（前中后景）
void createbackground() 
{
	putimagePNG2(bgx[0], 0, &imgbgs[0]);
	putimagePNG2(bgx[1], 120, &imgbgs[1]);
	putimagePNG2(bgx[2], 330, &imgbgs[2]);
}

int main() 
{
	init();//初始化
	while (1)
	{
		BeginBatchDraw();//双缓冲
		createbackground();//渲染游戏背景
		EndBatchDraw();
		bgroll();
		Sleep(10);
	}
	system("pause");
}
