#include <stdio.h>
#include <graphics.h>//ͼ�ο�
#include "tools.h"

#define WINDOW_LENGHTH 1012
#define WINDOW_WIDTH 396

IMAGE imgbgs[3];//����ǰ����ȫ�ֱ���
int bgx[3] = { 0 };//����ͼƬx����
int bgspeed[3] = { 2,3,5 };//����ǰ���������ٶ�

/// <summary>
/// ��Ϸ��ʼ��
/// </summary>
void init() 
{
	//��������
	initgraph(WINDOW_LENGHTH, WINDOW_WIDTH);
	//���ر���
	char name[64] = {};
	for (int i = 0; i < 3; i++)
	{
		sprintf_s(name, "res/bg%03d.png", i + 1);//��filename
		loadimage(&imgbgs[i],name);
	}
}

/// ��Ϸ��������
void bgroll()
{
	for (int i = 0; i < 3; i++)
	{
		bgx[i] -= bgspeed[i];
		if (bgx[i] < -WINDOW_LENGHTH)	
			bgx[i] = 0;
	}
}


/// ��Ⱦ��Ϸ������ǰ�к󾰣�
void createbackground() 
{
	putimagePNG2(bgx[0], 0, &imgbgs[0]);
	putimagePNG2(bgx[1], 120, &imgbgs[1]);
	putimagePNG2(bgx[2], 330, &imgbgs[2]);
}

int main() 
{
	init();//��ʼ��
	while (1)
	{
		BeginBatchDraw();//˫����
		createbackground();//��Ⱦ��Ϸ����
		EndBatchDraw();
		bgroll();
		Sleep(10);
	}
	system("pause");
}
