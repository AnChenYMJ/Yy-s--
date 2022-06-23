#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>	//bool��

#define HANG 23			//�������
#define LIE  31			//�������
#define LEFT 9			//���ڲ��ķָ���	���
#define RIGHT (LIE-10)	//���ڲ��ķָ���	�Ҳ�

char g_back[HANG][LIE];	//��������
char g_cPlaySqure = 0;	//��Ϸ������
char g_cNextSqure = 0;	//��ʾ��next����
char g_cHANG = 0;		//y ����������ʱ���ĵ��λ��
char g_cLIE  = 0;		//x �������Ϸ������Ե�
char g_cState = 0;		//�����ʼ״̬������������ת�ĽǶȳ��֣�ÿ����ת90�ȣ�����4��״̬����Ӧ 0 1 2 3 ״̬�仯ʱ+1��+2��+3����
int g_iScore = 0;		//�÷�ͳ��

//���ܿ���
int g_time = 0;			//���ڿ��������밴���жϵ�ʱ��ʹƽ�ƣ���׼ȷ����������˿��
bool g_pause = false;	//���ڿ�����Ϸ����/��ͣ
bool g_Die = false;		//���ڿ��� Game Over
int g_speed = 24;		//���ڿ��� �����ٶ�
bool g_Quit = false;	//���ڿ��� Esc���˳���Ϸ


//׼������
void Init(void);						//���������У���ʼ���õ��ĺ��������˳�򣬷�װ��������

//�������
void GotoXY(short hang, short lie);		//���ù��λ��
void Head(void);						//��ӡ��ʾ�����Ϸ�����Ϸ��
void Frame(void);						//��ӡ������
void LeftInfo(void);					//���С���ڵ���ʾ��Ϣ ������ʾ
void RightInfo(void);					//�Ҳ�С���ڵ���ʾ��Ϣ �÷���Ϣ����ʾ

//��ʼ����
void StartSqure(void);					//��Ϸ��ʼ����ʼ�����飨��Ϸ������ʾ����
void NewSqure(void);					//�����й������ɣ�������أ��²�������Ϊ��ʾ������
void InitState(void);					//����������ʱ�ĳ�ʼλ�ü�״̬
void DrawNextSqure(void);				//��ʾ���ķ������ɴ�ӡ��next���飩
void Draw(char* pic);					//��Ϸ���������ɴ�ӡ

//�����˶�
void Down(void);						//��������
void Left(void);						//��������
void Right(void);						//��������
void Change(void);						//���䷽����ת״̬�л�

//���������ж�
void KeyState(void);					//���������ж�

//��������
bool CanMove(char state, char hang, char lie);	//�߽��жϣ�ǽ�����߽п�Сbool��Ҫͷ�ļ�
bool CanChange(char state);						//������ת״̬��������
void StopSqure(void);							//�����ɡ�ǽ���ɶѻ�

//������÷�
void GetRange(char* low, char* high);	//��ȡӦ�����е��жϷ�Χ���������ķ���������������Ӧ�������ĵ��Ϸ����·����±�
bool full(char hang);					//�жϾ������һ���Ƿ�����
void Score(void);						//���е÷�
void disappear(char hang);				//�������㣬goback����
void Fall(char hang);					//���к�����
void DrawScore(void);

//��Ϸ����
bool Die(void);


int main(void)
{
	//���������
	srand((unsigned int)time(NULL));	//�õ�ǰϵͳʱ����Ϊ���������,��һ�ξͿ�����
	//׼��������ע��鿴����������Щ����ע�͵ģ�
	Init();								//׼����Ҫ���õĶ���ȫ��װ���˺�������

	//�����˶�
	while (1)
	{
		Sleep(10);					//10ms˯һ�£�ʵ�ֿ��Ʒ��������ٶ� ������ĸ��д��
		g_time++;		

		if (true == g_Quit)			//�˳���Ϸ�ж� �˳�ѭ������ִ�г����β����������
			break;

		if (g_time % 6 == 0)		//ÿ60ms�ж�һ�ΰ���
			KeyState();				//�����ж�

		if (g_pause == true && g_Die == false)	//��ͣ�жϣ�����ֻ�ǿ�/�� ���䣬��δ������������ ����Game Over�ж�
		{
			if (g_time % g_speed == 0)			//����ʱ�������ò��������������˼��ܺ��ã������ļ���Ҳ�ǰ���������ʣ�Ҳ���ԼӴ��뵥������
				Down();				//����
		}
		
	}
	

	system("pause>0");	//�������н�β��һ����ʾ����ʾ
	return 0;
	
}

//׼������
void Init(void)
{
	//�������
	GotoXY(5, 19);		//���ù��λ��
	Head();				//��Ϸ����ӡ
	Frame();			//��ӡ������
	LeftInfo();			//��������ʾ
	RightInfo();		//�Ҳ�÷���Ϣ
	printf("����GotoXY������ָ�����깦��");
	//��ʼ����
	StartSqure();		//��ʼ��
	DrawNextSqure();	//��ʾ���ķ������ɴ�ӡ��next��
	Draw("��");			//����ʼ���飬���ַ�����StartSqure�����������
	//�������
	g_pause = false;	//��Ϸ��ʼʱ����ͣ�ģ���Ҫ��ENTER����
	g_Die = false;		//Game Over ���Ʊ���һ��ʼ�ǹرյģ���Game Over���ٱ�Ϊtrue
}
//�������
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
	printf("\t\t\t\t\t\t*       ����˹����       *\n");
	printf("\t\t\t\t\t\t*                        *\n");
	printf("\t\t\t\t\t\t**************************\n");
}
void Frame(void)
{
	int i, j;
	for (i = 0; i < HANG; i++)
	{
		GotoXY(5+i, 30);			//��һ���ܹؼ��������ӡ�Ŀ�����
		for (j = 0; j < LIE; j++)
		{ 
			if (0 == i || i == HANG - 1 || 0 == j || j == LIE - 1 || j == LEFT || j == RIGHT)
			{
				g_back[i][j] = 1;//ǽ
				printf("��");
			}
			else
			{
				g_back[i][j] = 0;//�հ�
				printf("  ");	//�յĲ�����Ҫ�����ַ�λ���������ո�
			}
		}
	}
}
void LeftInfo(void)
{
	int i = 5, j = 31;
	GotoXY(i += 2, j);
	printf("    ������ʾ��\n");
	GotoXY(i += 2, j);
	printf("  �� �� �����ƶ�\n");
	GotoXY(i += 2, j);
	printf("   ��   ����\n");
	GotoXY(i += 2, j);
	printf("   ��   ����\n");
	GotoXY(i += 2, j);
	printf("  Enter ����/��ͣ\n");
	GotoXY(i += 2, j);
	printf("  Tab   �л��ٶ�\n");
	GotoXY(i += 2, j);
	printf("  ��ǰ����: 1\n");
	GotoXY(i += 2, j);
	printf("  Esc   �˳���Ϸ\n");
	GotoXY(i += 2, j);
	printf("  F2    ����Ϸ\n");
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
//��ʼ����
void StartSqure(void)
{
	g_cPlaySqure = rand() % 7;	//ͷ�ļ���stdlib ����Ҫ���ӣ�����ֵ��
	g_cNextSqure = rand() % 7;
	InitState();				//����������ʱ�ĳ�ʼλ�ü�״̬
}
void NewSqure(void)
{
	g_cPlaySqure = g_cNextSqure;	//��Ϸ���²�������Ϊnext����
	g_cNextSqure = rand() % 7;		//next���飬�����������
	InitState();					//����������ʱ�ĳ�ʼλ�ü�״̬
}
void InitState(void)
{
	//��������������ʱ�ĳ�ʼλ�ã�����ڶ���
	g_cHANG = 2;	 
	g_cLIE = LEFT + 6;

	//����ֻ��һ�У�����Ҫ�����ж�
	if (6 == g_cPlaySqure)
	{
		g_cHANG = 1;
		g_cLIE = LEFT + 7;
	}

	//�����ʼ״̬���ã�����������ת�ĽǶȳ��֣�ÿ����ת90�ȣ�����4��״̬����Ӧ 0 1 2 3
	g_cState = 0;	
}
void DrawNextSqure(void)
{
	int hang = 14, lie = 79;

	//�������⣬����������next����ʱ���µĸ��Ǿɵģ��ɵĻ����ţ����´�ӡ��ͼ���ǲ���ʶ��
	GotoXY(hang, lie-1);	//�����1��
	printf("        ");	//�ÿո񸲸ǵ��ɵļ���
	GotoXY(hang+1, lie);		//�����2��
	printf("        ");

	//�ж�next���������ַ���,Ȼ������ʾ����ӡ������next��
	switch (g_cNextSqure)
	{
	case 0:
		GotoXY(hang, lie);
		printf("��");	//һ������ռ2���ַ�λ
		GotoXY(hang+1, lie);
		printf("������");
		break;	
	case 1:				
		GotoXY(hang, lie+4);
		printf("��");	
		GotoXY(hang + 1, lie);
		printf("������");
		break;
	case 2:
		GotoXY(hang, lie);
		printf("����");	
		GotoXY(hang + 1, lie+2);
		printf("����");
		break;
	case 3:
		GotoXY(hang, lie+2);
		printf("����");
		GotoXY(hang + 1, lie);
		printf("����");
		break;
	case 4:
		GotoXY(hang, lie + 2);
		printf("��");
		GotoXY(hang + 1, lie);
		printf("������");
		break;
	case 5:
		GotoXY(hang, lie);
		printf("����");
		GotoXY(hang + 1, lie);
		printf("����");
		break;
	case 6:
		GotoXY(hang, lie-1);
		printf("��������");
		break;
	}

}
void Draw(char* pic)
{
	//�趨����ʼλ�ã�
	int hang = 5, lie = 30;
	//��Ϸ������ �ж������ַ���
	switch (g_cPlaySqure)	//g_cPlaySqure	
	{
	case 0:			//����0
		switch (g_cState)	//�����ʼ״̬�жϣ�����������ת�ĽǶȳ��֣�	g_cState
		{
		case 0:		//״̬0
			GotoXY(hang + g_cHANG, (g_cLIE *2) + lie);			//g_cLIE *2 ����Ϊһ�������ͼ��ռ2�ַ���*2 �ܸ��ô�����λ��
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE+ 1) * 2) + lie );	//ֱ��ʹ��g_cHANG��g_cLIE����InitState���Ѿ��������Ͷ������Ϸ���������ĵ������λ����
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			break;
		case 1:		//״̬1
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG - 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, (g_cLIE * 2) + lie);
			printf(pic);
			break;
		case 2:		//״̬2
			GotoXY(hang + g_cHANG, (g_cLIE * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG, ((g_cLIE - 1) * 2) + lie);
			printf(pic);
			GotoXY(hang + g_cHANG + 1, ((g_cLIE + 1) * 2) + lie);
			printf(pic);
			break;
		case 3:		//״̬3
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
	case 1:			//����1
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
	case 2:			//����2
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
	case 3:			//����3
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
	case 4:			//����4
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
	case 5:			//����5
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
	case 6:			//����6			���Է���6ʱר�ã�g_cHANG = 1;
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
//�����˶�
void Down(void)
{
	//���ײ��ж�
	if (true == CanMove(g_cState, g_cHANG + 1, g_cLIE))
	{
		Draw("  ");		//����ǰ���ո�����ɷ���ͼ��
		g_cHANG++;		//y���ԼӼ��ɣ����ɷ���ʱͳһʹ������
		Draw("��");		//�����飬�����𵽸��·���λ�����ã����ַ����Ѿ�ȷ�����˴�ʱ
	}
	//���ײ��ˣ������·���
	else
	{
		//������ر�ɫ����Ϊ��ǽ���ܶѻ�
		Draw("��");
		StopSqure();
		//���е÷�
		Score();
		//�ж���Ϸ����
		if (true == Die())
		{
			//Game Over����ͣ���в���
			g_Die = true;	

			//��ӡ��ʾ
			GotoXY(12, 30 + LEFT * 2 + 5);		//���ǵ�����ӡ��λ�����꣬ûɶ���⣬LEFT * 2����Ϊ �� ռ2��λ�ַ���LEFT����Ϊ���������ļ���һ���ı���
			printf("*****************\n");
			GotoXY(13, 30 + LEFT * 2 + 5);
			printf("*  Game Over����*\n");
			GotoXY(14, 30 + LEFT * 2 + 5);
			printf("*  Esc�˳���Ϸ  *\n");
			GotoXY(15, 30 + LEFT * 2 + 5);
			printf("* F2��ʼ����Ϸ  *\n");
			GotoXY(16, 30 + LEFT * 2 + 5);
			printf("*****************\n");
		}
			
		//�����µķ���
		NewSqure();		//��Ϊ����next����˳�����ɷ���
		Draw("��");		//��ӡ����
		DrawNextSqure();//���´�ӡnext������
	}
}
void Left(void)
{
	Draw("  ");		//����ɷ���ͼ��
	g_cLIE--;		//����
	Draw("��");		//���·���ͼ��
}
void Right(void)
{
	Draw("  ");		//����ɷ���ͼ��
	g_cLIE++;		//����
	Draw("��");		//���·���ͼ��
}
void Change(void)
{
	Draw("  ");			//����ɷ���ͼ��
	g_cState++;			//������ת״̬�ı�
	if (4 == g_cState)	//������ֵ�� 0~3 ֮��
	{
		g_cState = 0;
	}
	Draw("��");			//���·���ͼ��


	//g_cState %= 4;	//��������������ֵ�� 0~3 ֮��
						//���ַ�ʽ�б׶ˣ�����char��256ʱ��ص�0���Ǹ��߽�ʱ�̣��������4�Ļ���������5���Ͳ���������0~4 �л���
}
//���������ж�
void KeyState(void)
{
	if (g_pause == true && g_Die == false)		//��ͣ���ж�	���� Game Over�ж�
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x01)	// ��
		{
			if (true == CanMove(g_cState, g_cHANG, g_cLIE - 1))		//���Ʊ߽��ж�
				Left();
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x01)	// ��
		{
			if (true == CanMove(g_cState, g_cHANG, g_cLIE + 1))
				Right();
		}
		if (GetAsyncKeyState(VK_UP) & 0x01)	// ��
		{
			if (true == CanChange((g_cState + 1) % 4))
				Change();
		}
		if (GetAsyncKeyState(VK_DOWN))	// ��
		{
			Down();
		}
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x01)	//ENTER ��	
		g_pause = !g_pause;				//true �� false ֮���л� ��ʼֵ��Init ���������

	if (GetAsyncKeyState(VK_F2) & 0x01)		//F2 ��   ����Ϸ
		Init();

	if (GetAsyncKeyState(VK_TAB) & 0x01)//tab ��  ��λ��0x01�����⹦��ָ��������ǰ���ʱ�����ã���Ȼ�Ļ� ���¡�̧����Ч
	{
		g_speed -= 10;					
		if (4 > g_speed)				//���������ٶȣ��ҷ�Ϊ��3������ÿ��100ms���ٶȱ仯(ÿ�ζ��ٿɿ���)
			g_speed = 24;

		int i = 19, j = 43;
		switch (g_speed)				//��֪Ϊ�Σ��������ݣ����44��һ�е�һЩ�����е��ַ����ģ����������½��浫�Ҳ�������
		{								//�ҵ�ԭ���ˣ��ƺ���Ϊ��������⣬û�������漸�е�ͬ�ж��룬�Ҽ�1���ո��λ�ô�ӡ���Ͳ������ˢ��������
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

	if (GetAsyncKeyState(VK_ESCAPE) & 0x01)	//ESC ��  �˳���Ϸ
	{
		//�����仯
		g_Quit = true;

		//��ӡ��ʾ��Ϣ
		GotoXY(12, 30 + LEFT * 2 + 5);		//���ǵ�����ӡ��λ�����꣬ûɶ���⣬LEFT * 2����Ϊ �� ռ2��λ�ַ���LEFT����Ϊ���������ļ���һ���ı���
		printf("*****************\n");
		GotoXY(13, 30 + LEFT * 2 + 5);
		printf("* ȷ����Ϸ�ر�  *\n");
		GotoXY(14, 30 + LEFT * 2 + 5);
		printf("* �Ѿ��˳���Ϸ  *\n");
		GotoXY(15, 30 + LEFT * 2 + 5);
		printf("* ��������ر�  *\n");
		GotoXY(16, 30 + LEFT * 2 + 5);
		printf("*****************\n");
	}
}
//��������
bool CanMove(char state, char hang, char lie)
{
	switch (g_cPlaySqure)	//��������
	{
	case 0:					//����0
		switch (state)		//������ת״̬
		{
		case 0:				//״̬0
			if (1 == g_back[hang][lie])			//���ĵ�		��Ҫʡ�ԣ�	���鵽�ױ�ɡ�ǽ�������ʡ�Կ��ܳ����⣬�������������ʡ�Ե�С����
				return false;					//�����ˣ�����false
			if (1 == g_back[hang][lie - 1])		//���С��
				return false;
			if (1 == g_back[hang][lie + 1])		//�ұ�С��
				return false;
			if (1 == g_back[hang - 1][lie - 1])	//����С��	
				return false;
			break;
		case 1:				//״̬1
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang - 1][lie])
				return false;
			if (1 == g_back[hang - 1][lie + 1])
				return false;
			if (1 == g_back[hang + 1][lie])
				return false;
			break;
		case 2:				//״̬2
			if (1 == g_back[hang][lie])
				return false;
			if (1 == g_back[hang][lie + 1])
				return false;
			if (1 == g_back[hang][lie - 1])
				return false;
			if (1 == g_back[hang + 1][lie + 1])
				return false;
			break;
		case 3:				//״̬3
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
	case 1:					//����1
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
	case 2:					//����2
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
	case 3:					//����3
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
	case 4:					//����4
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
	case 5:					//����5
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
	case 6:					//����6
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
	return true;			//û��������true
}
void StopSqure(void)
{
	switch (g_cPlaySqure)	//��������
	{
	case 0:
		switch (g_cState)	//����״̬
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
	//�жϱ��κ��Ƿ���ǽ���غ�
	if (true == CanMove(state, g_cHANG, g_cLIE))	//ע��˴��õ��Ǵ������Ĳ���state��������ȫ�ֵ�g_cState���˺�����KeyState�д��ε���
		return true;
	//���жϱ��κ������ƶ��Ƿ��غ�
	if (true == CanMove(state, g_cHANG, g_cLIE - 1))
	{
		Draw("  ");		//�ƶ�ǰ��Ҫ����ԭ��λ�õ�ͼ��
		g_cLIE -= 1;	//û���غ�����������ƶ�1��
		return true;
	}
	//���κ������ƶ��Ƿ��غ�
	if (true == CanMove(state, g_cHANG, g_cLIE + 1))
	{
		Draw("  ");
		g_cLIE += 1;
		return true;
	}
	//���κ������ƶ��Ƿ��غ�
	if (true == CanMove(state, g_cHANG + 1, g_cLIE))
	{
		Draw("  ");
		g_cHANG += 1;
		return true;
	}
	//����6��1��3״̬���⣬����Ҫ�����ƶ�2��
	if (6 == g_cPlaySqure && 1 == g_cState || 3 == g_cState)
	{
		if (true == CanMove(state, g_cHANG, g_cLIE + 2))
		{
			Draw("  ");
			g_cLIE += 2;
			return true;
		}
	}

	//���غϷ���false
	return false;
}
//������÷�
void GetRange(char* low, char* high)
{
	switch (g_cPlaySqure)	//��������
	{
	case 0:					//0 1 4 �ķ�������з�Χ��һ���ģ���鿴���ŷ���ͼ���ܶԱ�֪����
	case 1:
	case 4:
		switch (g_cState)		//�������ת״̬
		{
		case 0:			
			*low	= g_cHANG - 1;
			*high	= g_cHANG;
			break;
		case 2:
			*low	= g_cHANG;
			*high	= g_cHANG + 1;
			break;
		case 1:					//1 3 ״̬�����з�Χ��һ����
		case 3:
			*low	= g_cHANG - 1;
			*high	= g_cHANG + 1;
			break;
		}
		break;
	case 2:					//ͬ��2 3 ��һ�������з�Χ
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
		sum += g_back[hang][i];		//�����һ�е� i ����ǽ����˶�ά����Ķ�ӦԪ��Ϊ 1	 sum���Ǽ�¼��һ�е�ǽ�ĸ����������ж������Ƿ�����
	}
	//�������˷���true
	if (11 == sum)
		return true;
	//û������false
	return false;
}
void Score(void)
{
	//��ȡ�������з�Χ
	char low = 0, high = 0;;
	GetRange(&low, &high);
	//��ʼ�жϷ�Χ�ڵ������Ƿ�����
	char i;
	for (i = low; i <= high; i++)
	{
		if (true == full(i))
		{
			disappear(i);			//���У�gback����
			Fall(i);				//���к󷽿�����
			DrawScore();			//���к󣬷������㣬��ʾ����
		}
	}
}
void disappear(char hang)
{
	int i = 5, j = 30;				//�󷽿����ʼλ��
	char n;
	for (n = LEFT + 1; n < RIGHT; n++)
	{
		if (1 == g_back[hang][n])	//����һ�еġ�ǽ�� ��գ�ע��һ���������ո�
		{
			g_back[hang][n] = 0;
			GotoXY(hang + i, n * 2 + j);	//ָ�����λ�ã��ڹ��λ�ô�ӡ�ո���������
			printf("  ");
			Sleep(40);				//�ӳ�һ�¾ͻ��񶯻�Ч��һ����������������
		}
	}
}
void Fall(char hang)
{
	int i, j;
	int h = 5, l = 30;						//�󷽿����ʼλ��
	for (i = hang; i >= 2; i--)				//�������±�Ϊ2����Ϊֹ�����ǲ�Ҫ�������򶥶�
	{
		for (j = LEFT + 1; j < RIGHT; j++)	//���ұ߽�
		{
			if (1 == g_back[i - 1][j])
			{
				//1.�������¸�ֵ				//���Ǳ��������ŵ���һ�У����¸�ֵ��ȡ������������
				g_back[i][j] = g_back[i - 1][j];
				g_back[i - 1][j] = 0;
				//2.���·���					//��ʱ�����������꣬�����ʵ�ģ��򻭷��飬����������������Ѿ�ȥ�����ˣ�����ֻ�軭ʵ�ľͺ�
				GotoXY(i + h, j * 2 + l);	// *2����Ϊ һ������ˮƽռ2���ո�����*2 
				printf("��");
				//3.ǰһ��λ��ȥ����			//��Ϊ�Ѿ�ȡ����������һ�У�����ԭ�ȵ�λ����գ�������һ��������׼�� ���������ԭ���ǡ�  �� �Ļ���������ԭ������ʵ�ĵı���ˡ�  ����
											//�����Լ����ƺ�ԭ��λ�þ�����ˣ��������һ�е�����
				GotoXY(i + h - 1, j * 2 + l);
				printf("  ");
			}
		}
	}
}
void DrawScore(void)
{
	g_iScore++;					//�������ӣ������Լ����Կ�����ô���㣬�����ʱ�����������У�ͬ�ζ��У������ӵĸ��ࣩ
	int i = 9, j = 80;			//������ʾ������	�����Լ��ټӴ����÷���������ʾ��
	GotoXY(i, j);
	printf("%d", g_iScore*10);	//��ӡ����,(��10�ı�����ʾ)
}
//��Ϸ����
bool Die(void)
{
	//�˺�����Down�����е���
	if (1 == g_cHANG || 2 == g_cHANG)
		return true;

	return false;
}
