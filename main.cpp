#include <stdio.h>
#include <graphics.h>//图形库
#include "tools.h"
#include <conio.h>
#include <vector>

using namespace std; //命名空间

#define WINDOW_WIDTH 1012//窗口长度
#define WINDOW_HEIGHT 396//窗口宽度
#define PERSON_NUM 12//人物帧数量
#define BG_NUM 3//背景帧数量
#define OBSTACLE_COUNT 10//障碍物种类

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

typedef enum {//障碍物枚举类型
	TORTOISE,//乌龟 0
	LION,//狮子 1
	OBSTACLE_TYPE_CONUT //2
}obstacle_type;

vector<vector<IMAGE>> obstacleImgs; //存放障碍物的图片

typedef struct obstacle
{
	obstacle_type type; //障碍物的类型
	int imgIndex;//当前显示图片的帧序号
	int x, y;//障碍物坐标
	int speed;
	int power; //伤害力
	bool exist;
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int update;//表示是否需要马上刷新画面

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
	//设置跳跃参数
	jumpstate = 0;
	jumpheightmax=355 - imgperson[0].getheight()-120;
	//设置刷新参数
	update = 1;
	//加载小乌龟素材
	IMAGE imgTort;
	vector<IMAGE>imgTortArray;
	for (int i = 1; i <= 7; i++)
	{
		sprintf_s(name, "res/t%d.png", i);//存乌龟filename
		loadimage(&imgTort, name);
		imgTortArray.push_back(imgTort);
	}
	obstacleImgs.push_back(imgTortArray);
	//加载狮子素材
	IMAGE imgLion;
	vector<IMAGE> imgLionArray;
	for (int i = 1; i <= 6; i++)
	{
		sprintf_s(name, "res/p%d.png", i);//存狮子filename
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);

	//初始化障碍物池
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		obstacles[i].exist = 0;
	}
}

void createObstacle()
{
	int i;
	for (i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist == 0)
		{
			break;
		}
	}
	if (i >= OBSTACLE_COUNT)
	{
		return;
	}
	obstacles[i].exist = 1;
	obstacles[i].imgIndex = 0;
	obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_TYPE_CONUT);
	obstacles[i].x = WINDOW_WIDTH;
	obstacles[i].y = 355 - obstacleImgs[obstacles[i].type][0].getheight();
	if (obstacles[i].type == TORTOISE)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == LION)
	{
		obstacles[i].speed = 4;
		obstacles[i].power = 20;
	}
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

	//障碍物创建与滚动
	static int framecount = 0;//记录当前帧数
	static int enemy_fre;//障碍物产生频率
	static int obs_count = 0;//记录障碍物帧数
	enemy_fre = 100+rand() % 100;
	framecount++;
	obs_count++;
	if (framecount > enemy_fre)
	{
		framecount = 0;
		enemy_fre = 100 + rand() % 100;
		createObstacle();
	}	
	for (int i =0; i< OBSTACLE_COUNT;i++)
	{
		if (obstacles[i].exist)
		{
			obstacles[i].x -= (obstacles[i].speed + bgspeed[2]);
			if (obstacles[i].x < obstacleImgs[obstacles[i].type][0].getwidth() * (-2))
			{
				obstacles[i].exist = 0;
			}
			if (obs_count>=2)//解决障碍物刷新过快的问题.
			{
				obs_count = 0;
				obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % obstacleImgs[obstacles[i].type].size();
			}
		}
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

void update_enemy()
{
	//渲染障碍物
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist)
		{
			putimagePNG2(obstacles[i].x, obstacles[i].y, WINDOW_WIDTH,
				&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}
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
		update = 1;//立即刷新画面
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
	int timer = 0;
	while (1)
	{
		keyEvent();
		timer += getDelay();
		if (timer > 15)
		{
			timer = 0;
			update = 1;
		}
		if (update)
		{
			update = 0;
			BeginBatchDraw();//双缓冲
			updatebg();//渲染游戏背景
			jumpaction();//跳跃
			updateperson(personx, persony);//渲染角色
			update_enemy();//渲染障碍物
			EndBatchDraw();
			bgroll();//背景滚动
		}
	}
	system("pause");
}
