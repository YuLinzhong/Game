#include <stdio.h>
#include <graphics.h>//ͼ�ο�
#include "tools.h"
#include <conio.h>
#include <vector>

using namespace std; //�����ռ�

#define WINDOW_WIDTH 1012//���ڳ���
#define WINDOW_HEIGHT 396//���ڿ��
#define PERSON_NUM 12//����֡����
#define BG_NUM 3//����֡����
#define OBSTACLE_COUNT 10//�ϰ�������

IMAGE imgbgs[BG_NUM];//����ǰ����ȫ�ֱ���
int bgx[BG_NUM] = { 0 };//����ͼƬx����
int bgspeed[BG_NUM] = { 2,3,5 };//����ǰ���������ٶ�

IMAGE imgperson[PERSON_NUM];//���ﱼ�ܵ�ͼƬȫ�ֱ���
int personx;//��������
int persony;//��������
int personindex;//����֡���

int jumpstate;//��ʾ����������Ծ
int jumpheightmax;//��Ծ��߸߶�
int jumpoff;//��Ծƫ����

int downstate;//��ʾ�������¶׶���
typedef enum {//�ϰ���ö������
	TORTOISE,//�ڹ� 0
	LION,//ʨ�� 1
	HOOK1,//����~
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_CONUT //6
}obstacle_type;

vector<vector<IMAGE>> obstacleImgs; //����ϰ����ͼƬ

typedef struct obstacle
{
	obstacle_type type; //�ϰ��������
	int imgIndex;//��ǰ��ʾͼƬ��֡���
	int x, y;//�ϰ�������
	int speed;
	int power; //�˺���
	bool exist;
	int obs_count;//������ˢ��֡��
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int update;//��ʾ�Ƿ���Ҫ����ˢ�»���

IMAGE imgPersonDown[2];


// ��Ϸ��ʼ��
void init() 
{
	//��������
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	//���ر���ͼƬ
	char name[64] = {};
	for (int i = 0; i < BG_NUM; i++)
	{
		sprintf_s(name, "res/bg%03d.png", i + 1);//�汳��filename
		loadimage(&imgbgs[i], name);
	}
	//���ؽ�ɫͼƬ�����ܣ�
	for (int i = 0; i < PERSON_NUM; i++)
	{
		sprintf_s(name, "res/hero%d.png", i + 1);//������filename
		loadimage(&imgperson[i], name);
	}
	//���������볡ʱ�ĳ�ʼλ��
	personx = WINDOW_WIDTH * 0.5-imgperson[0].getwidth()*0.5;
	persony=355-imgperson[0].getheight();
	personindex = 0;
	//������Ծ����
	jumpstate = 0;
	jumpheightmax=355 - imgperson[0].getheight()-120;
	//����ˢ�²���
	update = 1;
	//����С�ڹ��ز�
	IMAGE imgTort;
	vector<IMAGE>imgTortArray;
	for (int i = 1; i <= 7; i++)
	{
		sprintf_s(name, "res/t%d.png", i);//���ڹ�filename
		loadimage(&imgTort, name);
		imgTortArray.push_back(imgTort);
	}
	obstacleImgs.push_back(imgTortArray);
	//����ʨ���ز�
	IMAGE imgLion;
	vector<IMAGE> imgLionArray;
	for (int i = 1; i <= 6; i++)
	{
		sprintf_s(name, "res/p%d.png", i);//��ʨ��filename
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);

	//��ʼ���ϰ����
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		obstacles[i].exist = 0;
		obstacles[i].obs_count = 0;
	}

	//�����¶��ز�
	for (int i = 1; i <= 2; i++)
	{
		sprintf_s(name, "res/d%d.png", i);//���¶�ͼƬ��filename
		loadimage(&imgPersonDown[i-1], name);
	}
	downstate = 0;

	//�����ϰ�����ĸ�����
	IMAGE imgH;
	for (int i = 0; i < 4; i++)
	{
		vector<IMAGE> imgHookArray;
		sprintf_s(name, "res/h%d.png", i);//�湳��ͼƬ��filename
		loadimage(&imgH, name, 63, 260, true);
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);//�ö�άvector��ͼƬ
	}
}

void createObstacle()
{
	//�����ϰ���
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
	else if (obstacles[i].type >= HOOK1&& obstacles[i].type<=HOOK4)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 20;
		obstacles[i].y = 0;
	}
}
// ��Ϸ�����������ı䱳��x���꣩
void bgroll()
{
	for (int i = 0; i < BG_NUM; i++)
	{
		bgx[i] -= bgspeed[i];
		if (bgx[i] < -WINDOW_WIDTH)	
			bgx[i] = 0;
	}

	//�ϰ��ﴴ�������
	static int framecount = 0;//��¼��ǰ֡��
	static int enemy_fre;//�ϰ������Ƶ��
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
			if (obstacles[i].x < obstacleImgs[obstacles[i].type][0].getwidth() * (-2))//���ϰ����ܳ���Ļʱ
			{
				obstacles[i].exist = 0;//�ı��ϰ���״̬Ϊ������
			}
			obstacles[i].obs_count++;

			if (obstacles[i].obs_count>=2)//����ϰ���ˢ�¹��������
			{
				obstacles[i].obs_count = 0;
				obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % obstacleImgs[obstacles[i].type].size();
			}
		}
	}
}

//ʵ����Ծ����������yֵ
void jumpaction()
{
	if (jumpstate)
	{
		if (persony < jumpheightmax)//��Ϸ��ɫ�Ӵ����
		{
			jumpstate = -1;
		}
		jumpoff = jumpstate * ( (int) ( (jumpheightmax - persony) * 0.04) - 4);//ƫ����
		persony += jumpoff;
		if (persony > 355 - imgperson[0].getheight())//��Ϸ��ɫ�Ӵ�����
		{
			jumpstate = 0;//������Ծ
		}
		//����Ծ��ʱ�򣬲�����index֡���У���Ϊupdateperson����һֱ++������������--����
		personindex--;
		personindex = personindex % 12;
	}
}

// ��Ⱦ��Ϸ������ǰ�к󾰣�
void updatebg() 
{
	putimagePNG2(bgx[0], 0, &imgbgs[0]);
	putimagePNG2(bgx[1], 120, &imgbgs[1]);
	putimagePNG2(bgx[2], 330, &imgbgs[2]);
}

//��Ⱦ����
void updateperson(int personx,int persony)
{
	if (downstate == 0)//���¶׵�ʱ��
	{
		personindex++;
		personindex %= 12;
		putimagePNG2(personx, persony, &imgperson[personindex]);
	}
	else
	{
		static int downcount = 0;//downcount��Ҫ���ڿ���֡��
		int delays[2] = { 5,38 };//delays�ֱ��¼��֡�¶׶����ĳ���ʱ��
		downcount++;
		if (downcount >= delays[personindex])
		{
			downcount = 0;
			personindex++;//���ʱ��Ÿ����¶�
			if (personindex >= 2)//�¶׶�������
			{
				personindex = 0;
				downstate = 0;
			}
		}
		int y = 355 - imgPersonDown[personindex].getheight();//��Ϊ�¶�ʱ����߶ȷ����ı䣬��������ı�һ��yֵ
		putimagePNG2(personx, y,& imgPersonDown[personindex]);
	}
}

void update_enemy()
{
	//��Ⱦ�ϰ���
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist)
		{
			putimagePNG2(obstacles[i].x, obstacles[i].y, WINDOW_WIDTH,
				&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}
}

//��Ծ״̬�ı��
void jump()
{
	jumpstate = 1;
}

//���¶׿���
void down()
{
	downstate = 1;
	personindex = 0;//�¶�ʱ�ȳ�ʼ��index
}
//�����û�����
void keyEvent()
{
	char ch;
	//ע�⣬ֱ����scanf�Ȼ����������������
	if (_kbhit())//����а�������
	{
		update = 1;//����ˢ�»���
		ch=_getch(); //��ȡ�ַ�������Ҫ���س�
		if (ch == ' ')//��Ծ
		{
			jump();
		}
		else if (ch == 's')
		{
			down();
		}
	}
}

int main() 
{
	init();//��ʼ��
	int timer = 0;
	while (1)
	{
		keyEvent();
		timer += getDelay();
		if (timer > 15)//����ˢ��
		{
			timer = 0;
			update = 1;
		}
		if (update)
		{
			update = 0;
			BeginBatchDraw();//˫����
			updatebg();//��Ⱦ��Ϸ����
			jumpaction();//��Ծ
			updateperson(personx, persony);//��Ⱦ��ɫ
			update_enemy();//��Ⱦ�ϰ���
			EndBatchDraw();
			bgroll();//��������
		}
	}
	system("pause");
}
