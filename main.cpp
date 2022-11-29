#include <stdio.h>
#include <graphics.h>//图形库
#include "tools.h"

#define WINDOW_LENGHTH 1012
#define WINDOW_WIDTH 396

IMAGE imgbgs[3];//背景全局变量
int bgx[3] = { 0 };//背景图片x坐标

/// <summary>
/// 游戏初始化
/// </summary>
void init() {
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

/// <summary>
/// 渲染游戏背景（前中后景）
/// </summary>
void createbackground() {
	putimagePNG(bgx[0], 0, &imgbgs[0]);
	putimagePNG(bgx[0], 120, &imgbgs[1]);
	putimagePNG(bgx[0], 330, &imgbgs[2]);
}
int main() {
	init();//初始化
	createbackground();//渲染游戏背景
	system("pause");
}
