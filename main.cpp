#include <stdio.h>
#include <graphics.h>//ͼ�ο�
#include "tools.h"
#include <conio.h>

#define WINDOW_WIDTH 1012//���ڳ���
#define WINDOW_HEIGHT 396//���ڿ��
#define PERSON_NUM 12//����֡����
#define BG_NUM 3//����֡����

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

	jumpstate = 0;
	jumpheightmax=355 - imgperson[0].getheight()-120;
	jumpoff = (int)(jumpheightmax - persony) * 0.06 + 2;
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
}

//ʵ����Ծ
void jumpaction()
{
	if (jumpstate)
	{
		if (persony < jumpheightmax)//��Ϸ��ɫ�Ӵ����
		{
			jumpstate = -1;
		}
		jumpoff = jumpstate * ( (int) ( (jumpheightmax - persony) * 0.04) - 4);
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

//��Ⱦ���ﱼ�ܶ���
void updateperson(int personx,int persony)
{
	personindex++;
	personindex %= 12;
	putimagePNG2(personx, persony, &imgperson[personindex]);
}

//��Ծ״̬�ı��
void jump()
{
	jumpstate = 1;
}

//�����û�����
void keyEvent()
{
	char ch;
	//ע�⣬ֱ����scanf�Ȼ����������������
	if (_kbhit())//����а�������
	{
		ch=_getch(); //��ȡ�ַ�������Ҫ���س�
		if (ch == ' ')//��Ծ
		{
			jump();
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
		if (timer > 30)
		{
			timer = 0;
			BeginBatchDraw();//˫����
			updatebg();//��Ⱦ��Ϸ����
			jumpaction();//��Ծ
			updateperson(personx, persony);
			EndBatchDraw();
			bgroll();//��������
		}
	}
	system("pause");
}
