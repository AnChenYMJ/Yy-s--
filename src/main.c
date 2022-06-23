#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>	//bool用

#define HANG 23			//框的行数
#define LIE  31			//框的列数
#define LEFT 9			//框内部的分割线	左侧
#define RIGHT (LIE-10)	//框内部的分割线	右侧

char g_back[HANG][LIE];	//背景数组
char g_cPlaySqure = 0;	//游戏区方块
char g_cNextSqure = 0;	//提示区next方块
char g_cHANG = 0;		//y 方块在生成时中心点的位置
char g_cLIE  = 0;		//x 相对于游戏区框而言的
char g_cState = 0;		//方块初始状态（即以哪种旋转的角度出现）每次旋转90度，故有4种状态，对应 0 1 2 3 状态变化时+1、+2、+3即可
int g_iScore = 0;		//得分统计

//功能控制
int g_time = 0;			//用于控制下落与按键判断的时间差，使平移（更准确：操作）更丝滑
bool g_pause = false;	//用于控制游戏启动/暂停
bool g_Die = false;		//用于控制 Game Over
int g_speed = 24;		//用于控制 下落速度
bool g_Quit = false;	//用于控制 Esc键退出游戏


//准备函数
void Init(void);						//将主函数中，初始化用到的函数与调用顺序，封装到这里面

//界面相关
void GotoXY(short hang, short lie);		//设置光标位置
void Head(void);						//打印显示界面上方的游戏名
void Frame(void);						//打印最外层框
void LeftInfo(void);					//左侧小框内的提示信息 操作提示
void RightInfo(void);					//右侧小框内的提示信息 得分信息、提示

//初始方块
void StartSqure(void);					//游戏开始，初始化方块（游戏区、提示区）
void NewSqure(void);					//方块有规则生成，方块落地，新产生方块为提示区方块
void InitState(void);					//方块在生成时的初始位置及状态
void DrawNextSqure(void);				//提示区的方块生成打印（next方块）
void Draw(char* pic);					//游戏区方块生成打印

//方块运动
void Down(void);						//方块下落
void Left(void);						//方块左移
void Right(void);						//方块右移
void Change(void);						//下落方块旋转状态切换

//按键输入判断
void KeyState(void);					//按键输入判断

//限制条件
bool CanMove(char state, char hang, char lie);	//边界判断（墙，或者叫框）小bool需要头文件
bool CanChange(char state);						//方块旋转状态限制条件
void StopSqure(void);							//方块变成‘墙’可堆积

//消行与得分
void GetRange(char* low, char* high);	//获取应该消行的判断范围（本案例的方法）两个参数对应方块中心点上方与下方的下标
bool full(char hang);					//判断具体的那一行是否满了
void Score(void);						//消行得分
void disappear(char hang);				//满行清零，goback清零
void Fall(char hang);					//消行后下落
void DrawScore(void);

//游戏结束
bool Die(void);


int main(void)
{
	//随机数种子
	srand((unsigned int)time(NULL));	//用当前系统时间作为随机数种子,种一次就可以了
	//准备工作（注意查看，调用了哪些，有注释的）
	Init();								//准备需要调用的东西全封装到此函数中了

	//方块运动
	while (1)
	{
		Sleep(10);					//10ms睡一下，实现控制方块下落速度 （首字母大写）
		g_time++;		

		if (true == g_Quit)			//退出游戏判断 退出循环即可执行程序结尾，结束程序
			break;

		if (g_time % 6 == 0)		//每60ms判断一次按键
			KeyState();				//按键判断

		if (g_pause == true && g_Die == false)	//暂停判断，这里只是开/关 下落，还未控制其它按键 还有Game Over判断
		{
			if (g_time % g_speed == 0)			//二者时间差就能让操作更流畅，这个思想很好用，↓键的加速也是按照这个倍率，也可以加代码单独控制
				Down();				//下落
		}
		
	}
	

	system("pause>0");	//让命令行结尾的一大串提示不显示
	return 0;
	
}

//准备函数
void Init(void)
{
	//界面相关
	GotoXY(5, 19);		//设置光标位置
	Head();				//游戏名打印
	Frame();			//打印最外层框
	LeftInfo();			//左侧操作提示
	RightInfo();		//右侧得分信息
	printf("测试GotoXY函数的指定坐标功能");
	//初始方块
	StartSqure();		//初始化
	DrawNextSqure();	//提示区的方块生成打印（next）
	Draw("□");			//画初始方块，哪种方块由StartSqure的随机数决定
	//控制相关
	g_pause = false;	//游戏初始时是暂停的，需要按ENTER启动
	g_Die = false;		//Game Over 控制变量一开始是关闭的，等Game Over了再变为true
}
//界面相关
void GotoXY(short hang, short lie)
{
	COORD cd = { lie, hang };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cd);
}
void Head(void)
{
	GotoXY(0, 0);
	printf("\t\t\t\t\t\t**************************\n");
	printf("\t\t\t\t\t\t*                        *\n");
	printf("\t\t\t\t\t\t*       俄罗斯方块       *\n");
	printf("\t\t\t\t\t\t*                        *\n");
	printf("\t\t\t\t\t\t**************************\n");
}
void Frame(void)
{
	int i, j;
	for (i = 0; i < HANG; i++)
	{
		GotoXY(5+i, 30);			//这一步很关键，否则打印的框很奇怪
		for (j = 0; j < LIE; j++)
		{ 
			if (0 == i || i == HANG - 1 || 0 == j || j == LIE - 1 || j == LEFT || j == RIGHT)
			{
				g_back[i][j] = 1;//墙
				printf("※");
			}
			else
			{
				g_back[i][j] = 0;//空白
				printf("  ");	//空的部分需要两个字符位，即两个空格
			}
		}
	}
}
void LeftInfo(void)
{
	int i = 5, j = 31;
	GotoXY(i += 2, j);
	printf("    操作提示：\n");
	GotoXY(i += 2, j);
	printf("  ← → 左右移动\n");
	GotoXY(i += 2, j);
	printf("   ↑   变形\n");
	GotoXY(i += 2, j);
	printf("   ↓   加速\n");
	GotoXY(i += 2, j);
	printf("  Enter 启动/暂停\n");
	GotoXY(i += 2, j);
	printf("  Tab   切换速度\n");
	GotoXY(i += 2, j);
	printf("  当前级别: 1\n");
	GotoXY(i += 2, j);
	printf("  Esc   退出游戏\n");
	GotoXY(i += 2, j);
	printf("  F2    新游戏\n");
}
void RightInfo(void)
{
	int i = 5, j = 77;
	GotoXY(i += 2, j);
	printf("  <score>\n");
	GotoXY(i += 2, j);
	printf("     0\n");
	GotoXY(i += 3, j);
	printf("  <next>\n");
}
//初始方块
void StartSqure(void)
{
	g_cPlaySqure = rand() % 7;	//头文件：stdlib 且需要种子（触发值）
	g_cNextSqure = rand() % 7;
	InitState();				//方块在生成时的初始位置及状态
}
void NewSqure(void)
{
	g_cPlaySqure = g_cNextSqure;	//游戏区新产生方块为next方块
	g_cNextSqure = rand() % 7;		//next方块，重新随机生成
	InitState();					//方块在生成时的初始位置及状态
}
void InitState(void)
{
	//各方块中心生成时的初始位置（相对于顶框）
	g_cHANG = 2;	 
	g_cLIE = LEFT + 6;

	//长条只有一行，所以要单独判断
	if (6 == g_cPlaySqure)
	{
		g_cHANG = 1;
		g_cLIE = LEFT + 7;
	}

	//方块初始状态设置（即以哪种旋转的角度出现）每次旋转90度，故有4种状态，对应 0 1 2 3
	g_cState = 0;	
}
void DrawNextSqure(void)
{
	int hang = 14, lie = 79;

	//残留问题，即重新生成next方块时，新的覆盖旧的，旧的还留着，导致打印的图形是不认识的
	GotoXY(hang, lie-1);	//方块第1行
	printf("        ");	//用空格覆盖掉旧的即可
	GotoXY(hang+1, lie);		//方块第2行
	printf("        ");

	//判断next方块是哪种方块,然后在提示区打印出来（next）
	switch (g_cNextSqure)
	{
	case 0:
		GotoXY(hang, lie);
		printf("■");	//一个方块占2个字符位
		GotoXY(hang+1, lie);
		printf("■■■");
		break;	
	case 1:				
		GotoXY(hang, lie+4);
		printf("■");	
		GotoXY(hang + 1, lie);
		printf("■■■");
		break;
	case 2:
		GotoXY(hang, lie);
		printf("■■");	
		GotoXY(hang + 1, lie+2);
		printf("■■");
		break;
	case 3:
		GotoXY(hang, lie+2);
		printf("■■");
		GotoXY(hang + 1, lie);
		printf("■■");
		break;
	case 4:
		GotoXY(hang, lie + 2);
		printf("■");
		GotoXY(hang + 1, lie);
		printf("■■■");
		break;
	case 5:
		GotoXY(hang, lie);
		printf("■■");
		GotoXY(hang + 1, lie);
		printf("■■");
		break;
	case 6:
		GotoXY(hang, lie-1);
		printf("■■■■");
		break;
	}

}
void Draw(char* pic)
{
	//设定光标初始位置，
	int hang = 5, lie = 30;
	//游戏区方块 判断是哪种方块
	switch (g_cPlaySqure)	//g_cPlaySqure	
	{
	case 0:			//方块0
		switch (g_cState)	//方块初始状态判断（即以哪种旋转的角度出现）	g_cState
		{
		case 0:		//状态0
			GotoXY(hang + g_cHANG, (g_cLIE *2) + lie);			//g_cLIE *2 是因为一个方块或图形占2字符，*2 能更好代表方块位置
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE+ 1) * 2) + lie );	//直接使用g_cHANG、g_cLIE，在InitState中已经给各类型定义好游戏区方块中心点的生成位置了
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 1:		//状态1
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 2:		//状态2
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 3:		//状态3
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		}
		break;
	case 1:			//方块1
		switch (g_cState)
		{
		case 0:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 1:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 2:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 3:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		}
		break;
	case 2:			//方块2
		switch (g_cState)
		{
		case 0:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 1:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 2:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 3:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		}
		break;
	case 3:			//方块3
		switch (g_cState)
		{
		case 0:
			GotoXY(hang + g_cHANG,		(g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG,		((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1,	(g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1,	((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 1:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 2:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 3:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		}
		break;
	case 4:			//方块4
		switch (g_cState)
		{
		case 0:
			GotoXY(hang + g_cHANG,		(g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG,		((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG,		((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1,	(g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 1:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 2:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 3:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		}
		break;
	case 5:			//方块5
		switch (g_cState)
		{
		case 0:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE* 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 1:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 2:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 3:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		}
		break;
	case 6:			//方块6			测试方块6时专用，g_cHANG = 1;
		switch (g_cState)
		{
		case 0:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 2) * 2) + lie);
			printf(pic);
			break;
		case 1:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 2, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 2:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 2) * 2) + lie);
			printf(pic);
			break;
		case 3:
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 2, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		}
		break;
	}
}
//方块运动
void Down(void)
{
	//到底部判断
	if (true == CanMove(g_cState, g_cHANG + 1, g_cLIE))
	{
		Draw("  ");		//更新前，空格清除旧方块图形
		g_cHANG++;		//y轴自加即可，生成方块时统一使用了它
		Draw("□");		//画方块，这里起到更新方块位置作用，哪种方块已经确定好了此时
	}
	//到底部了，产生新方块
	else
	{
		//方块落地变色，成为‘墙’能堆积
		Draw("■");
		StopSqure();
		//消行得分
		Score();
		//判断游戏结束
		if (true == Die())
		{
			//Game Over，暂停所有操作
			g_Die = true;	

			//打印提示
			GotoXY(12, 30 + LEFT * 2 + 5);		//就是调整打印的位置坐标，没啥深意，LEFT * 2是因为 ※ 占2单位字符，LEFT就是为它而产生的计数一样的变量
			printf("*****************\n");
			GotoXY(13, 30 + LEFT * 2 + 5);
			printf("*  Game Over！！*\n");
			GotoXY(14, 30 + LEFT * 2 + 5);
			printf("*  Esc退出游戏  *\n");
			GotoXY(15, 30 + LEFT * 2 + 5);
			printf("* F2开始新游戏  *\n");
			GotoXY(16, 30 + LEFT * 2 + 5);
			printf("*****************\n");
		}
			
		//产生新的方块
		NewSqure();		//改为按照next区的顺序生成方块
		Draw("□");		//打印方块
		DrawNextSqure();//更新打印next区方块
	}
}
void Left(void)
{
	Draw("  ");		//清除旧方块图形
	g_cLIE--;		//左移
	Draw("□");		//更新方块图形
}
void Right(void)
{
	Draw("  ");		//清除旧方块图形
	g_cLIE++;		//右移
	Draw("□");		//更新方块图形
}
void Change(void)
{
	Draw("  ");			//清除旧方块图形
	g_cState++;			//方块旋转状态改变
	if (4 == g_cState)	//让其数值在 0~3 之间
	{
		g_cState = 0;
	}
	Draw("□");			//更新方块图形


	//g_cState %= 4;	//方法二，让其数值在 0~3 之间
						//这种方式有弊端，比如char到256时会回到0，那个边界时刻，如果不是4的话，假如是5，就不是流畅的0~4 切换了
}
//按键输入判断
void KeyState(void)
{
	if (g_pause == true && g_Die == false)		//暂停键判断	并且 Game Over判断
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x01)	// ←
		{
			if (true == CanMove(g_cState, g_cHANG, g_cLIE - 1))		//左移边界判断
				Left();
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x01)	// →
		{
			if (true == CanMove(g_cState, g_cHANG, g_cLIE + 1))
				Right();
		}
		if (GetAsyncKeyState(VK_UP) & 0x01)	// ↑
		{
			if (true == CanChange((g_cState + 1) % 4))
				Change();
		}
		if (GetAsyncKeyState(VK_DOWN))	// ↓
		{
			Down();
		}
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x01)	//ENTER 键	
		g_pause = !g_pause;				//true 与 false 之间切换 初始值在Init 函数里给了

	if (GetAsyncKeyState(VK_F2) & 0x01)		//F2 键   新游戏
		Init();

	if (GetAsyncKeyState(VK_TAB) & 0x01)//tab 键  按位与0x01是特殊功能指定，这个是按下时有作用，不然的话 按下、抬起都有效
	{
		g_speed -= 10;					
		if (4 > g_speed)				//控制下落速度，我分为了3个级别，每次100ms的速度变化(每次多少可控制)
			g_speed = 24;

		int i = 19, j = 43;
		switch (g_speed)				//不知为何，更新数据，会把44那一列的一些其他行的字符变得模糊，我想更新界面但找不到函数
		{								//找到原因了，似乎因为对齐的问题，没有与上面几行的同列对齐，我减1个空格的位置打印，就不会出现刷新问题了
		case 24:
			GotoXY(i, j);
			printf("1");
			break;
		case 14:
			GotoXY(i, j);
			printf("2");
			break;
		case 4:
			GotoXY(i, j);
			printf("3");
			break;
		}
	}

	if (GetAsyncKeyState(VK_ESCAPE) & 0x01)	//ESC 键  退出游戏
	{
		//变量变化
		g_Quit = true;

		//打印提示信息
		GotoXY(12, 30 + LEFT * 2 + 5);		//就是调整打印的位置坐标，没啥深意，LEFT * 2是因为 ※ 占2单位字符，LEFT就是为它而产生的计数一样的变量
		printf("*****************\n");
		GotoXY(13, 30 + LEFT * 2 + 5);
		printf("* 确认游戏关闭  *\n");
		GotoXY(14, 30 + LEFT * 2 + 5);
		printf("* 已经退出游戏  *\n");
		GotoXY(15, 30 + LEFT * 2 + 5);
		printf("* 按任意键关闭  *\n");
		GotoXY(16, 30 + LEFT * 2 + 5);
		printf("*****************\n");
	}
}
//限制条件
bool CanMove(char state, char hang, char lie)
{
	switch (g_cPlaySqure)	//方块种类
	{
	case 0:					//方块0
		switch (state)		//方块旋转状态
		{
		case 0:				//状态0
			if (1 == g_back[hang][lie])			//中心点		不要省略，	方块到底变成‘墙’，如果省略可能出问题，方块可能碰到被省略的小格子
				return false;					//碰到了，返回false
			if (1 == g_back[hang][lie - 1])		//左边小格
				return false;
			if (1 == g_back[hang][lie + 1])		//右边小格
				return false;
			if (1 == g_back[hang - 1][lie - 1])	//左上小格	
				return false;
			break;
		case 1:				//状态1
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			break;
		case 2:				//状态2
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang + 1][lie + 1])
				return false;
			break;
		case 3:				//状态3
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie - 1])
				return false;
			break;
		}
		break;
	case 1:					//方块1
		switch (state)
		{
		case 0:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			break;
		case 1:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie + 1])
				return false;
			break;
		case 2:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang + 1][lie - 1])
				return false;
			break;
		case 3:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			break;
		}
		break;
	case 2:					//方块2
		switch (state)
		{
		case 0:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			break;
		case 1:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			break;
		case 2:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			break;
		case 3:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			break;
		}
		break;
	case 3:					//方块3
		switch (state)
		{
		case 0:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			break;
		case 1:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie + 1])
				return false;
			break;
		case 2:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			break;
		case 3:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie + 1])
				return false;
			break;
		}
		break;
	case 4:					//方块4
		switch (state)
		{
		case 0:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			break;
		case 1:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			break;
		case 2:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			break;
		case 3:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			break;
		}
		break;
	case 5:					//方块5
		switch (state)
		{
		case 0:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			break;
		case 1:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			break;
		case 2:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			break;
		case 3:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie - 1])
				return false;
			break;
		}
		break;
	case 6:					//方块6
		switch (state)
		{
		case 0:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang][lie - 2])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			break;
		case 1:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 2][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			break;
		case 2:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang][lie - 2])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			break;
		case 3:
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 2][lie])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			break;
		}
		break;
	}
	return true;			//没碰到返回true
}
void StopSqure(void)
{
	switch (g_cPlaySqure)	//方块种类
	{
	case 0:
		switch (g_cState)	//方块状态
		{
		case 0:
			g_back[g_cHANG][g_cLIE]			= 1;
			g_back[g_cHANG][g_cLIE - 1]		= 1;
			g_back[g_cHANG][g_cLIE + 1]		= 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE]			= 1;
			g_back[g_cHANG + 1][g_cLIE]		= 1;
			g_back[g_cHANG - 1][g_cLIE]		= 1;
			g_back[g_cHANG - 1][g_cLIE + 1] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE]			= 1;
			g_back[g_cHANG][g_cLIE - 1]		= 1;
			g_back[g_cHANG][g_cLIE + 1]		= 1;
			g_back[g_cHANG + 1][g_cLIE + 1] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE]			= 1;
			g_back[g_cHANG - 1][g_cLIE]		= 1;
			g_back[g_cHANG + 1][g_cLIE]		= 1;
			g_back[g_cHANG + 1][g_cLIE - 1] = 1;
			break;
		}
		break;
	case 1:
		switch (g_cState)
		{
		case 0:
			g_back[g_cHANG][g_cLIE]			= 1;
			g_back[g_cHANG][g_cLIE - 1]		= 1;
			g_back[g_cHANG][g_cLIE + 1]		= 1;
			g_back[g_cHANG - 1][g_cLIE + 1] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE]			= 1;
			g_back[g_cHANG + 1][g_cLIE]		= 1;
			g_back[g_cHANG - 1][g_cLIE]		= 1;
			g_back[g_cHANG + 1][g_cLIE + 1] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG + 1][g_cLIE - 1] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		}
		break;
	case 2:
		switch (g_cState)
		{
		case 0:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE + 1] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE + 1] = 1;
			break;
		}
		break;
	case 3:
		switch (g_cState)
		{
		case 0:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE + 1] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE + 1] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE + 1] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE + 1] = 1;
			break;
		}
		break;
	case 4:
		switch (g_cState)
		{
		case 0:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			break;
		}
		break;
	case 5:
		switch (g_cState)
		{
		case 0:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE - 1] = 1;
			break;
		}
		break;
	case 6:
		switch (g_cState)
		{
		case 0:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG][g_cLIE - 2] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			break;
		case 1:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 2][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			break;
		case 2:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG][g_cLIE - 1] = 1;
			g_back[g_cHANG][g_cLIE - 2] = 1;
			g_back[g_cHANG][g_cLIE + 1] = 1;
			break;
		case 3:
			g_back[g_cHANG][g_cLIE] = 1;
			g_back[g_cHANG - 1][g_cLIE] = 1;
			g_back[g_cHANG - 2][g_cLIE] = 1;
			g_back[g_cHANG + 1][g_cLIE] = 1;
			break;
		}
		break;
	}
}
bool CanChange(char state)
{
	//判断变形后是否与墙有重合
	if (true == CanMove(state, g_cHANG, g_cLIE))	//注意此处用的是传进来的参数state，而不是全局的g_cState，此函数在KeyState中传参调用
		return true;
	//再判断变形后向左移动是否重合
	if (true == CanMove(state, g_cHANG, g_cLIE - 1))
	{
		Draw("  ");		//移动前，要清理原来位置的图形
		g_cLIE -= 1;	//没有重合则真的向左移动1格
		return true;
	}
	//变形后向右移动是否重合
	if (true == CanMove(state, g_cHANG, g_cLIE + 1))
	{
		Draw("  ");
		g_cLIE += 1;
		return true;
	}
	//变形后向下移动是否重合
	if (true == CanMove(state, g_cHANG + 1, g_cLIE))
	{
		Draw("  ");
		g_cHANG += 1;
		return true;
	}
	//方块6的1、3状态特殊，可能要向右移动2格
	if (6 == g_cPlaySqure && 1 == g_cState || 3 == g_cState)
	{
		if (true == CanMove(state, g_cHANG, g_cLIE + 2))
		{
			Draw("  ");
			g_cLIE += 2;
			return true;
		}
	}

	//有重合返回false
	return false;
}
//消行与得分
void GetRange(char* low, char* high)
{
	switch (g_cPlaySqure)	//方块类型
	{
	case 0:					//0 1 4 的方块的消行范围是一样的，请查看那张方块图就能对比知道了
	case 1:
	case 4:
		switch (g_cState)		//方块的旋转状态
		{
		case 0:			
			*low	= g_cHANG - 1;
			*high	= g_cHANG;
			break;
		case 2:
			*low	= g_cHANG;
			*high	= g_cHANG + 1;
			break;
		case 1:					//1 3 状态的消行范围是一样的
		case 3:
			*low	= g_cHANG - 1;
			*high	= g_cHANG + 1;
			break;
		}
		break;
	case 2:					//同理2 3 是一样的消行范围
	case 3:
		switch (g_cState)		
		{
		case 0:
		case 2:
			*low	= g_cHANG - 1;
			*high	= g_cHANG;
			break;
		case 1:			
		case 3:
			*low	= g_cHANG - 1;
			*high	= g_cHANG + 1;
			break;
		}
		break;
	case 5:
		switch (g_cState)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			*low = g_cHANG - 1;
			*high = g_cHANG;
			break;
		}
		break;
	case 6:
		switch (g_cState)
		{
		case 0:
		case 2:
			*low	= g_cHANG;
			*high	= g_cHANG;
			break;
		case 1:
		case 3:
			*low	= g_cHANG - 2;
			*high	= g_cHANG + 1;
			break;
		}
		break;
	}
}
bool full(char hang)
{
	char i;
	char sum = 0;
	for (i = LEFT + 1; i < RIGHT; i++)
	{
		sum += g_back[hang][i];		//如果那一行的 i 列是墙，则此二维数组的对应元素为 1	 sum就是记录这一行的墙的个数，就能判断这行是否满了
	}
	//这行满了返回true
	if (11 == sum)
		return true;
	//没满返回false
	return false;
}
void Score(void)
{
	//获取具体消行范围
	char low = 0, high = 0;;
	GetRange(&low, &high);
	//开始判断范围内的消行是否满足
	char i;
	for (i = low; i <= high; i++)
	{
		if (true == full(i))
		{
			disappear(i);			//消行，gback清零
			Fall(i);				//消行后方块下落
			DrawScore();			//消行后，分数计算，显示分数
		}
	}
}
void disappear(char hang)
{
	int i = 5, j = 30;				//大方框的起始位置
	char n;
	for (n = LEFT + 1; n < RIGHT; n++)
	{
		if (1 == g_back[hang][n])	//把这一行的‘墙’ 清空，注意一个方块俩空格
		{
			g_back[hang][n] = 0;
			GotoXY(hang + i, n * 2 + j);	//指定光标位置，在光标位置打印空格，消除满行
			printf("  ");
			Sleep(40);				//延迟一下就会像动画效果一样，依次消除满行
		}
	}
}
void Fall(char hang)
{
	int i, j;
	int h = 5, l = 30;						//大方框的起始位置
	for (i = hang; i >= 2; i--)				//遍历到下标为2的行为止，就是不要遍历方框顶端
	{
		for (j = LEFT + 1; j < RIGHT; j++)	//左右边界
		{
			if (1 == g_back[i - 1][j])
			{
				//1.数组向下赋值				//就是被消的那排的上一行，向下赋值，取代被消的那行
				g_back[i][j] = g_back[i - 1][j];
				g_back[i - 1][j] = 0;
				//2.画新方快					//此时遍历到的坐标，如果是实心，则画方块，被消的那行消完后已经去残留了，所以只需画实心就好
				GotoXY(i + h, j * 2 + l);	// *2是因为 一个方块水平占2个空格，所以*2 
				printf("■");
				//3.前一个位置去残留			//因为已经取代到了了下一行，所以原先的位置清空，给更上一行下移做准备 （结果就是原本是“  ” 的还是那样，原本是是实心的变成了“  ”）
											//这样自己下移后原本位置就清空了，方便更上一行的下移
				GotoXY(i + h - 1, j * 2 + l);
				printf("  ");
			}
		}
	}
}
void DrawScore(void)
{
	g_iScore++;					//分数增加（或者自己可以控制怎么计算，比如短时间内连续消行，同次多行，则增加的更多）
	int i = 9, j = 80;			//分数显示的坐标	（可自己再加代码让分数居中显示）
	GotoXY(i, j);
	printf("%d", g_iScore*10);	//打印分数,(以10的倍数显示)
}
//游戏结束
bool Die(void)
{
	//此函数在Down函数中调用
	if (1 == g_cHANG || 2 == g_cHANG)
		return true;

	return false;
}
