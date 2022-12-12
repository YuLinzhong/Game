#include <stdio.h>
#include <graphics.h>//图形库
#include "tools.h"
#include <conio.h>
#include <vector>
#pragma warning(disable :  4996)

using namespace std; //命名空间

#define WIN_SCORE 100//100分胜利

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
int personblood;//人物血量
int score;//分数

int jumpstate;//表示人物正在跳跃
int jumpheightmax;//跳跃最高高度
int jumpoff;//跳跃偏移量

int downstate;//表示人物在下蹲动作

IMAGE imgSZ[10];


typedef enum {//障碍物枚举类型
	TORTOISE,//乌龟 0
	LION,//狮子 1
	HOOK1,//钩子~
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_CONUT //6
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
	int obs_count;//控制其刷新帧率
	bool hited;//表示是否已经发生碰撞
	bool passed;//表示是否已经被通过
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int update;//表示是否需要马上刷新画面

IMAGE imgPersonDown[2];

//排名
void ranks()
{
	FILE* f;
	f = fopen("score.txt", "r+");
	fscanf_s(f, "%*[^\n]");//跳过第一行文字
	int i = 0;
	int sc[100];
	while (!feof(f))
	{
		fscanf_s(f, "%d", &sc[i]);
		i++;
	}
	int rank=1;
	for (int j = 0; j < i; j++)
	{
		if (sc[j] > score) rank++;
	}
	printf("您的排名为：%d", rank);
	fprintf_s(f,"%d\n", score);
	fclose(f);
}

// 游戏初始化
void init() 
{
	//创建窗口
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT,EW_SHOWCONSOLE);
	//initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
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
		obstacles[i].obs_count = 0;
	}

	//加载下蹲素材
	for (int i = 1; i <= 2; i++)
	{
		sprintf_s(name, "res/d%d.png", i);//存下蹲图片的filename
		loadimage(&imgPersonDown[i-1], name);
	}
	downstate = 0;

	//加载障碍物——四个钩子
	IMAGE imgH;
	for (int i = 0; i < 4; i++)
	{
		vector<IMAGE> imgHookArray;
		sprintf_s(name, "res/h%d.png", i);//存钩子图片的filename
		loadimage(&imgH, name, 63, 260, true);
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);//用二维vector存图片
	}

	//初始化血量
	personblood = 100;
	//预加载碰撞音效
	preLoadSound("res/hit.mp3");

	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);//播放背景音乐

	score = 0;//初始得分置为0

	//加载数字图片
	for (int i = 0; i < 10; i++)
	{
		sprintf_s(name, "res/num/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}

void createObstacle()
{
	//创建障碍物
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
	obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_TYPE_CONUT);//这一个变量决定障碍物的类型
	obstacles[i].x = WINDOW_WIDTH;
	obstacles[i].y = 355 - obstacleImgs[obstacles[i].type][0].getheight();
	obstacles[i].hited = false;
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
	else if (obstacles[i].type >= HOOK1&& obstacles[i].type<=HOOK4)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 20;
		obstacles[i].y = 0;
	}

	obstacles[i].passed = false;
}

void checkHit()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)//分别检测每个障碍物与玩家是否碰撞即可
	{
		if (obstacles[i].exist&&obstacles[i].hited==false)
		{
			//分别计算各图片的左上角坐标和右上角坐标
			int a1x, a1y, a2x, a2y;
			int off = 30;//碰撞误差，小于30px时不算碰撞
			if (!downstate)//如果不是下蹲状态
			{
				a1x = personx + off;
				a1y = persony + off;
				a2x = personx + imgperson[personindex].getwidth() - off;
				a2y = persony + imgperson[personindex].getheight();
			}
			else//下蹲状态
			{
				a1x = personx + off;
				a1y = 355 - imgperson[personindex].getheight()+100;
				a2x = personx + imgPersonDown[personindex].getwidth() - off;
				a2y = persony + imgPersonDown[personindex].getheight();
				
			}
			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
			int b1x = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x + img.getwidth() - off;
			int b2y = obstacles[i].y+ img.getheight()- 10;

			//判断是否碰撞
			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y))
			{
				personblood -= obstacles[i].power;//减血
				printf("血量剩余 %d\n", personblood);
				
				playSound("res/hit.mp3");
				obstacles[i].hited = 1;
			}
		}
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
	enemy_fre = 100+rand() % 100;
	framecount++;
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
			if (obstacles[i].x < obstacleImgs[obstacles[i].type][0].getwidth() * (-2))//当障碍物跑出屏幕时
			{
				obstacles[i].exist = 0;//改变障碍物状态为不存在
			}
			obstacles[i].obs_count++;

			if (obstacles[i].obs_count>=2)//解决障碍物刷新过快的问题
			{
				obstacles[i].obs_count = 0;
				obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % obstacleImgs[obstacles[i].type].size();
			}
		}
	}

	//碰撞检测
	checkHit();
}

//实现跳跃，更改人物y值
void jumpaction()
{
	if (jumpstate)
	{
		if (persony < jumpheightmax)//游戏角色接触天空
		{
			jumpstate = -1;
		}
		jumpoff = jumpstate * ( (int) ( (jumpheightmax - persony) * 0.04) - 4);//偏移量
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

//渲染人物
void updateperson(int personx,int persony)
{
	if (downstate == 0)//不下蹲的时候
	{
		personindex++;
		personindex %= 12;
		putimagePNG2(personx, persony, &imgperson[personindex]);
	}
	else
	{
		static int downcount = 0;//downcount主要用于控制帧率
		int delays[2] = { 5,38 };//delays分别记录两帧下蹲动作的持续时间
		downcount++;
		if (downcount >= delays[personindex])
		{
			downcount = 0;
			personindex++;//这个时候才更新下蹲
			if (personindex >= 2)//下蹲动作结束
			{
				personindex = 0;
				downstate = 0;
			}
		}
		int y = 355 - imgPersonDown[personindex].getheight();//因为下蹲时人物高度发生改变，所以这里改变一下y值
		putimagePNG2(personx, y,& imgPersonDown[personindex]);
	}
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
	downstate = 0;
}

//打开下蹲开关
void down()
{
	downstate = 1;
	jumpstate = 0;
	personindex = 0;//下蹲时先初始化index
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
		if (ch == 'w'||ch=='W')//跳跃
		{
			jump();
		}
		else if (ch == 's'||ch=='S')
		{
			down();
		}
	}
}

//渲染血条
void updatebloodbar()
{
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, personblood / 100.0);//血条绘制
}

//检查游戏结束
void checkover()
{
	if (personblood <= 0)
	{
		loadimage(0, "res/over.png");
		FlushBatchDraw();//刷新缓冲
		mciSendString("stop res/bg.mp3", 0, 0, 0);//关闭背景音乐
		ranks();
		system("pause");//暂停
		//按任意键继续游戏
		personblood = 100;//血加满
		score = 0;//分数清零
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}

//检查得分
void checkscore()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist&&//判断障碍物存在
			obstacles[i].passed==false&&//判断是否通过
			obstacles[i].hited==false&&//判断是否撞击
			obstacles[i].x+obstacleImgs[obstacles[i].type][0].getwidth()<personx)//判断x坐标位置
		{
			score++;
			obstacles[i].passed = true;
			printf("score:%d\n", score);
		}
	}
}

//渲染得分
void updatescore()
{
	char str[8];
	sprintf_s(str, "%d", score);//将数字转化为字符串

	int x = 20;
	int y = 25;

	for (int i = 0; str[i]; i++)
	{
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth()+5;
	}
}

//检查胜利
void checkwin()
{
	if (score >= WIN_SCORE)
	{
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);//播放胜利音乐
		Sleep(2000);
		loadimage(0, "res/win.png");//加载胜利画面
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);//停止背景音乐
		ranks();
		system("pause");

		//重新初始化血量和得分，播放音乐
		personblood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}


int main() 
{
	init();//初始化

	//显示初始画面
	loadimage(0, "res/over.png");
	system("pause");//暂停

	int timer = 0;
	while (1)
	{
		keyEvent();
		timer += getDelay();
		if (timer > 15)//控制刷新
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
			updatebloodbar();//渲染血条
			updatescore();//渲染得分
			checkwin();//检查是否胜利
			EndBatchDraw();

			checkover();//检查游戏是否结束
			checkscore();//检查得分
			bgroll();//背景滚动
		}
	}
	system("pause");
}
