#include <stdio.h>
#include <graphics.h>//图形库
#include "tools.h"
#include <conio.h>

#define WINDOW_WIDTH 1012//窗口长度
#define WINDOW_HEIGHT 396//窗口宽度
#define PERSON_NUM 12//人物帧数量
#define BG_NUM 3//背景帧数量

IMAGE imgbgs[BG_NUM];//后中前背景全局变量
int bgx[BG_NUM] = { 0 };//背景图片x坐标
int bgspeed[BG_NUM] = { 2,3,5 };//后中前背景滚动速度

IMAGE imgperson[PERSON_NUM];//人物奔跑的图片全局变量
int personx;//人物坐标
int persony;//人物坐标
int personindex;//人物帧序号

int jumpstate;//表示人物正在跳跃
int jumpheightmax;//跳跃最高高度
int jumpoff;//跳跃偏移量
// 游戏初始化
void init() 
{
	//创建窗口
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	//加载背景图片
	char name[64] = {};
	for (int i = 0; i < BG_NUM; i++)
	{
		sprintf_s(name, "res/bg%03d.png", i + 1);//存背景filename
		loadimage(&imgbgs[i], name);
	}
	//加载角色图片（奔跑）
	for (int i = 0; i < PERSON_NUM; i++)
	{
		sprintf_s(name, "res/hero%d.png", i + 1);//存人物filename
		loadimage(&imgperson[i], name);
	}
	//设置人物入场时的初始位置
	personx = WINDOW_WIDTH * 0.5-imgperson[0].getwidth()*0.5;
	persony=355-imgperson[0].getheight();
	personindex = 0;

	jumpstate = 0;
	jumpheightmax=355 - imgperson[0].getheight()-120;
	jumpoff = (int)(jumpheightmax - persony) * 0.06 + 2;
}

// 游戏背景滚动（改变背景x坐标）
void bgroll()
{
	for (int i = 0; i < BG_NUM; i++)
	{
		bgx[i] -= bgspeed[i];
		if (bgx[i] < -WINDOW_WIDTH)	
			bgx[i] = 0;
	}
}

//实现跳跃
void jumpaction()
{
	if (jumpstate)
	{
		if (persony < jumpheightmax)//游戏角色接触天空
		{
			jumpstate = -1;
		}
		jumpoff = jumpstate * ( (int) ( (jumpheightmax - persony) * 0.04) - 4);
		persony += jumpoff;
		if (persony > 355 - imgperson[0].getheight())//游戏角色接触地面
		{
			jumpstate = 0;//结束跳跃
		}
		//当跳跃的时候，不更新index帧序列，因为updateperson函数一直++，所以这里用--抵消
		personindex--;
		personindex = personindex % 12;
	}
}

// 渲染游戏背景（前中后景）
void updatebg() 
{
	putimagePNG2(bgx[0], 0, &imgbgs[0]);
	putimagePNG2(bgx[1], 120, &imgbgs[1]);
	putimagePNG2(bgx[2], 330, &imgbgs[2]);
}

//渲染人物奔跑动画
void updateperson(int personx,int persony)
{
	personindex++;
	personindex %= 12;
	putimagePNG2(personx, persony, &imgperson[personindex]);
}



//跳跃状态的变更
void jump()
{
	jumpstate = 1;
}

//处理用户输入
void keyEvent()
{
	char ch;
	//注意，直接用scanf等会造成阻塞程序运行
	if (_kbhit())//如果有按键按下
	{
		ch=_getch(); //读取字符而不需要按回车
		if (ch == ' ')//跳跃
		{
			jump();
		}
	}

}

int main() 
{
	init();//初始化
	while (1)
	{
		keyEvent();

		BeginBatchDraw();//双缓冲
		updatebg();//渲染游戏背景
		jumpaction();//跳跃
		updateperson(personx, persony);
		EndBatchDraw();
		bgroll();//背景滚动
		Sleep(10);
	}
	system("pause");
}
