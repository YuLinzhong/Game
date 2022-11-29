#include <stdio.h>
#include <graphics.h>//图形库
#define WINDOW_LENGHTH 1012
#define WINDOW_WIDTH 396
/// <summary>
/// 游戏初始化
/// </summary>
void init() {
	//创建窗口
	initgraph(WINDOW_LENGHTH, WINDOW_WIDTH);

}
/// <summary>
/// 渲染游戏背景（前中后景）
/// </summary>
void createbackground() {

}
int main() {
	init();//初始化
	createbackground;//渲染游戏背景
}
