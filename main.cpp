#include <stdio.h>
#include <graphics.h>//ͼ�ο�
#include "tools.h"

#define WINDOW_LENGHTH 1012
#define WINDOW_WIDTH 396

IMAGE imgbgs[3];//����ȫ�ֱ���
int bgx[3] = { 0 };//����ͼƬx����

/// <summary>
/// ��Ϸ��ʼ��
/// </summary>
void init() {
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

/// <summary>
/// ��Ⱦ��Ϸ������ǰ�к󾰣�
/// </summary>
void createbackground() {
	putimagePNG(bgx[0], 0, &imgbgs[0]);
	putimagePNG(bgx[0], 120, &imgbgs[1]);
	putimagePNG(bgx[0], 330, &imgbgs[2]);
}
int main() {
	init();//��ʼ��
	createbackground();//��Ⱦ��Ϸ����
	system("pause");
}
