/*
	口袋妖怪
模块：
	1.人物移动（包括 场景变换，行走动画）                    完成
		---》其他npc的移动；                                 初步完成
	2.地图碰撞区（墙壁 树木 河湖 山脉）                      局部完成
	3.游戏菜单  （包括 欢迎界面，背包，精灵列表）
		---》背包											 初步完成
		---》菜单											 初步完成
		---》精灵列表										 初步完成
		---> 开始界面
		---》训练师名片
	4.事件系统  （包括 触发剧情，引起打斗，对话）
		---> 触发对战										 完成
		---》对话（文本考虑用文件方式只读）					 完成
		---》随机遇敌
	5.战斗系统  （场景绘制，技能选择，精灵状态变化，背包道具数量变化，技能动画） 未完成(最大难点）
		---》场景绘制										 部分完成
		---》技能											!!!!!!
		---》逃跑											初步完成
	6.音乐（背景音乐，战斗音乐，技能音效)					大部分完成
	7.存档系统
	8.道具系统												初步完成
*/
//对战时信息播放速度有待优化调整

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream> 
#include <sstream>	
#include <graphics.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <mmsystem.h>
#include "Item.h"
#include "Skill.h"
#include "Pokemon.h"
#include "Object.h"
#include "Npc.h"
#pragma comment(lib,"Winmm.lib")
using namespace std;

#define cUP 1
#define cLF 2
#define cDN 3
#define cRT 4

#define RUN -2
#define LOSE -1
#define VICTOR 0


void startup();													//初始化参数
void main_mapstart();											//main地图初始化
void npc_strat();												//main地图角色初始化
void updata();													//刷新画面
void npcmove(Npc&);												//npc随机移动
void scanINPUT();												//检查输入
void cantmove(Npc,bool[]);										//障碍物判断
void interface_switch();										//界面切换动画
template <class T, class S> int  CalculateDIS(T n1, S n2);		//计算距离
template <class T> void darwmsg(T);								//显示一个对话框
//--------------------菜单------------------
void MenuSys();
void MenuShow();
void MenuInput();
//--------------------背包-------------------
void BagSys();
void BagShow();
void BagInput();
void BagUse();
//-----------------名片-------------------
void CardSys();
void CardShow();
void CardMsgInput();
//-------------------战斗--------------------
void battlesys(Npc&);				//战斗系统
void battleshow(Npc&);				//绘制战斗场景
void battlestart(Npc&);					//开场动画及初始化
void battlefinish(Npc&);			//战斗结束动画
void battleinput(Npc&);				//战斗时输入检测
void battleSuccess(Npc&);			//战斗胜利动画
void battleLose();					//战斗失败效果
void battlemusic_open();			//战斗相关音乐打开
void battlemusic_close();			//………………关闭
void battlemenu();					//战斗时的菜单
void playerAttack(Npc&,int);		//玩家攻击动画及效果
void playerRun(Npc&);				//逃跑
void emyAttack(Npc&);				//敌人进攻
void attackflush(Npc);				//攻击动画
int is_over(Npc&);

IMAGE ground;			//背景
IMAGE msgbk[10];		//对话框 (0:战斗 1：对话 2:菜单）
IMAGE bottom[10];		//按钮 （0：攻击 1:指示器）
IMAGE bag[3];			//三个背包
IMAGE PKlist[5];		//0：背景图，1：选择框，2：选择框掩码，3：大框，4：大框掩码图；
IMAGE statebk[10];		//战斗时的技能窗口  （0：火 1：水）
IMAGE battle;			//战斗画面


int mapSIZEx, mapSIZEy;				//地图大小
int mapX, mapY;						//地图坐标
int cover_glass[1000][1000];		//地图
/*menu*/
int pointX, pointY;					//菜单指示器
int menu_close;						//控制关闭菜单
/*pklist*/
int pklist_close;					//控制关闭列表
int pki;							//精灵列表的序号
/*bag*/
int bag_close;						//控制关闭背包
bool onBattle;						//是否在战斗中打开背包
int bagi;							//当前背包页面序号
int codei;							//当前选中的物品序号
int codemin, codemax;				//背包当前页面中最小物品序号和最大物品序号
int point2X, point2Y;				//背包指示器
/*battle*/
int battle_close;					//控制战斗结束
int turn;							//-1：玩家，1：敌人
int point3X, point3Y;				//战斗菜单的指示器
int playerflush;					//控制玩家攻击动画
int emyflush;						//控制敌人攻击动画


void main_mapstart()
{
	mapSIZEx = 990;
	mapSIZEy = 926;
	mapX = 0;
	mapY = -mapSIZEy / 2;
	for (int i = 0; i < mapSIZEy; i++) {          //-1:障碍物  -2：物件
		for (int j = 0; j < mapSIZEx; j++) {
			if (j >= 151 && j <= 281 && i >= 568 && i <= 658)	//t1
				cover_glass[i][j] = -1;
			if (j >= 280 && j <= 416 && i >= 628 && i <= 640)	//k1 
				cover_glass[i][j] = -1;
			if (j >= 240 && j <= 260 && i >= 668 && i <= 678)	//b1
				cover_glass[i][j] = -2;
		}
	}
}
void startup()
{
	initgraph(495, 463);

	//初始化
	main_mapstart();
	skill_startup();
	pokemon_start();
	npc_strat();

	//播放背景音乐
	mciSendString(_T("open test\\backmic.mp3 alias backmusic"), NULL, 0, NULL);
	mciSendString(_T("play backmusic repeat"), NULL, 0, NULL);
	mciSendString("open test\\chose.mp3 alias chosemic", 0, 0, 0);
	mciSendString("open test\\升级.mp3 alias levelupmic", 0, 0, 0);
	mciSendString("open test\\撞墙.mp3 alias movemic", 0, 0, 0);

	Item hp_potion;      //回血药

	BeginBatchDraw();

	loadimage(&ground, "test\\glass.png");
	loadimage(&role1.pic, "test\\ROLE.png");
	loadimage(&role1.picB, "test\\ROLEblack.png");
	loadimage(&role1.battlepic, "test\\ROLEfight.png");
	loadimage(&role1.battlepicB, "test\\ROLEfightblack.png");
	loadimage(&npc[1].picB, "test\\npc2black.png");
	loadimage(&npc[1].pic, "test\\npc2.png");
	loadimage(&npc[1].battlepic, "test\\npc1fight.png");
	loadimage(&npc[1].battlepicB, "test\\npc1fightblack.png");
	loadimage(&hp_potion.pic, "test\\hppotion.png");
	loadimage(&battle, "test\\battle1.png");
	loadimage(&msgbk[0], "test\\msgbk.png");		// 0 ：战斗的对话框
	loadimage(&msgbk[1], "test\\msgtalk.png");		// 1：与npc对话
	loadimage(&msgbk[2], "test\\menu.png");			//2：菜单
	loadimage(&statebk[1], "test\\fire.png");		//状态窗口
	loadimage(&statebk[2], "test\\water.png");		//状态窗口
	loadimage(&bottom[0], "test\\bt_attack.png");	//攻击按钮
	loadimage(&bottom[1], "test\\point.png");		//指示器
	loadimage(&bottom[2], "test\\bt_bag.png");
	loadimage(&bottom[3], "test\\bt_pokemon.png");
	loadimage(&bottom[4], "test\\bt_run.png");
	loadimage(&bottom[5], "test\\bt_point.png");
	loadimage(&bag[0], "test\\bag0.png");
	loadimage(&bag[1], "test\\bag1.png");
	loadimage(&bag[2], "test\\bag2.png");
	loadimage(&PKlist[0], "test\\pklist.png");
	loadimage(&PKlist[1], "test\\小框.png");
	loadimage(&PKlist[2], "test\\小框B.png");
	loadimage(&PKlist[3], "test\\大框.png");
	loadimage(&PKlist[4], "test\\大框B.png");


	hp_potion.count = 5;
	hp_potion.type = HPPOTION;
	hp_potion.name = "伤药";
	hp_potion.size = 30;
	role1.item.push_back(hp_potion);
	hp_potion.count = 2;
	npc[1].item.push_back(hp_potion);

	board[1].x = 250;//246;
	board[1].y = 210;//668;
	board[1].text = "board1text.txt";
}
void cantmove(Npc role, bool can[])
{
	if (cover_glass[role1.y - mapY + 10][role1.x - mapX] != 0)
		can[cDN] = 0;
	if (cover_glass[role1.y - mapY - 10][role1.x - mapX] != 0)
		can[cUP] = 0;
	if (cover_glass[role1.y - mapY][role1.x - mapX + 10] != 0)
		can[cRT] = 0;
	if (cover_glass[role1.y - mapY][role1.x - mapX - 10] != 0)
		can[cLF] = 0;
}
template <class T, class S>
int CalculateDIS(T n1, S n2)   //计算对象间的距离
{
	int x = n1.x - n2.x;
	int y = n1.y - n2.y;
	int dis = (int)sqrt(x * x + y * y);
	return dis;
}
void interface_switch()								//界面切换动画
{
	int i, j;
	i = 247, j = 0;                                 //从正上方开始画
	setlinecolor(BLACK);							//画线颜色设置为黑色
	while (!(i == 246 && j == 0))
	{
		line(247, 231, i, j);
		FlushBatchDraw();
		if (i == 640)
			j++;
		if (i == 0)
			j--;
		if (j == 600)
			i--;
		if (j == 0)
			i++;
	}
}
void playmic(string name, string odds = "")
{
	string ply = "play " + name + " " + odds;
	string sek = "seek " + name + " to start";
	mciSendString(sek.c_str(), 0, 0, 0);
	mciSendString(ply.c_str(), 0, 0, 0);
}
void stopmic(string name)
{
	string stp = "stop " + name;
	mciSendString(stp.c_str(), 0, 0, 0);
}
void updata()
{
	cleardevice();
	//播放动画
	putimage(mapX, mapY, &ground);
	//role1
	putimage(role1.x, role1.y, role1.sizex, role1.sizey, &role1.picB, role1.sizex * role1.state, 0, NOTSRCERASE);  //原图白底，掩码图黑底
	putimage(role1.x, role1.y, role1.sizex, role1.sizey, &role1.pic, role1.sizex * role1.state, 0, SRCINVERT);
	//npc[1]
	putimage(npc[1].x, npc[1].y, npc[1].sizex, npc[1].sizey, &npc[1].picB, npc[1].sizex * npc[1].state, 0, NOTSRCERASE);  //原图白底，掩码图黑底
	putimage(npc[1].x, npc[1].y, npc[1].sizex, npc[1].sizey, &npc[1].pic, npc[1].sizex * npc[1].state, 0, SRCINVERT);

	FlushBatchDraw();
}
void npcmove(Npc& people)
{
	//随机移动
	srand(time(0));
	int range = 1 - (-1) + 1;   // 移动频率范围【-1，1】；
	int val = rand() % range + -1;
	srand(time(0));
	int f = rand() % 5;  //随机方向；
	if (f == 0) {
		people.x += val;
		//改变运动状态
		if (val > 0)
			people.state = people.Ri;
		else
			people.state = people.Li;
		if (people.Ri++ == 5) people.Ri = 3;
		if (people.Li++ == 11) people.Li = 9;
	}
	if (f == 1) {
		people.y -= val;
		if (val < 0)
			people.state = people.Di;
		else
			people.state = people.Ui;
		if (people.Ui++ == 8) people.Ui = 6;
		if (people.Di++ == 2) people.Di = 1;
	}
}

void Settxt(int color = BLACK, int hight = 0, int weight = 400, int width = 0)
{
	LOGFONT f;
	settextcolor(color);
	gettextstyle(&f);					// 获取当前字体设置
	f.lfHeight = hight;						// 设置字体高度为
	f.lfWeight = weight;					//字体粗细
	f.lfWidth = width;                     //设置宽度
	_tcscpy(f.lfFaceName, _T("微软雅黑"));    // 设置字体
	//f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);                     // 应用字体样式
	setbkmode(TRANSPARENT);
}
//*****************pokmonlist**************
void PKlistshow()
{
	int y1 = 17;    //17  +60  
	int y2 = 19;	//20  +60
	char lv[10];
	char hp[10];
	Settxt(WHITE);
	putimage(0, 0, &PKlist[0]);   //背景
	/*首发*/
	putimage(2 + (pki == 0) * 2, 39, 173, 144, &PKlist[4], (pki == 0) * 173, 0, NOTSRCERASE);   //大框
	putimage(2 + (pki == 0) * 2, 39, 173, 144, &PKlist[3], (pki == 0) * 173, 0, SRCINVERT);
	putimage(33, 67, &role1.theMON[0].piclistB, NOTSRCERASE);   //首发精灵
	putimage(33, 67, &role1.theMON[0].piclist, SRCINVERT);
	sprintf(lv, "%2d", role1.theMON[0].level);
	sprintf(hp, "%3d  %3d", role1.theMON[0].life, role1.theMON[0].max_life);
	outtextxy(105, 97, lv);
	outtextxy(92, 130, hp);
	/*其余五只*/
	for (int i = 1; i < role1.theMON.size(); i++, y1 += 60, y2 += 60) {
		sprintf(lv, "%2d", role1.theMON[i].level);
		sprintf(hp, "%3d  %3d", role1.theMON[i].life, role1.theMON[i].max_life);
		putimage(176, y2, 300, 48, &PKlist[2], 0, (pki == i) * 48, NOTSRCERASE);
		putimage(176, y2, 300, 48, &PKlist[1], 0, (pki == i) * 48, SRCINVERT);
		putimage(212, y1 + (pki == i) * 2, &role1.theMON[i].piclistB, NOTSRCERASE);
		putimage(212, y1 + (pki == i) * 2, &role1.theMON[i].piclist, SRCINVERT);
		outtextxy(280, y2 + 26, lv);
		outtextxy(393, y2 + 26, hp);
	}
	FlushBatchDraw();
}
void PKlistinput()
{
	if (_kbhit()) {
		playmic("chosemic");
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			(pki > 0) ? pki-- : 0;
		}
		else if (::GetAsyncKeyState(VK_DOWN) & 0x8000) {
			(pki < role1.theMON.size() - 1) ? pki++ : 0;
		}
		else if (::GetAsyncKeyState(VK_LEFT) & 0x8000) {
			pki = 0;
		}
		else if (::GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			pki = 1;
		}
		else if (GetAsyncKeyState(0x41) & 0x8000) {  //A

		}
		else if (GetAsyncKeyState(0x44) & 0x8000) { //D
			pklist_close = 0;
			playmic("openmic");
		}
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
void PKlistsys()
{
	pklist_close = 1;
	cleardevice();
	Resize(0, 480, 322);
	while (pklist_close) {
		PKlistshow();
		PKlistinput();
	}
	Resize(0, 495, 463);
	updata();
}
//*****************bagsys******************
void BagUse()
{
	Settxt(WHITE);
	int flag = 1;
	string outxt;
	if (codei < role1.item.size()) {
		Item& potion = role1.item[codei];
		if (potion.count > 0) {
			char s[30];
			int t1 = 0, t2 = 0;
			flag = potion.count;
			role1.theMON[role1.useNo].use(potion);
			if (potion.count < flag) flag = 0;
			t1 = role1.theMON[role1.useNo].life;
			t2 = role1.theMON[role1.useNo].max_life;
			if (t1 == t2) {
				if (!flag) {
					strcpy(s, "已经完全回复");
					playmic("covermic");
				}
				else
					strcpy(s, "不需要回复");
			}
			else {
				sprintf(s, "回复了%d点", potion.size);
				playmic("covermic");
			}
			outxt = role1.theMON[role1.useNo].name + "的" + potion.typestr() + s;
		}
		else {
			outxt = "已经没有剩余了";
		}
		outtextxy(100, 360, outxt.c_str());
		FlushBatchDraw();
		Sleep(1100);
		if (onBattle && !flag) {
			bag_close = 0;
			turn = 1;
		}
	}
}
void BagInput()
{
	if (_kbhit()) {
		playmic("chosemic");
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			if (point2Y > 45)point2Y -= 45;
			if (codei > codemin) codei--;
		}
		else if (::GetAsyncKeyState(VK_DOWN) & 0x8000) {
			if (point2Y < 265)point2Y += 45;
			if (codei < codemax - 1) codei++;
		}
		else if (::GetAsyncKeyState(VK_LEFT) & 0x8000) {
			if (bagi > 0)
				bagi--;
			if (codemin > 0) {
				codemax -= 6;
				codemin -= 6;
				point2Y = 40;
				codei = codemin;
			}
		}
		else if (::GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			if (bagi < 2)
				bagi++;
			if (codemax < 18) {
				codemax += 6;
				codemin += 6;
				point2Y = 40;
				codei = codemin;
			}
		}
		else if (GetAsyncKeyState(0x41) & 0x8000) {						//A键
			playmic("chosemic");
			BagUse();
		}
		else if (GetAsyncKeyState(0x44) & 0x8000) {   //D键退出
			stopmic("chosemic");
			playmic("openmic");
			bag_close = 0;
		}
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
void BagShow()
{
	putimage(0, 0, &bag[bagi]);
	putimage(point2X, point2Y, &bottom[1]);
	Settxt(BLACK);
	int y = 40;
	char s[10];
	for (int i = codemin; i < min(codemax, role1.item.size()); i++, y += 45) {
		sprintf(s, "X%d", role1.item[i].count);
		outtextxy(220, y, role1.item[i].name.c_str());
		outtextxy(420, y, s);
	}
	if (codei < role1.item.size())
		putimage(23, 380, &role1.item[codei].pic);
	FlushBatchDraw();
}
void BagSys()
{
	if (!onBattle) mciSendString("open test\\回复.mp3 alias covermic", 0, 0, 0);
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	bag_close = 1;
	bagi = 0;
	codei = 0;
	codemin = 0;
	codemax = 6;
	point2X = 200;
	point2Y = 40;
	cleardevice();
	while (bag_close) {
		BagShow();
		BagInput();
	}
	if (!onBattle) mciSendString("close covermic", 0, 0, 0);
}
//*****************menu*****************
void MenuShow()
{
	putimage(355, 50, &msgbk[2]);
	putimage(pointX, pointY, &bottom[1]);
	Settxt(BLACK);
	outtextxy(385, 70, "背包");
	outtextxy(385, 90, "精灵");
	FlushBatchDraw();
}
void MenuInput()
{
	if (_kbhit()) {
		playmic("chosemic");
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			if (pointY > 70)
				pointY -= 20;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			if (pointY < 310)
				pointY += 20;
		}
		else if (GetAsyncKeyState(0x41) & 0x8000) {
			playmic("chosemic");
			switch (pointY)
			{
			case 73://第一个选项
				BagSys();
				updata();
				break;
			case 93://第二个选项
				PKlistsys();
				break;
			}
		}
		else if (GetAsyncKeyState(0x44) & 0x8000) {
			menu_close = 0;  //再按一次打开菜单就退出菜单
		}
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
void MenuSys()
{
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	menu_close = 1;
	pointX = 365;
	pointY = 73;
	mciSendString("open test\\open.mp3 alias openmic", 0, 0, 0);
	playmic("openmic");

	while (menu_close) {
		MenuShow();
		MenuInput();
	}
	mciSendString("close openmic", 0, 0, 0);
	Sleep(200);
}
//——————————————————————
void battleSuccess(Npc& emy)
{
	char s[100];
	/*经验获得*/
	int rate = emy.theMON[emy.useNo].level - role1.theMON[role1.useNo].level;
	rate = (rate < 0) ? 0 : rate;
	const int getexp = rate * 8 + 10;
	sprintf(s, "获得%d点经验", getexp);
	putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
	outtextxy(40, 300, s);
	FlushBatchDraw();
	Sleep(500);
	//升级判定
	if (getexp + role1.theMON[role1.useNo].EXP >= role1.theMON[role1.useNo].need_EXP) {
		role1.theMON[role1.useNo].levelup();
		playmic("levelupmic");
		int muchexp = getexp + role1.theMON[role1.useNo].EXP - role1.theMON[role1.useNo].need_EXP;
		muchexp = (muchexp < 0) ? 0 : muchexp;
		role1.theMON[role1.useNo].EXP += muchexp;
		/*升级信息*/
		putimage(0, 279, &msgbk[0]);		//覆盖之前的画面
		outtextxy(40, 300, "等级提升了！");
		FlushBatchDraw();
		Sleep(1000);
	}
	/*金钱获得*/
	const int getmoney = emy.money / 2;
	role1.money += getmoney;
	sprintf(s, "从对手那边拿到了%dG", getmoney);
	putimage(0, 279, &msgbk[0]);			//覆盖之前的画面
	outtextxy(40, 300, s);
	FlushBatchDraw();
	Sleep(500);
	system("pause");
}
void battleLose()
{
	/*失去金钱*/
	char s[100] = "";
	const int losemoney = role1.money * 0.2;
	role1.money -= losemoney;
	sprintf(s, "失去了%dG", losemoney);
	putimage(0, 279, &msgbk[0]);			//覆盖之前的画面
	outtextxy(40, 300, s);
	FlushBatchDraw();
	Sleep(500);
	system("pause");
	/*传送回治疗中心*/
}
void attackflush(Npc emy)
{
	//进攻动画
	if (turn == -2)
		playerflush = 15;
	else if (turn == 1)
		emyflush = 15;
	int tmp = turn;
	turn = 0;
	battleshow(emy);
	Sleep(150);
	battleshow(emy);
	turn = (tmp == -2) ? 1 : -1;
	//
}
void playerAttack(Npc& emy,int no)
{
	if (no >= role1.theMON[role1.useNo].skill.size())
		return;
	Skill skl = role1.theMON[role1.useNo].skill[no];
	string s = "你使用了" + skl.name;
	putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
	outtextxy(40, 300, s.c_str());
	FlushBatchDraw();
	Sleep(600);
	double r = 1;   //伤害倍数；
	srand(time(NULL));
	//命中的判定
	if (3 == rand() % 10) {
		putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
		outtextxy(40, 300, "没有命中");
		FlushBatchDraw();
		Sleep(600);
	}
	else {
		attackflush(emy);
		playmic("attackmic");
		//属性克制的判定
		putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
		if (skl > emy.theMON[emy.useNo].property) {
			outtextxy(40, 300, "效果拔群");
			r *= 1.5;
		}
		else if (skl < emy.theMON[emy.useNo].property) {
			outtextxy(40, 300, "效果微弱");
			r *= 0.5;
		}
		else
			outtextxy(40, 300, "效果一般");
		FlushBatchDraw();
		Sleep(600);
		//命中要害的判定
		srand(time(NULL));
		if (3 == rand() % 7) {
			putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
			outtextxy(40, 300, "命中要害");
			FlushBatchDraw();
			Sleep(600);
			r *= 1.5;
		}
	}
	int damage = role1.theMON[role1.useNo].ATK * 0.3 + skl.power;
	emy.theMON[emy.useNo].hurt(r, damage);
}
void playerRun(Npc& emy)
{
	srand(time(NULL));
	int pro = 0;										//概率,（1~9）对应（10%~100%)
	Pokemon* emypok = &emy.theMON[emy.useNo];
	Pokemon* playerpok = &role1.theMON[role1.useNo];
	int levelgap = emypok->level - playerpok->level;	//等级差距
	if (levelgap >= 10)
		pro = 1;
	else if (levelgap >= 5)
		pro = 5;
	else
		pro = 7;
	int r = rand() % 9;
	if (r < pro)
		battle_close = RUN;
	else
	{
		string m = emypok->name + "死死盯着你不放";
		putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
		outtextxy(40, 300, m.c_str());
		FlushBatchDraw();
		Sleep(1000);
	}
}
void battleinput(Npc& emy)
{
	if (_kbhit()) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			if (point3X > 38) point3X -= 214;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			if (point3X < 252) point3X += 214;
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (point3Y > 299) point3Y -= 80;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (point3Y < 379) point3Y += 80;
		}
		else if (GetAsyncKeyState(0x41) & 0x8000) {
			if (point3X == 38 && point3Y == 299)			//左上按钮
			{
				if(turn == -1)
					turn = -2;
				else if (turn == -2) {
					playerAttack(emy, 0);
					//turn = 1;
				}
			}
			else if (point3X == 252 && point3Y == 299)		//右上按钮
			{
				if (turn == -1) {								//如果是总菜单就是背包按钮
					BagSys();
					battleshow(emy);
				}
				else if (turn == -2) {
					playerAttack(emy, 1);
					//turn = 1;
				}
			}
			else if (point3X == 252 && point3Y == 379)		//右下按钮
			{
				if (turn == -1) {								//逃跑
					playerRun(emy);
					turn = 1;
				}
				else if (turn == -2) {
					playerAttack(emy, 3);
					//turn = 1;
				}
			}
			Sleep(500);
		}
		else if (GetAsyncKeyState(0x44) & 0x8000) {
			if (turn == -2)
				turn = -1;
		}
	}
	FlushMouseMsgBuffer();
}
void emyAttack(Npc& emy)   //敌人的攻击方式
{
	/*
		1.血量充足则攻击
		2.血量不足回血
	*/
	if (emy.theMON[emy.useNo].life <= emy.theMON[emy.useNo].max_life * 0.25
		&& emy.item[0].count > 0)
	{
		outtextxy(40, 300, "对方使用了伤药");
		FlushBatchDraw();
		playmic("covermic");
		Sleep(800);
		emy.theMON[emy.useNo].life += emy.item[0].size;
		turn = -1;
	}
	else {	
		outtextxy(40, 300, "对方使用了撞击");
		FlushBatchDraw();
		Sleep(600);		
		double r = 1;   //伤害倍数；
		srand(time(NULL));
		if (3 == rand() % 10) {
			putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
			outtextxy(40, 300, "没有命中");
			FlushBatchDraw();
			Sleep(600);
			r = 0;
		}
		else {
			attackflush(emy);
			playmic("attackmic");
			putimage(0, 279, &msgbk[0]);    //覆盖之前的画面
			outtextxy(40, 300, "效果一般");
			FlushBatchDraw();
			Sleep(600);
			srand(time(NULL));
			if (3 == rand() % 7) {
				Sleep(600);
				putimage(0, 279, &msgbk[0]);//覆盖之前的画面
				outtextxy(40, 300, "命中要害");
				FlushBatchDraw();
				Sleep(600);
				r = 1.5;
			}
		}
		role1.theMON[role1.useNo].hurt(r, emy.theMON[emy.useNo].ATK);
		Sleep(1000);
	}
}
void battlemusic_open()							//关闭背景音乐 打开战斗音乐
{
	mciSendString("stop backmusic", NULL, 0, NULL);

	mciSendString(_T("open test\\battlemic.mp3 alias battlemic"), NULL, 0, NULL);//战斗背景音乐
	mciSendString("open test\\攻击.mp3 alias attackmic", NULL, 0, NULL);
	mciSendString("open test\\濒死.mp3 alias deadmic", NULL, 0, NULL);
	mciSendString("open test\\回复.mp3 alias covermic", NULL, 0, NULL);
	mciSendString("open test\\投精灵球.mp3 alias throwmic", NULL, 0, NULL);
	mciSendString("open test\\胜利.mp3 alias successmic", 0, 0, 0);
	mciSendString("open test\\逃跑.mp3 alias runmic", 0, 0, 0);

}
void battlemusic_close()
{
	mciSendString("close battlemic", NULL, 0, NULL);
	mciSendString("close attackmic", NULL, 0, NULL);
	mciSendString("close successmic", 0, 0, 0);
	mciSendString("close deadmic", 0, 0, 0);
	mciSendString("close covermic", 0, 0, 0);
	mciSendString("close throwmic", 0, 0, 0);
	mciSendString("close runmic", 0, 0, 0);

	mciSendString("play backmusic repeat", NULL, 0, NULL);
}
void battlestart(Npc& emy)														//战斗开始的场景（只播放一次）
{
	battlemusic_open();
	mciSendString(_T("play battlemic repeat"), NULL, 0, NULL);
	interface_switch();
	cleardevice();
	char s[50];
	Settxt(BLACK);
	/*画面 1*/
	putimage(0, 0, &battle);  //场景
	putimage(0, 279, &msgbk[0]);  //对话框
	putimage(0, 149, 136, 130, &role1.battlepicB, 0, 0, NOTSRCERASE);	 //原图白底，掩码图黑底
	putimage(0, 149, 136, 130, &role1.battlepic, 0, 0, SRCINVERT);
	putimage(330, 64, 120, 120, &emy.battlepicB, 0, 0, NOTSRCERASE);  //原图白底，掩码图黑底
	putimage(330, 64, 120, 120, &emy.battlepic, 0, 0, SRCINVERT);
	FlushBatchDraw();
	Sleep(500);
	/*画面 2*/
	outtextxy(50, 300, "对方发起了挑战");
	putimage(0, 0, &battle);  //场景
	putimage(0, 149, 135, 130, &role1.battlepicB, 138, 0, NOTSRCERASE);	 //原图白底，掩码图黑底
	putimage(0, 149, 135, 130, &role1.battlepic, 138, 0, SRCINVERT);
	putimage(330, 64, 120, 120, &emy.battlepicB, 0, 0, NOTSRCERASE);  //原图白底，掩码图黑底
	putimage(330, 64, 120, 120, &emy.battlepic, 0, 0, SRCINVERT);
	FlushBatchDraw();
	Sleep(1000);
	/*画面 3*/
	putimage(0, 279, &msgbk[0]);  //对话框覆盖
	string msg = "就决定是你了，" + role1.theMON[role1.useNo].name;
	outtextxy(50, 300, msg.c_str());
	FlushBatchDraw();
	playmic("throwmic");
	for (int i = 269; i <= 579; i += 155) {
		putimage(0, 0, &battle);  //场景
		putimage(0, 149, 155, 130, &role1.battlepicB, i, 0, NOTSRCERASE);	 //原图白底，掩码图黑底
		putimage(0, 149, 155, 130, &role1.battlepic, i, 0, SRCINVERT);
		putimage(330, 64, 120, 120, &emy.battlepicB, 0, 0, NOTSRCERASE);  //原图白底，掩码图黑底
		putimage(330, 64, 120, 120, &emy.battlepic, 0, 0, SRCINVERT);
		FlushBatchDraw();
		Sleep(100);
	}
	Sleep(300);

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
void battlemenu()
{
	Pokemon temp = role1.theMON[role1.useNo];
	if (turn == -1) {
		putimage(point3X, point3Y, &bottom[5]);		//指示器 
		putimage(40, 300, &bottom[0]);				//攻击按钮
		putimage(254, 300, &bottom[2]);				//背包
		putimage(40, 380, &bottom[3]);				//精灵列表
		putimage(254, 380, &bottom[4]);				//逃跑
	}
	else if (turn == -2) {
		putimage(point3X, point3Y, &bottom[5]);		//指示器 
		int i = 0;
		for (int y = 300; y <= 380; y += 80) {
			for (int x = 40; x <= 254; x += 214) {
				outtextxy(x, y, temp.skill[i].name.c_str());
				if (i++ == temp.skill.size() - 1) {
					i = -1;
					break;
				}
			}
			if (i == -1) break;
		}
	}
}
void battleshow(Npc& emy)  //绘制战斗场景
{
	cleardevice();
	char hp[50];
	char lv[10];
	char exp[50];
	Settxt(BLACK);
	if (role1.theMON[role1.useNo].life < (double)role1.theMON[role1.useNo].max_life * 0.15)
		playmic("deadmic", "repeat");
	else
		stopmic("deadmic");
	putimage(0, 0, &battle);						//场景
	putimage(0, 279, &msgbk[0]);					//对话框

	if (playerflush != 15) battlemenu();
	//主角
	putimage(10, 120, &statebk[role1.theMON[role1.useNo].property]);//状态窗口		
	putimage(30 + playerflush, 159, &role1.theMON[role1.useNo].picB, NOTSRCERASE);//主角宠物
	putimage(30 + playerflush, 159, &role1.theMON[role1.useNo].pic, SRCINVERT);
	sprintf(hp, "HP: %3d/%3d", role1.theMON[role1.useNo].life, role1.theMON[role1.useNo].max_life);
	sprintf(lv, "LV: %2d", role1.theMON[role1.useNo].level);
	sprintf(exp, "EXP:%3d/%3d", role1.theMON[role1.useNo].EXP, role1.theMON[role1.useNo].need_EXP);
	outtextxy(20, 125, hp);
	outtextxy(150, 125, lv);
	outtextxy(121, 147, exp);
	//敌人
	putimage(280, 30, &statebk[emy.theMON[emy.useNo].property]);//状态窗口	
	putimage(320 - emyflush, 60, &emy.theMON[emy.useNo].picB, NOTSRCERASE);
	putimage(320 - emyflush, 60, &emy.theMON[emy.useNo].pic, SRCINVERT);  //emy
	sprintf(hp, "HP: %3d/%3d", emy.theMON[emy.useNo].life, emy.theMON[emy.useNo].max_life);
	sprintf(lv, "LV: %2d", emy.theMON[emy.useNo].level);
	sprintf(exp, "EXP:%3d/%3d", emy.theMON[emy.useNo].EXP, emy.theMON[emy.useNo].need_EXP);
	outtextxy(290, 35, hp);
	outtextxy(420, 35, lv);
	outtextxy(391, 57, exp);

	FlushBatchDraw();

	playerflush = emyflush = 0;
}
int is_over(Npc& emy)   //判断战斗是否结束
{
	if (battle_close == RUN)
		return RUN;
	if (role1.theMON[role1.useNo].life <= 0)
		return LOSE;
	if (emy.theMON[emy.useNo].life <= 0)
		return VICTOR;
	return 1;
}
void battlefinish(Npc& emy)
{
	cleardevice();
	putimage(0, 0, &battle);  //场景
	putimage(0, 279, &msgbk[0]);  //对话框
	//主角
	putimage(0, 149, 140, 130, &role1.battlepicB, 0, 0, NOTSRCERASE);  //原图白底，掩码图黑底
	putimage(0, 149, 140, 130, &role1.battlepic, 0, 0, SRCINVERT);
	//敌人
	putimage(330, 64, 120, 120, &emy.battlepicB, 0, 0, NOTSRCERASE);  //原图白底，掩码图黑底
	putimage(330, 64, 120, 120, &emy.battlepic, 0, 0, SRCINVERT);
	char s[100] = "";
	putimage(0, 279, &msgbk[0]);
	switch (battle_close)
	{
	case LOSE:
		strcpy(s, "你输了");
		outtextxy(40, 300, s);
		FlushBatchDraw();
		Sleep(1000);
		battleLose();
		break;
	case VICTOR:
		strcpy(s, "你赢的了胜利");
		outtextxy(40, 300, s);
		FlushBatchDraw();
		Sleep(1000);
		stopmic("battlemic");
		playmic("successmic");
		battleSuccess(emy);
		break;
	case RUN:
		strcpy(s, "逃跑成功了");
		outtextxy(40, 300, s);
		FlushBatchDraw();
		Sleep(1000);
		stopmic("battlemic");
		playmic("runmic");
		break;
	}
	system("pause");
	battlemusic_close();
}
void battlesys(Npc& emy)   //战斗系统
{
	battlestart(emy);
	battle_close = 1;
	onBattle = 1;
	turn = -1;				//回合变化
	playerflush = 0;
	point3X = 38;
	point3Y = 299;
	while (battle_close == 1) {
		if (turn > 0)
		{
			emyAttack(emy);
			//turn = -1;
		}
		else
		{
			battleinput(emy);
		}
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		battleshow(emy);
		battle_close = is_over(emy);
	}
	emy.fight = (battle_close == VICTOR) ? 0 : 1;
	battlefinish(emy);
	onBattle = 0;
}
//____________________________________________
template <class T>
void darwmsg(T role)  //绘制对话框
{
	Settxt(BLACK);
	char s[100] = "";
	//加入文件读取文本
	ifstream msg(role.text);   //只读打开
	while (!msg.eof()) {
		playmic("chosemic");
		putimage(0, 403, &msgbk[1]);
		msg.getline(s, 100);     //读取一行
		outtextxy(60, 420, s);
		Sleep(150);  //防止闪频
		FlushBatchDraw();
		system("pause"); //按任意键继续
	}
	if (role.fight = true && !role1.theMON[role1.useNo].life) {
		putimage(0, 403, &msgbk[1]);
		strcpy(s, "你的首发精灵没有体力了");
		outtextxy(60, 420, s);
		FlushBatchDraw();
		system("pause");
	}
	msg.close(); //关闭
}
void scanINPUT()
{
	bool can[5];
	can[0] = can[1] = can[2] = can[3] = can[4] = 1;
	cantmove(role1, can);
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (can[cRT]) {
			if (role1.x < 463)
				if (mapX + mapSIZEx > 495)
					role1.x += 2;
				else
					role1.x += 4;
			if (mapX + mapSIZEx > 495) {
				mapX -= 3;
				npc[1].x -= 3;
			}
		}
		else playmic("movemic");
		//改变运动状态
		role1.state = role1.Ri;
		if (role1.Ri++ == 5)
			role1.Ri = 3;
	}
	else if (::GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if (can[cLF]) {
			if (role1.x > 0)
				if (mapX < 0)
					role1.x -= 2;
				else
					role1.x -= 4;
			if (mapX < 0) {
				mapX += 3;
				npc[1].x += 3;
			}
		}
		else playmic("movemic");
		role1.state = role1.Li;
		if (role1.Li++ == 11)
			role1.Li = 9;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (can[cUP]) {
			if (role1.y > 0)
				if (mapY < 0)
					role1.y -= 2;
				else
					role1.y -= 3;
			if (mapY < 0) {
				mapY += 3;
				npc[1].y += 3;
			}
		}
		else playmic("movemic");
		role1.state = role1.Ui;
		if (role1.Ui++ == 8)
			role1.Ui = 6;
	}
	else if (::GetAsyncKeyState(VK_DOWN) & 0x8000) {
		if (can[cDN]) {
			if (role1.y < 428)
				if (mapY + mapSIZEy > 464)
					role1.y += 2;
				else
					role1.y += 3;
			if (mapY + mapSIZEy > 464) {
				mapY -= 3;
				npc[1].y -= 3;
			}
		}
		else playmic("movemic");
		role1.state = role1.Di;
		if (role1.Di++ == 2)
			role1.Di = 1;
	}
	//对话
	else if (GetAsyncKeyState(0x41) & 0x8000) {
		int i = cover_glass[role1.y - mapY - 10][role1.x - mapX];
		if (i <= -2) {
			darwmsg(board[abs(i) - 1]);
		}
		else if (CalculateDIS(role1, npc[1]) < 15) {
			darwmsg(npc[1]);
			if (npc[1].fight == true && role1.theMON[role1.useNo].life)
				battlesys(npc[1]);
		}
		Sleep(200);
	}
	//打开菜单
	else if (GetAsyncKeyState(0x44) & 0x8000) {
		MenuSys();
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
int main()
{
	startup();
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	while (1) {
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		scanINPUT();
		updata();
		Sleep(70);
		npcmove(npc[1]);
	};
	EndBatchDraw();
	closegraph();
	mciSendString("close chosemic", 0, 0, 0);
	mciSendString("close backmic", 0, 0, 0);
	mciSendString("close battlemic", 0, 0, 0);
	mciSendString("close movemic", 0, 0, 0);
	mciSendString("close levelupmic", 0, 0, 0);
	return 0;
}
