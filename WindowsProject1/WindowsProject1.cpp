﻿#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <map>

#define set0(x) memset(x,0,sizeof(x))
#define isin(xl,xr,yu,yd) (latestX>=(xl)&&latestX<=(xr)&&latestY>=(yu)&&latestY<=(yd))
#define WAIT_UNTIL_MOUSE_INPUT \
isMouseTouched=false;\
while (GetMessage(&msg, NULL, 0, 0)){\
	TranslateMessage(&msg);\
	DispatchMessage(&msg);\
	if(isMouseTouched)break;\
}\
isMouseTouched=false;
#define WAIT_UNTIL_REDRAW \
InvalidateRect(hwnd,NULL,0);isRedrawed=false;\
while (GetMessage(&msg, NULL, 0, 0)){\
	TranslateMessage(&msg);\
	DispatchMessage(&msg);\
	if(isRedrawed)break;\
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hwnd;
int outbreakTrack=0;
int infectRate=0;
int numOfResearch = 0;
int cardNumByRate[7] = { 2,2,2,3,3,4,4 };
int status;
bool isMouseTouched, isRedrawed; int latestX, latestY; int nowPlayer=0;
bool flag;

typedef struct _Color {
	const wchar_t* chineseName;
	wchar_t englishName;
	int num;
	int cureStatus;//0 uncured,1 cured,2 eraditated
	int virusRemain;//24 in the beginning, -1 to lose, cure to recover
}Color;
Color colors[4];
typedef struct _HandCard {
	int nCityNum;//is special event num when cardtype==1
	int cardType;//0 when normal; 1 when special event; 2 when pandemic,-1 when empty
	_HandCard(int x = -1, int y = -1) :nCityNum(y), cardType(x) {}
	bool operator== (_HandCard cd) { return nCityNum == cd.nCityNum && cardType == cd.cardType; }
}HandCard;
HandCard emptyCard;
std::vector<HandCard> toUseHandCards;
std::vector<HandCard> usedHandCards;
void showUsedHC(); void showUsedVC(); int getClickOfCardPlc();
typedef struct _VirusCard {
	int nCitynum;
	_VirusCard(int x=0):nCitynum(x){}
	bool operator== (_VirusCard vs) { return nCitynum == vs.nCitynum; }
}VirusCard;
std::vector<VirusCard> toUseVirusCards;
std::vector<VirusCard> usedVirusCards;
int getClickOfCardPlc();
void outputText(int x, int y, const wchar_t* text,bool transparent=0);
#define CIRCLE 0
#define RECTANGLE 1
#define PENTAGON 2
void drawShape(const int type, int x, int y, int x1, int y1, COLORREF color, bool hasout);
void drawShape(const int type, POINT* ps,COLORREF color);
class City{
private:
	int neighbor[10];
	bool operator==(City c);
	bool hasVirus[3];
	int colVirus[3];
	bool hasOutbreak;
	int cx, cy;
	mutable int nowin;
	const int num;
	int getNextCity(int nowCity)const;//iterate all the city neighboring,return its num,start from self
public:
	City();
	const wchar_t* englishName;
	const wchar_t* chineseName;
	bool hasResearch;
	int color;
	bool isPeopleHere[4];
	int outbreak(int col);//>3 outbreak
	void redraw();
	bool isNeighbor(const City& c)const;
	friend void checkRemove();//check if virus of color color doesn't exist in the city
	void addVirus(int col=-1);
	bool subVirus();
	bool isClickNear(int clickX, int clickY);
	static void init();
	static void clearOutbreakStatus();
	void showInformation();
	bool confirmSelection(const wchar_t* thingToDo);
};
City cities[49];
void epidemic();
class Player {
protected:
	int cardToCure;
	void discardCard(HandCard h,bool toused);
	void addCard(HandCard h);
public:
	char playerType;
	int num;
	int nowCity;
	Player(int n);
	HandCard handCards[7];
	int remainMove;
	void drive(int c);//correctnesses of all the functions here is protected by outside. These are all noexcept.
	void directFlight(HandCard h);//go to h directed
	void charterFlight(HandCard h,int to);
	void shuttleFlight(int cWithResearch);
	virtual void buildResearch(HandCard h);
	void removeResearch();
	bool discoverCure(HandCard* hs);
	virtual bool treatDisease();
	virtual void /*a.k.a.share knowledge*/ deliverCard(HandCard h, Player* p,bool isGive);
	virtual void skill(Player* opee,int cityto) = 0;
	void touchCards();
	void drawCards();
	void infectVirus();
	void specialEvent(HandCard h,int type, Player* plr, int cty);
	void specialEvent(HandCard h,int type, int cty);
	void specialEvent(HandCard h,int type, HandCard* rearranged);
};
Player* players[4];
class SkilllessPlayer :public Player {
public:
	SkilllessPlayer(int n) :Player(n) { playerType = 'n'; }
	virtual void skill(Player* opee, int cityto) {}//this is a testing skillless player
};
class Actor :public Player {
public:
	Actor(int n) :Player(n) { playerType = 'a'; }
	virtual void skill(Player* opee, int cityto) {/*actually overload buildResearch();here to implement it,below same*/}
	virtual void buildResearch(HandCard h);
};
class Dispatcher :public Player {
public:
	Dispatcher(int n) :Player(n) { playerType = 'd'; }
	virtual void skill(Player* opee, int cityto);//here to check and move
};
class Medic :public Player {
public:
	Medic(int n) :Player(n) { playerType = 'm'; }
	virtual void skill(Player* opee, int cityto){}
	virtual bool treatDisease();
};
class Researcher :public Player {
public:
	Researcher(int n) :Player(n) { playerType = 'r'; }
	virtual void skill(Player* opee, int cityto){}//outside check,no need to overload anything
};
class Scientist :public Player {
public:
	virtual void skill(Player* opee, int cityto){}
	Scientist(int n) :Player(n) { playerType = 's'; cardToCure = 4; }//only need to override constructor to change cardToCure to 4
};

void drawBitmap(const wchar_t* fileName, int width, int height, int xUp, int yUp);
void executeSpecialEvent(HandCard u, Player* p, bool& flag2);

const wchar_t* event[] = { L"airmove",L"boom",L"foresee",L"fund",L"silent" };
const wchar_t* eventC[] = { L"空运",L"人口快速恢复",L"预测",L"政府拨款",L"寂静的一夜" };
const wchar_t* eventExplanation[] = { L"把随便一个玩家（包括自己）的棋子移动到任意城市",L"移除一张传染牌弃牌堆的牌",
L"改变牌堆上6张牌的顺序",L"在任意位置建立研究所",L"跳过即将进行的传染疾病阶段" };
const wchar_t* eventOperationDirection[] = { L"点击玩家，再点击城市",L"点击城市，将自动移除",L"在手牌区显示，点击一个牌将其放到第一个",
L"点击城市，将自动建立",L"直接点击确定，将生效" };
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){
	srand(time(0));
#include "colorinit.txt"
	City::init();
	players[0] = new SkilllessPlayer(0); cities[2].isPeopleHere[0] = true;
	players[1] = new SkilllessPlayer(1); cities[2].isPeopleHere[1] = true;
	players[2] = new SkilllessPlayer(2); cities[2].isPeopleHere[2] = true;
	players[3] = new SkilllessPlayer(3); cities[2].isPeopleHere[3] = true;
	std::map<char, const wchar_t*> nameOfPlayer, skillOfPlayer;	nameOfPlayer['n'] = L"无技能者"; skillOfPlayer['n'] = L"null";
	nameOfPlayer['a'] = L"行动专家"; skillOfPlayer['a'] = L"建立研究所时无需打出牌（仍然需要行动）";
	nameOfPlayer['d'] = L"调度员"; skillOfPlayer['d'] = L"你可以移动别人的棋子，就像自己的一样；还可以把别的棋子移动到有棋子的地方\n\
具体来说，你可以对别人执行前四个行动，用自己的手牌\n以上操作都需要一个行动";
	nameOfPlayer['m'] = L"医务兵"; skillOfPlayer['m'] = L"你治疗疾病可以移走一个地区的所有病毒。如果这里已经治愈，不需要消耗行动";
	nameOfPlayer['r'] = L"研究员"; skillOfPlayer['r'] = L"你给出牌只要和人在同一个城市就行，不是一定要给出那个城市的牌";
	nameOfPlayer['s'] = L"科学家"; skillOfPlayer['s'] = L"你只需要4张牌就可以发现解药";
	for (int i = 1; i <= 48; i++)
		toUseVirusCards.emplace_back(i),
		toUseHandCards.emplace_back(0, i);
	for (int i = 0; i < 5; i++)
		toUseHandCards.emplace_back(1, i);
	std::random_shuffle(toUseVirusCards.begin(), toUseVirusCards.end());
	std::random_shuffle(toUseHandCards.begin(), toUseHandCards.end());
	int difficulty = 4;
	flag = 0;
	WNDCLASS wndclass;
	MSG msg;
	wndclass.cbClsExtra = 0;wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = L"瘟疫危机";
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndclass);
	hwnd = CreateWindow(L"瘟疫危机", L"瘟疫危机", WS_OVERLAPPEDWINDOW, 0/*initial x*/
		, 0/*initial y*/, 1500/*width*/, 800/*height*/,NULL, NULL, hInstance, NULL);
	DWORD   dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	dwStyle &= ~(WS_SIZEBOX);
	SetWindowLong(hwnd, GWL_STYLE, dwStyle);//*/
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	int sz =(int) toUseHandCards.size();
	for (int i = 0; i < difficulty; i++) {
		int t = rand() % (sz / difficulty) + (sz / difficulty + 1) * (i);
		toUseHandCards.insert(toUseHandCards.begin()+t, _HandCard(2, 0));
	}
	nowPlayer = 0; bool flag = false, flag1 = false, flag2 = true; wchar_t info[100];
	for (int i = 0; i < 5; i++)
		players[0]->handCards[i] = _HandCard(1, i);
	while (1) {
		players[nowPlayer]->remainMove = 4;
		while (players[nowPlayer]->remainMove) {
			wchar_t uu[200] = L"还有x个行动";
			uu[2] = L'0' + players[nowPlayer]->remainMove;
			outputText(1050, 730, uu);
			WAIT_UNTIL_MOUSE_INPUT; //wsprintf(info, L"%d %d", latestX, latestY); MessageBox(hwnd, info, info, 0);
			if (isin(16, 60, 717, 749)) {
				//drive
				MessageBox(hwnd, L"请点击你想要去的城市来驾车去\n如果后悔了，点击取消", L"坐车", 0);
				while (1) {
					WAIT_UNTIL_MOUSE_INPUT;
					if (isin(847, 901, 714, 743))break;
					flag = false;
					for (int i = 1; i <= 48; i++) {
						if (cities[i].isClickNear(latestX, latestY)) {
							if (cities[i].isNeighbor(cities[players[nowPlayer]->nowCity])) {
								if (cities[i].confirmSelection(L"坐车"))
									players[nowPlayer]->drive(i), flag = true;
								else
									flag = false;
							}
							else {
								wsprintf(info, L"%s不是你的邻居，你不能去那里", cities[i].chineseName);
								MessageBox(hwnd, info, L"错误", MB_ICONEXCLAMATION);
							}
							break;
						}
					}
					if (flag)
						break;
					else
						MessageBox(hwnd, L"请重新选择或点击取消", L"坐车", 0);
				}
			}
			else if (isin(70, 116, 717, 749)) {
				//direct flight
				MessageBox(hwnd, L"请点击你想去的城市来乘坐直航\n你无需选择卡牌，系统将自动将对应的卡牌弃去\n如果后悔了，点击取消", L"直航", 0);
				while (1) {
					WAIT_UNTIL_MOUSE_INPUT;
					if (isin(847, 901, 714, 743))break;
					flag = false;
					for (int i = 1; i <= 48; i++) {
						if (cities[i].isClickNear(latestX, latestY)) {
							flag1 = false;
							int j;
							for (j = 0; j < 7; j++) {
								if (players[nowPlayer]->handCards[j].cardType == 0 && players[nowPlayer]->handCards[j].nCityNum == i) {
									flag1 = true; break;
								}
							}
							if (flag1) {
								if (cities[i].confirmSelection(L"直航"))
									players[nowPlayer]->directFlight(players[nowPlayer]->handCards[j]), flag = true;
								else
									flag = false;
							}
							else {
								wsprintf(info, L"你没有%s牌，你不能去那里", cities[i].chineseName); flag = false;
								MessageBox(hwnd, info, L"错误", MB_ICONEXCLAMATION);
							}
							break;
						}
					}
					if (flag)break;
					else MessageBox(hwnd, L"请重新选择或点击取消", L"直航", 0);
				}
			}
			else if (isin(128, 175, 717, 749)) {
				//chatter flight
				int j;
				flag1 = false;
				for (j = 0; j < 7; j++) {
					if (players[nowPlayer]->handCards[j].cardType == 0 && players[nowPlayer]->handCards[j].nCityNum == players[nowPlayer]->nowCity) {
						flag1 = true; break;
					}
				}
				if (flag1) {
					MessageBox(hwnd, L"请点击你想去的城市来乘坐包机\n你无需选择卡牌，系统将自动将本地的卡牌弃去\n如果后悔了，点击取消", L"包机", 0);
					while (1) {
						WAIT_UNTIL_MOUSE_INPUT;
						if (isin(847, 901, 714, 743))break;
						flag = false;
						for (int i = 1; i <= 48; i++) {
							if (cities[i].isClickNear(latestX, latestY)) {
								if (cities[i].confirmSelection(L"包机"))
									players[nowPlayer]->charterFlight(players[nowPlayer]->handCards[j], i), flag = true;
								else
									flag = false;
								break;
							}
						}
						if (flag)break;
						else MessageBox(hwnd, L"请重新选择或点击取消", L"包机", 0);
					}
				}
				else {
					MessageBox(hwnd, L"你没有本地牌，不能包机", L"错误", MB_ICONEXCLAMATION);
				}
			}
			else if (isin(181, 315, 717, 749)) {
				//shuttle flight
				if (cities[players[nowPlayer]->nowCity].hasResearch) {
					MessageBox(hwnd, L"请点击你想去的有研究所的城市来乘坐定点往返班机\n如果后悔了，点击取消", L"定点往返班机", 0);
					while (1) {
						WAIT_UNTIL_MOUSE_INPUT;
						if (isin(847, 901, 714, 743))break;
						flag = false;
						for (int i = 1; i <= 48; i++) {
							if (cities[i].isClickNear(latestX, latestY)) {
								if (cities[i].hasResearch) {
									if (cities[i].confirmSelection(L"定点往返班机"))
										players[nowPlayer]->shuttleFlight(i), flag = true;
									else
										flag = false;
									break;
								}
								else
									MessageBox(hwnd, L"那里没有研究所，不能定点往返班机", L"错误", MB_ICONEXCLAMATION);
							}
						}
						if (flag)break;
						else MessageBox(hwnd, L"请重新选择或点击取消", L"包机", 0);
					}
				}
				else
					MessageBox(hwnd, L"这里没有研究所，不能定点往返班机", L"错误", MB_ICONEXCLAMATION);
			}
			else if (isin(324, 414, 717, 749)) {
				//curedisease
				if (cities[players[nowPlayer]->nowCity].confirmSelection(L"治疗疾病")) {
					if (players[nowPlayer]->treatDisease())
						MessageBox(hwnd, L"成功治疗了疾病", L"治疗成功", 0);
					else
						MessageBox(hwnd, L"这里没有疾病", L"治疗失败", 0);
				}
			}
			else if (isin(422, 531, 717, 749)) {
				//buildresearch
				int j;
				flag1 = players[nowPlayer]->playerType == 'a';
				for (j = 0; j < 7; j++) {
					if (players[nowPlayer]->handCards[j].cardType == 0 && players[nowPlayer]->handCards[j].nCityNum == players[nowPlayer]->nowCity) {
						flag1 = true; break;
					}
				}
				if (j == 7)j = 6;
				if (flag1) {
					if (cities[players[nowPlayer]->nowCity].confirmSelection(L"建研究所"))
						players[nowPlayer]->buildResearch(players[nowPlayer]->handCards[j]);
				}
				else
					MessageBox(hwnd, L"你没有本地牌，也不是行动专家，不能建研究所", L"错误", MB_ICONEXCLAMATION);
			}
			else if (isin(546, 627, 717, 749)) {
				//findcure
				bool chosen[8]; HandCard cdChosen[7]; bool qx = false; set0(chosen); int cnt = 0;
				if (cities[players[nowPlayer]->nowCity].hasResearch) {
					MessageBox(hwnd, L"请选择想要用来找到解药的牌，取消放弃\n再按一次发现解药按钮确认选择\n注意：必须恰好5张（科学家4张），多、错、漏均不行", L"找解药", 0);
					while (1) {
						WAIT_UNTIL_MOUSE_INPUT;
						if (isin(546, 627, 717, 749)) {
							qx = false; break;
						}
						if (isin(847, 901, 714, 743)) {
							qx = true; break;
						}
						chosen[getClickOfCardPlc()] ^= 1;
						wchar_t uu[200] = L"已经选定的牌分别是第（从左到右从上到下）："; wchar_t xx[3] = L"1 ";
						outputText(1050, 730, L"                                                                                                                                    ");
						for (int i = 0; i < 7; i++)
							if (chosen[i])
								xx[0] = i + '1', wcscat(uu, xx);
						wcscat(uu, L"张牌");
						outputText(1050, 730, uu);
					}
					outputText(1050, 730, L"                                                                                                                                    ");
					if (!qx) {
						for (int i = 0; i < 7; i++)
							if (chosen[i])
								cdChosen[cnt++] = players[nowPlayer]->handCards[i];
						int co = cities[cdChosen[0].nCityNum].color; wchar_t uu[200];
						wsprintf(uu, L"%s色病毒已治愈", colors[co].chineseName);
						cdChosen[cnt] = _HandCard(-1, -1);
						if (players[nowPlayer]->discoverCure(cdChosen))
							MessageBox(hwnd, uu, L"发现解药", 0);
						else
							MessageBox(hwnd, L"给出的牌不符合要求", L"错误", MB_ICONEXCLAMATION);
					}
				}
				else {
					MessageBox(hwnd, L"这里没有研究所", L"错误", MB_ICONEXCLAMATION);
				}
			}
			else if (isin(642, 727, 717, 749)) {
				//change card
				int j; int toPeople = -1; bool isgive = false;
				MessageBox(hwnd, L"请通过点击头像选择给牌对象\n取消放弃", L"给牌", 0);
				while (1) {
					WAIT_UNTIL_MOUSE_INPUT;
					if (isin(847, 901, 714, 743))break;
					if (isin(1200, 1280, 700, 720)) {
						toPeople = ((latestX - 1200) / 20 + nowPlayer) % 4;
						if (players[toPeople]->nowCity == players[nowPlayer]->nowCity) {
							isgive = MessageBox(hwnd, L"给出牌还是收到牌？\n给出点是，收到点否", L"选择方向", MB_YESNO | MB_ICONQUESTION) == IDYES;
							break;
						}
						else
							MessageBox(hwnd, L"两个人不在同一个城市，不能给出牌", L"错误", MB_ICONEXCLAMATION);
					}
				}
				if (toPeople != -1) {
					if (isgive) {
						if (players[nowPlayer]->playerType == 'r') {
							MessageBox(hwnd, L"请选择要给出的牌\n取消放弃", L"给牌", 0);
							while (1) {
								WAIT_UNTIL_MOUSE_INPUT;
								if (isin(847, 901, 714, 743))break;
								int togivec = getClickOfCardPlc();
								if (togivec != 7) {
									if (players[nowPlayer]->handCards[togivec].cardType == 0) {
										if (cities[players[nowPlayer]->handCards[togivec].nCityNum].confirmSelection(L"给牌")) {
											players[nowPlayer]->deliverCard(players[nowPlayer]->handCards[togivec], players[toPeople], 1);
											break;
										}
									}
									else if (players[nowPlayer]->handCards[togivec].cardType == 1) {
										wchar_t u[200]; wsprintf(u, L"你确定要把特别行动牌%s给出吗？", eventC[players[nowPlayer]->handCards[togivec].nCityNum]);
										if (MessageBox(hwnd, u, L"给牌操作确认", MB_YESNO | MB_ICONQUESTION) == IDYES) {
											players[nowPlayer]->deliverCard(players[nowPlayer]->handCards[togivec], players[toPeople], 1);
											break;
										}
									}
									else {
										MessageBox(hwnd, L"你给的不是牌", L"错误", MB_ICONEXCLAMATION);
									}
								}
							}
						}
						else {
							flag1 = false;
							for (j = 0; j < 7; j++) {
								if (players[nowPlayer]->handCards[j].cardType == 0 && players[nowPlayer]->handCards[j].nCityNum == players[nowPlayer]->nowCity) {
									flag1 = true; break;
								}
							}
							if (flag1) {
								if (cities[players[nowPlayer]->nowCity].confirmSelection(L"给出本地牌")) {
									players[nowPlayer]->deliverCard(players[nowPlayer]->handCards[j], players[toPeople], 1);
								}
							}
							else
								MessageBox(hwnd, L"你没有本地牌，不能给出牌", L"错误", MB_ICONEXCLAMATION);
						}
					}
					else {
						flag1 = false;
						for (j = 0; j < 7; j++) {
							if (players[toPeople]->handCards[j].cardType == 0 && players[toPeople]->handCards[j].nCityNum == players[nowPlayer]->nowCity) {
								flag1 = true; break;
							}
						}
						if (flag1) {
							if (cities[players[nowPlayer]->nowCity].confirmSelection(L"收到本地牌")) {
								players[nowPlayer]->deliverCard(players[nowPlayer]->handCards[j], players[toPeople], 1);
							}
						}
						else
							MessageBox(hwnd, L"对方没有本地牌，不能收到牌", L"错误", MB_ICONEXCLAMATION);
					}
				}
				//complicated! by yxc
			}
			else if (isin(734, 824, 717, 749)) {
				//end turn
				break;
			}
			else if (isin(879, 1007, 535, 667)) {
				showUsedHC();
			}
			else if (isin(870, 1070, 22, 132)) {
				showUsedVC();
			}
			else if (isin(1200, 1440, 0, 600)) {
				int t = getClickOfCardPlc(); wchar_t uu[200];
				if (t != 7 && players[nowPlayer]->handCards[t].cardType != -1) {
					auto u = players[nowPlayer]->handCards[t];
					if (u.cardType == 0) {
						if (u.nCityNum == players[nowPlayer]->nowCity) {
							wsprintf(uu, L"%s是一张本地牌\n你可以用它来建研究所，传递或包机到任何地方", cities[u.nCityNum].chineseName);
							MessageBox(hwnd, uu, L"卡牌信息", 0);
						}
						else {
							wsprintf(uu, L"%s是一张外地牌\n你可以用它直航到那里", cities[u.nCityNum].chineseName);
							MessageBox(hwnd, uu, L"卡牌信息", 0);
						}
					}
					if (u.cardType == 1) {
						wsprintf(uu, L"你确定要使用特别事件牌%s吗？\n用处：%s\n用法：%s", eventC[u.nCityNum], eventExplanation[u.nCityNum], eventOperationDirection[u.nCityNum]);
						if (MessageBox(hwnd, uu, L"使用特别事件牌确认", MB_YESNO | MB_ICONQUESTION) == IDYES) {
							executeSpecialEvent(u, players[nowPlayer], flag2);
						}
					}
				}
			}
			else if (isin(1200, 1280, 700, 720)) {
				int t = ((latestX - 1200) / 20 + nowPlayer) % 4, i;
				wchar_t pp[300], tt[30];
				wsprintf(tt, L"玩家%d的信息", t);
				wsprintf(pp, L"名称：%s\n技能：%s\n手牌：", nameOfPlayer[players[t]->playerType], skillOfPlayer[players[t]->playerType]);
				for (i = 0; i < 7 && players[t]->handCards[i].cardType != -1; i++)
					wcscat(pp, players[t]->handCards[i].cardType ?
						eventC[players[t]->handCards[i].nCityNum] :
						cities[players[t]->handCards[i].nCityNum].chineseName),
					wcscat(pp, L"\n");
				if (players[nowPlayer]->playerType != 'd') {
					wcscat(pp, L"点是使用他的特殊技能牌，点否退出");
					if (MessageBox(hwnd, pp, tt, MB_YESNO | MB_ICONQUESTION)==IDYES) {
						for(int i=0;i<7&&players[t]->handCards[i].cardType!=-1;i++)
							if (players[t]->handCards[i].cardType == 1) {
								set0(pp); wsprintf(pp, L"你确定要使用特殊事件牌%s吗", eventC[players[t]->handCards[i].nCityNum]);
								if (MessageBox(hwnd, pp, L"使用别人的特殊事件牌", MB_YESNO | MB_ICONQUESTION)==IDYES) {
									executeSpecialEvent(players[t]->handCards[i], players[t], flag2);
									break;
								}
							}
					}
				}
				else {
					wcscat(pp, L"点是用他的特殊技能牌，点否退出，点取消移动他");
					int q = MessageBox(hwnd, pp, tt, MB_YESNOCANCEL | MB_ICONQUESTION);
					if (q == IDYES) {
						for (int i = 0; i < 7 && players[t]->handCards[i].cardType != -1; i++)
							if (players[t]->handCards[i].cardType == 1) {
								set0(pp); wsprintf(pp, L"你确定要使用特殊事件牌%s吗", eventC[players[t]->handCards[i].nCityNum]);
								if (MessageBox(hwnd, pp, L"使用别人的特殊事件牌", MB_YESNO | MB_ICONQUESTION)==IDYES) {
									executeSpecialEvent(players[t]->handCards[i], players[t], flag2);
									break;
								}
							}
					}
					if (q == IDCANCEL) {
						MessageBox(hwnd, L"请点击城市，将选择适当的方法把他送过去\n（优先使用不用牌的方法，两种以上用牌方法会询问），取消退出",L"调度员技能",0);
						while(1){
							WAIT_UNTIL_MOUSE_INPUT;
							if (isin(847, 901, 714, 743))break;
							flag = false;
							for (int i = 1; i <= 48; i++) {
								if (cities[i].isClickNear(latestX, latestY)) {
									players[nowPlayer]->skill(players[t], i);
									flag = true;
									break;
								}
							}
							if (flag)break;
							else MessageBox(hwnd, L"你点的不是城市", L"调度员技能", 0);
						}
					}
				}
			}
			else {
				for (int i = 1; i <= 48; i++) {
					if (cities[i].isClickNear(latestX, latestY)) {
						cities[i].showInformation(); break;
					}
				}
			}
			WAIT_UNTIL_REDRAW;
		}
		players[nowPlayer]->touchCards();
		WAIT_UNTIL_REDRAW;
		if(flag2)
			players[nowPlayer]->infectVirus();
		else
			MessageBox(hwnd, L"由于人口快速恢复特别事件牌，本次病毒感染取消",L"感染病毒", 0), flag2 = true;
		nowPlayer = (nowPlayer + 1) % 4;
		WAIT_UNTIL_REDRAW;
	}
	return 0;
}
void drawBitmap(const wchar_t* fileName, int width, int height, int xUp, int yUp) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("drawBitmap",f);
	fclose(f);*/
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps), dc = GetDC(hwnd),H1;
	RECT rect; GetClientRect(hwnd, &rect);
	HBITMAP map = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, width, height, LR_LOADFROMFILE);
	H1 = CreateCompatibleDC(dc);
	SelectObject(H1, map);
	BitBlt(dc, xUp, yUp, width, height, H1, 0, 0, SRCCOPY);
	DeleteObject(map);
	ReleaseDC(hwnd, H1); DeleteDC(H1);
	ReleaseDC(hwnd, hdc); DeleteDC(hdc);
	ReleaseDC(hwnd, dc); DeleteDC(dc);
	EndPaint(hwnd, &ps);
}
void outputText(int x, int y, const wchar_t* text, bool transparent) {
	HDC hdc = GetDC(hwnd);
	if (!transparent)
		SetTextColor(hdc, RGB(0, 0, 0)), SetBkColor(hdc, RGB(255, 255, 255));
	else
		SetTextColor(hdc, RGB(255, 255, 255)), SetBkColor(hdc, RGB(0,0,0)),SetBkMode(hdc,TRANSPARENT);
	TextOut(hdc, x, y, text, wcslen(text));
	ReleaseDC(hwnd, hdc); DeleteDC(hdc);
}
const int outbreakTrackX[9] = { 57,96,57,96,57,96,57,96,57 };
const int outbreakTrackY[9] = { 407,438,467,498,530,560,590,620,650 };
const int infectRateX[8] = { 709,757,805,857,903,956,1005 };
const int infectRateY[8] = { 140,155,160,165,159,154,141 };
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	/*FILE *f = fopen("a.txt", "a");
	fprintf(f,"+wndproc\n");
	fclose(f);*/
	COLORREF cols[4] = { RGB(255,255,255),RGB(0,255,0),RGB(255,0,255),RGB(0,255,255) };
	switch (message){
	case WM_CREATE:
		/*f = fopen("a.txt", "a");
		fprintf(f, "-crt:wndproc\n");
		fclose(f);*/
		return 0;
	case WM_PAINT:
		wchar_t c[40];
		drawBitmap(L"Pictures/white.bmp", 1500, 1000, 0, 0);
		drawBitmap(L"Pictures/main_background.bmp", 1200, 700, 0, 0);
		drawBitmap(L"Pictures/hback.bmp", 128, 152, 715, 535);
		wsprintf(c, L"剩余手牌数：%d", toUseHandCards.size());
		outputText(730, 600, c, true);
		drawBitmap(L"Pictures/movements.bmp", 1117, 55, 0, 701);
		for (int i = 0; i < 4; i++) {
			drawShape(CIRCLE, 1200 + i * 20, 700, 1220 + i * 20, 720, cols[(i + nowPlayer) % 4], (i + nowPlayer) % 4 == 0);
		}
		if (usedHandCards.size()) {
			auto t = *usedHandCards.rbegin();
			if (t.cardType == 0)
				wsprintf(c, L"Pictures/cities/city%d/h.bmp", (t).nCityNum);
			else if (t.cardType == 1)
				wsprintf(c, L"Pictures/Event/%s.bmp", event[t.nCityNum]);
			else
				wsprintf(c, L"Pictures/epidemic.bmp");
			drawBitmap(c, 128, 152, 879, 535);
		}
		drawBitmap(L"Pictures/vback.bmp", 200, 110, 684, 22);
		wsprintf(c, L"剩余病毒牌数：%d", toUseVirusCards.size());
		outputText(700, 30, c, true);
		if (usedVirusCards.size()) {
			wsprintf(c, L"Pictures/cities/city%d/v.bmp", (*usedVirusCards.rbegin()).nCitynum);
			drawBitmap(c, 200, 110, 870, 22);
		}
		if (outbreakTrack == 0)
			drawBitmap(L"Pictures/O0.bmp", 44, 40, outbreakTrackX[outbreakTrack], outbreakTrackY[outbreakTrack]);
		else if(outbreakTrack%2)
			drawBitmap(L"Pictures/O1.bmp", 44, 40, outbreakTrackX[outbreakTrack], outbreakTrackY[outbreakTrack]);
		else
			drawBitmap(L"Pictures/O2.bmp", 44, 40, outbreakTrackX[outbreakTrack], outbreakTrackY[outbreakTrack]);
		drawBitmap(L"Pictures/infRate.bmp", 42, 37, infectRateX[infectRate], infectRateY[infectRate]);
		players[nowPlayer]->drawCards();
		for (int i = 0; i < 4; i++) {
			wchar_t nm[30]; swprintf_s(nm, L"Pictures/cureEff/%c%d.bmp", colors[i].englishName, colors[i].cureStatus);
			wsprintf(c, L"剩%d病毒", colors[i].virusRemain);
			outputText(345 +85 * i, 680, c,true);
			//MessageBox(hwnd, nm, L"abc", 0);
			drawBitmap(nm, 40, 40, 355 + 85 * i, 642 - (colors[i].cureStatus != 0) * 42);
		}
		for (int i = 1; i <= 48; i++)
			if(1||i%6==0)
				cities[i].redraw();
		isRedrawed = true;
		/*f = fopen("a.txt", "a");
		fprintf(f, "-pnt:wndproc\n");
		fclose(f);*/
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		/*f = fopen("a.txt", "a");
		fprintf(f, "-dsty:wndproc\n");
		fclose(f);*/
		return 0;
	case WM_LBUTTONDOWN:
		int mouse_x = (int)LOWORD(lParam);
		int mouse_y = (int)HIWORD(lParam);
		isMouseTouched = 1; latestX = mouse_x; latestY = mouse_y; 
		//wsprintf(a, L"pos:%d %d", mouse_x, mouse_y), MessageBox(hwnd,a, L"abc", 0);
		/*f = fopen("a.txt", "a");
		fprintf(f, "-bt:wndproc\n");
		fclose(f);*/
		return 0;
	}
	/*f = fopen("a.txt", "a");
	fprintf(f, "-nm:wndproc\n");
	fclose(f);*/
	return DefWindowProc(hwnd, message, wParam, lParam);
}


City::City() :num((int)(this-cities)){
	int t =(int)( this - cities), u = 0, i = 0;
	set0(hasVirus); set0(isPeopleHere);
	hasOutbreak = hasResearch = false;
	hasVirus[0] = hasVirus[1] = hasVirus[2] =false; 
	isPeopleHere[0] = isPeopleHere[1] = isPeopleHere[2] = isPeopleHere[3] = false;
	hasResearch = false;
}
void City::init() {
#include "cityinit.txt"
}
void drawShape(const int type, int x, int y, int x1, int y1, COLORREF color, bool hasout) {
	HPEN pen = !hasout ? CreatePen(PS_SOLID, 1, color) : CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	HBRUSH brush = CreateSolidBrush(color); HDC hdc = GetDC(hwnd);
	if (type == CIRCLE) {
		SelectObject(hdc, pen); SelectObject(hdc, brush);
		Ellipse(hdc, x, y, x1, y1);
	}
	else if (type == RECTANGLE) {
		SelectObject(hdc, pen); SelectObject(hdc, brush);
		Rectangle(hdc, x, y, x1, y1);
	}
	DeleteObject( pen); DeleteObject( brush);
	ReleaseDC(hwnd, hdc); DeleteDC(hdc);
}
void drawShape(int type, POINT* ps,COLORREF color) {
	HPEN pen=CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	HDC hdc = GetDC(hwnd);
	if (type == PENTAGON) {
		SelectObject(hdc, pen); SelectObject(hdc, brush);
		Polygon(hdc, ps, 5);
	}
	DeleteObject(pen); DeleteObject(brush);
	ReleaseDC(hwnd, hdc); DeleteDC(hdc);
}
void City::redraw() {
	/*FILE *f = fopen("a.txt", "a");
	fputs( "city::redraw",f);
	fclose(f);*/
	COLORREF cols[4] = { RGB(0,0,250),RGB(255,255,0),RGB(0,0,0),RGB(250,0,0) };
	if (this->hasVirus[0]) {
		drawShape(RECTANGLE, cx - 5, cy - 15, cx + 5, cy - 5, cols[colVirus[0]], colVirus[0] == 1);
	}
	if (this->hasVirus[1]) {
		drawShape(RECTANGLE, cx - 5, cy + 5, cx + 5, cy + 15, cols[colVirus[1]], colVirus[1] == 1);
	}
	if (this->hasVirus[2]) {
		drawShape(RECTANGLE, cx - 15, cy - 5, cx - 5, cy + 5, cols[colVirus[2]], colVirus[2] == 1);
	}
	if (this->isPeopleHere[0]) {
		drawShape(CIRCLE, cx - 15, cy - 15, cx - 5, cy - 5, RGB(255, 255, 255), 0);
	}
	if (this->isPeopleHere[1]) {
		drawShape(CIRCLE, cx + 5, cy - 15, cx + 15, cy - 5, RGB(0, 255, 0), 0);
	}
	if (this->isPeopleHere[2]) {
		drawShape(CIRCLE, cx - 15, cy + 5, cx - 5, cy + 15, RGB(255, 0, 255), 0);
	}
	if (this->isPeopleHere[3]) {
		drawShape(CIRCLE, cx + 5, cy + 5, cx + 15, cy + 15, RGB(0, 255, 255), 0);
	}
#define setxy(i,ix,iy) pentagon[i].x=cx + ix;pentagon[i].y=cy + iy;
	if (this->hasResearch) {
		POINT pentagon[5];
		setxy(0, 10, -5); setxy(1, 5, 0); setxy(2, 5, 5); setxy(3, 15, 5); setxy(4, 15, 0);
		drawShape(PENTAGON, pentagon, RGB(255, 128, 0));
	}
}//*/
Player::Player(int n) {
	cardToCure = 5; this->nowCity = 2; playerType = 'n'; num = n; remainMove = 0;
	for (int i = 0; i < 7; i++)
		handCards[i] = _HandCard(-1, -1);
}

void Player::drawCards() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::drawcards", f);
	fclose(f);*/
	wchar_t nm[100];
	drawBitmap(L"Pictures/white.bmp", 250, 620, 1200, 0);
	for (int i = 0; i < 7; i++) {
		if (handCards[i].cardType == 0) {
			wsprintf(nm, L"Pictures/cities/city%d/h.bmp", handCards[i].nCityNum); //MessageBox(hwnd, nm, L"anc", 0);
			drawBitmap(nm, 120, 150, 1200 + (i % 2) * 120, (i / 2) * 150);
		}
		else if (handCards[i].cardType == 1) {
			wsprintf(nm, L"Pictures/Event/%s.bmp", event[handCards[i].nCityNum]);// MessageBox(hwnd, nm, L"anc", 0);
			drawBitmap(nm, 120, 150, 1200 + (i % 2) * 120, (i / 2) * 150);
		}
		else if (handCards[i].cardType == 2) {
			drawBitmap(L"Pictures/epidemic.bmp", 120, 150, 1200 + (i % 2) * 120, (i / 2) * 150);
		}
		else return;
	}
	return;
}
int getClickOfCardPlc() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("getclickofcardplc", f);
	fclose(f);*/
	if (latestX < 1200 || latestX>1440 || latestY > 600)
		return 7;
	return (latestY / 150) * 2 + (latestX > 1320);
}
bool City::operator== (City c) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city==", f);
	fclose(f);*/
	return num == c.num;
}
int City::getNextCity(int nowCity)const{
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::getnextcity", f);
	fclose(f);*/
	if (nowCity == num) {
		nowin = 0;
		return neighbor[0];
	}
	if (nowCity == 0) {
		nowin = 0; return 0;
	}
	++nowin;
	return neighbor[nowin];
}
bool City::isNeighbor(const City& c)const {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::isneighbor", f);
	fclose(f);*/
	int t = c.num;
	while (t = c.getNextCity(t)) {
		if (cities[t] == *this)return true;
	}
	return false;
}
bool checkRemove(int col) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::checkrm", f);
	fclose(f);*/
	for (int i = 1; i <= 48; i++)
		
				return false;
	return true;
}
void City::addVirus(int col) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::+v", f);
	fclose(f);*/
	if (col == -1)
		col = color;
	if (colors[col].cureStatus == 2)return;
	for (int i = 0; i < 3; i++) {
		if (!hasVirus[i]) {
			hasVirus[i] = true;
			colVirus[i] = col;
			colors[col].virusRemain--;
			return;
		}
	}
	outbreakTrack += outbreak(col);
	if (outbreakTrack > 8)
		outbreakTrack = 8;
}
int City::outbreak(int col) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::outbreak", f);
	fclose(f);*/
	if (hasOutbreak)return 0;
	hasOutbreak = true;
	MessageBox(hwnd, chineseName, L"oops:将要爆发的位置：", 0);
	int t = this->num;
	while (t = getNextCity(t)) {
		cities[t].addVirus(col);
	}
	return 1;
}
bool City::subVirus() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::-v", f);
	fclose(f);*/
	bool res = false;
	if (colors[color].cureStatus == 1) {
		for (int i = 0; i < 3; i++) {
			res |= hasVirus[i];
			hasVirus[i] = 0;
			colors[colVirus[i]].virusRemain++;
		}
		return res;
	}
	for(int i=2;i>-1;i--)
		if (hasVirus[i]) {
			hasVirus[i] = 0;  
			colors[colVirus[i]].virusRemain++;
			return true;
		}
	return false;
}
void City::clearOutbreakStatus() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::-outbreak status", f);
	fclose(f);*/
	for (int i = 1; i <= 48; i++)
		cities[i].hasOutbreak = false;
}
bool City::isClickNear(int clickX, int clickY) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::isclicknear", f);
	fclose(f);*/
	//wchar_t info[20]; wsprintf(info, "%d %d %d %d", clickX, clickY, cx, cy);MessageBox(hwnd, info, "abc", 0);
	return clickX <= cx + 20 && clickY <= cy + 20 && clickX >= cx - 20 && clickY >= cy - 20;
}
void City::showInformation() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::showinf", f);
	fclose(f);*/
	wchar_t info[200], title[20]; int nvirus = 0+hasVirus[0] + hasVirus[1] + hasVirus[2],
		nplayer=0+isPeopleHere[0]+isPeopleHere[1]+isPeopleHere[2]+isPeopleHere[3];
	wsprintf(info, L"%s是个%s色城市，%s研究所\n有%d个病毒，%d人在此", chineseName, colors[color].chineseName, hasResearch ? L"有" : L"无",nvirus, nplayer);
	wsprintf(title, L"%s信息", chineseName);
	MessageBox(hwnd, info, title, 0);
}
bool City::confirmSelection(const wchar_t* thingToDo) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("city::confirmsel", f);
	fclose(f);*/
	wchar_t info[200], title[20];
	wsprintf(info, L"你确定要对%s执行%s操作吗？", chineseName, thingToDo);
	wsprintf(title, L"%s操作确认", thingToDo);
	return MessageBox(hwnd, info, title, MB_YESNO | MB_ICONQUESTION) == IDYES;
}
void showUsedHC() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("showusedhc", f);
	fclose(f);*/
	wchar_t info[1000] = L"已经弃掉的手牌有：\n"; int t = 0;
	for (auto it : usedHandCards) {
		if (it.cardType == 0)
			wcscat(info, cities[it.nCityNum].chineseName);
		else if (it.cardType == 1)
			wcscat(info, eventC[it.nCityNum]);
		else
			wcscat(info,L"蔓延");
		if (++t % 4 == 0)wcscat(info, L"\n");
		else wcscat(info, L" ");
	}
	MessageBox(hwnd, info, L"已经弃掉的手牌信息", 0);
}
void showUsedVC() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("showusedvc", f);
	fclose(f);*/
	wchar_t info[1000] = L"已经弃掉的病毒牌有：\n"; int t = 0;
	for (auto it : usedVirusCards) {
		wcscat(info, cities[it.nCitynum].chineseName);
		if (++t % 4 == 0)wcscat(info, L"\n");
		else wcscat(info, L" ");
	}
	MessageBox(hwnd, info, L"已经弃掉的病毒牌信息", 0);
}
void Player::discardCard(HandCard h,bool toused) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::discardcard", f);
	fclose(f);*/
	for (int i = 0; i < 7; i++) {
		if (handCards[i] == h) {
			if (toused)
				usedHandCards.push_back(h);
			for (int j = i; j < 6; j++)
				handCards[j] = handCards[j + 1];
			handCards[6].cardType = -1;
			break;
		}
	}
}
void Player::buildResearch(HandCard h){
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::+research", f);
	fclose(f);*/
	cities[nowCity].hasResearch = true;
	numOfResearch++;
	discardCard(h,1);
	remainMove--;
}
void Actor::buildResearch(HandCard h) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("actor::+research", f);
	fclose(f);*/
	cities[nowCity].hasResearch = true;
	numOfResearch++;
	remainMove--;
}
void Player::drive(int c) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::drive", f);
	fclose(f);*/
	cities[nowCity].isPeopleHere[num] = false;
	nowCity = c;
	cities[nowCity].isPeopleHere[num] = true;
	remainMove--;
}
void Player::directFlight(HandCard h) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::directto", f);
	fclose(f);*/
	drive(h.nCityNum);
	discardCard(h,1);
}
void Player::charterFlight(HandCard h, int to) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::charterto", f);
	fclose(f);*/
	drive(to);
	discardCard(h,1);
}
void Player::shuttleFlight(int cWithResearch) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::shuttleto", f);
	fclose(f);*/
	drive(cWithResearch);
}
bool Player::treatDisease(){
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::treat", f);
	fclose(f);*/
	bool ret;
	if (colors[cities[nowCity].color].cureStatus == 0)
		remainMove -= ret = cities[nowCity].subVirus();
	else
		remainMove -= ret = cities[nowCity].subVirus() | cities[nowCity].subVirus() | cities[nowCity].subVirus();
	return ret;
}
bool Medic::treatDisease() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("medic::treat", f);
	fclose(f);*/
	bool t = cities[nowCity].subVirus() | cities[nowCity].subVirus() | cities[nowCity].subVirus();
	if (colors[cities[nowCity].color].cureStatus == 0)
		remainMove -= t;
	return t;
}
void Player::addCard(HandCard h) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::addcard", f);
	fclose(f);*/
	if(h.cardType==0)
		MessageBox(hwnd, cities[h.nCityNum].chineseName, L"摸到了：", 0);
	if (h.cardType == 1)
		MessageBox(hwnd, eventC[h.nCityNum], L"摸到了", 0);
	if (handCards[6].cardType!=-1) {
		MessageBox(hwnd, L"你的牌多于7张了，需要弃去\n点击有牌的地方弃牌，没牌的地方不摸牌", L"牌数过多", 0);
		MSG msg;
		WAIT_UNTIL_MOUSE_INPUT;
		int t = getClickOfCardPlc();
		if (t == 7)return;
		else discardCard(handCards[t],1);
	}
	for (int i=0;i<7;i++)
		if (handCards[i].cardType==-1) {
			handCards[i] = h;
			return;
		}
}
void Player::deliverCard(HandCard h, Player* p, bool isGive){
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::delivercard", f);
	fclose(f);*/
	if (isGive) {
		discardCard(h,0);
		p->addCard(h);
	}
	else {
		p->discardCard(h,0);
		addCard(h);
	}
}
void Player::removeResearch() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::-research", f);
	fclose(f);*/
	cities[nowCity].hasResearch = false;
}
void Player::infectVirus() {
	FILE *f = fopen("a.txt", "a");
	fputs("player::infectv\nat:", f);
	wchar_t info[200]=L"在";
	for (int i = 0; i < cardNumByRate[infectRate]; i++) {
		auto t = toUseVirusCards.back(); //MessageBox(hwnd, cities[t.nCitynum].chineseName, L"a", 0);
		City::clearOutbreakStatus();
		cities[t.nCitynum].addVirus();
		fwprintf(f, L"%s ", cities[t.nCitynum].chineseName);
		wcscat(info, cities[t.nCitynum].chineseName);
		wcscat(info, L" ");
		usedVirusCards.push_back(t);
		toUseVirusCards.pop_back();
	}
	wcscat(info, L"发生了病毒传播");
	MessageBox(hwnd, info, L"传播", 0);
	fprintf(f, "\ntouse");
	for (auto i : toUseVirusCards)
		fwprintf(f, L"%s ", cities[i.nCitynum].englishName);
	fprintf(f, "\nused:");
	for (auto i : usedVirusCards)
		fwprintf(f, L"%s ", cities[i.nCitynum].englishName);
	fprintf(f, "\n");
	fclose(f);
}
void Player::touchCards() {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::touchcard", f);
	fclose(f);*/
	for (int i = 0; i < 2; i++) {
		auto t = toUseHandCards.back();
		if (t.cardType == 2) {
			MessageBox(hwnd, L"发生了蔓延！", L"oops", 0) ;
			epidemic();
			toUseHandCards.pop_back();
			usedHandCards.push_back(t);
		}
		else {
			addCard(t);
			toUseHandCards.pop_back();
		}
	}
}
bool Player::discoverCure(HandCard* h) {
	/*FILE *f = fopen("a.txt", "a");
	fputs("player::discovercure", f);
	fclose(f);*/
	int co = cities[h->nCityNum].color;
	for (int i = 0; i < cardToCure; i++) {
		if (h[i].cardType != 0 || cities[h[i].nCityNum].color != co)
			return false;
	}
	if (h[cardToCure].cardType != -1)return false;
	for (int i = 0; i < cardToCure; i++) {
		discardCard(h[i],1);
	}
	colors[co].cureStatus = 1;
	remainMove--;
	return true;
}
void Player::specialEvent(HandCard h,int type, Player* plr, int cty) {
	if (type == 0) {
		plr->drive(cty);
		plr->remainMove++;
	}
	discardCard(h,1);
}
void Player::specialEvent(HandCard h,int type, int cty) {
	wchar_t u[200];
	if (type == 1) {
		auto t = std::find(usedVirusCards.begin(), usedVirusCards.end(), _VirusCard(cty));
		if (t != usedVirusCards.end()) {
			usedVirusCards.erase(t);
			wsprintf(u, L"%s病毒牌清除成功", cities[cty].chineseName);
			MessageBox(hwnd, u, L"人口快速恢复特别事件牌", 0);
		}
		else {
			wsprintf(u, L"%s病毒牌清除失败：没有这张牌", cities[cty].chineseName);
			MessageBox(hwnd, u, L"人口快速恢复特别事件牌", MB_ICONEXCLAMATION);
		}
	}
	else if (type == 3) {
		cities[cty].hasResearch = true;
		numOfResearch--;
		wsprintfW(u, L"在%s建研究所成功", cities[cty].chineseName);
		MessageBox(hwnd, u, L"政府拨款特别事件牌",0);
	}
	else if (type == 4) {
		MessageBox(hwnd, L"成功使用：下次病毒传播将不会发生", L"寂静的一夜特别事件牌", 0);
	}
	discardCard(h,1);
}
void Player::specialEvent(HandCard h,int type, HandCard* rearranged) {
	if (type == 2) {
		for (int i = 5; i >= 0; i--)
			toUseHandCards.push_back(rearranged[i]);
		MessageBox(hwnd, L"成功按照想法排列了", L"预测特别事件牌", 0);
	}
	discardCard(h,1);
}
void epidemic() {
	FILE *f = fopen("a.txt", "a");
	fputs("player::epidemic\nat:", f);
	infectRate++;
	MessageBox(hwnd, cities[toUseVirusCards[0].nCitynum].chineseName, L"要增加三个病毒的位置：", 0);
	fwprintf(f, L"%s \n", cities[toUseVirusCards[0].nCitynum].chineseName);
	for (int i = 0; i < 3; i++)City::clearOutbreakStatus(), cities[toUseVirusCards[0].nCitynum].addVirus();
	usedVirusCards.push_back(toUseVirusCards[0]);
	toUseVirusCards.erase(toUseVirusCards.begin());
	std::random_shuffle(usedVirusCards.begin(), usedVirusCards.end());
	while (usedVirusCards.size()) {
		toUseVirusCards.push_back(usedVirusCards.back());
		usedVirusCards.pop_back();
	}
	for (auto i : toUseVirusCards)
		fwprintf(f, L"%s ", cities[i.nCitynum].englishName);
	fprintf(f, "\nused:");
	for (auto i : usedVirusCards)
		fwprintf(f, L"%s ", cities[i.nCitynum].englishName);
	fclose(f);
}
void executeSpecialEvent(HandCard u, Player* p, bool& flag2) {
	MSG msg; int i, toplr = -1, tocty = 0;
	if (u.nCityNum == 0) {
		MessageBox(hwnd, L"请点击头像选择要移动的人，取消退出", L"空运特别事件牌", 0);
		while (1) {
			WAIT_UNTIL_MOUSE_INPUT;
			if (isin(847, 901, 714, 743))break;
			else if (isin(1200, 1280, 700, 720)) {
				toplr = (nowPlayer + (latestX - 1200) / 20) % 4; break;
			}
			else
				MessageBox(hwnd, L"点的地方不对，请重新选择", L"空运特别事件牌", MB_ICONEXCLAMATION);
		}
		if (toplr != -1) {
			MessageBox(hwnd, L"请点击你想去的城市，取消退出", L"空运特别事件牌", 0);
			while (1) {
				WAIT_UNTIL_MOUSE_INPUT;
				if (isin(847, 901, 714, 743))break;
				else {
					flag = false;
					for (i = 1; i <= 48; i++)
						if (cities[i].isClickNear(latestX, latestY)) {
							flag = 1; break;
						}
					if (flag)break;
				}
				MessageBox(hwnd, L"点的地方不对，请重新选择", L"空运特别事件牌", MB_ICONEXCLAMATION);
			}
			if(flag)
			    p->specialEvent(u, 0, players[toplr], i);
		}
	}
	else if (u.nCityNum == 1) {
		MessageBox(hwnd, L"请点击城市选择要移除的病毒牌，取消退出\n注意：请事先查有没有，没有你这牌也没了\n现在按已用病毒牌还可以查",
			L"人口迅速恢复特别事件牌", 0);
		while (1) {
			WAIT_UNTIL_MOUSE_INPUT;
			if (isin(847, 901, 714, 743))break;
			else if (isin(870, 1070, 22, 132)) {
				showUsedVC(); continue;
			}
			else {
				flag = false;
				for (i = 1; i <= 48; i++)
					if (cities[i].isClickNear(latestX, latestY)) {
						flag = 1; break;
					}
				if (flag)break;
			}
			MessageBox(hwnd, L"点的地方不对，请重新选择", L"人口迅速恢复特别事件牌", MB_ICONEXCLAMATION);
		}
		if(flag)
		    p->specialEvent(u, 1, i);
	}
	else if (u.nCityNum == 3) {
		MessageBox(hwnd, L"请点击城市选择要建立的研究所，取消退出", L"政府拨款特别事件牌", 0);
		while (1) {
			WAIT_UNTIL_MOUSE_INPUT;
			if (isin(847, 901, 714, 743))break;
			else if (isin(870, 1070, 22, 132)) {
				showUsedVC(); continue;
			}
			else {
				flag = false;
				for (i = 1; i <= 48; i++)
					if (cities[i].isClickNear(latestX, latestY)) {
						flag = 1; break;
					}
				if (flag)break;
			}
			MessageBox(hwnd, L"点的地方不对，请重新选择", L"政府拨款特别事件牌", MB_ICONEXCLAMATION);
		}
		if(flag)
		    p->specialEvent(u, 3, i);
	}
	else if (u.nCityNum == 4) {
		flag2 = false;
		p->specialEvent(u, 4, 0);
	}
	else if (u.nCityNum == 2) {
		MessageBox(hwnd, L"使用预测特殊事件牌：\n系统将把接下来几张手牌（先摸到的在前）的图片放在手牌区，\
看到后点击任一张牌，他将变成第一个\n例如原来6张牌按照将摸到的顺序123456，你想变成642135，可以依次点击2 4 6\n\
想变成654321，可以依次点击2 3 4 5 6\n点击取消退出，发现解药确认", L"预测特别事件牌", 0);
		Player* shower = new SkilllessPlayer(4); bool qx = false; int t, i, j, uyxu[] = { 1,2,3,4,5,6 };
		wchar_t x[200];
		HandCard tog[6];
		for (i = 0; i < 6; i++)
			shower->handCards[i] = toUseHandCards.back(), toUseHandCards.pop_back();
		shower->drawCards();
		while (1) {
			WAIT_UNTIL_MOUSE_INPUT;
			if (isin(546, 627, 717, 749)) {
				qx = false; break;
			}
			if (isin(847, 901, 714, 743)) {
				qx = true; break;
			}
			t = getClickOfCardPlc() + 1;
			for (i = 0; i < 6; i++) {
				if (uyxu[i] == t) {
					for (j = i; j > 0; j--)
						uyxu[j] = uyxu[j - 1];
					uyxu[0] = t;
				}
			}
			wsprintf(x, L"顺序为：%d %d %d %d %d %d", uyxu[0], uyxu[1], uyxu[2], uyxu[3], uyxu[4], uyxu[5]);
			outputText(1050, 730, x);
		}
		for (i = 0; i < 6; i++)
			tog[i] = shower->handCards[uyxu[i] - 1];
		delete shower;
		p->specialEvent(u, 2, tog);
	}
}
void Dispatcher::skill(Player* opee, int cityto) {
	bool suc = false, hasto = false, hasfrom = false,czto; int toc, fromc;
	if (cities[cityto].isNeighbor(cities[opee->nowCity]) ||
		(cities[cityto].hasResearch && cities[opee->nowCity].hasResearch) ||
		(cities[cityto].isPeopleHere[0] || cities[cityto].isPeopleHere[1] || cities[cityto].isPeopleHere[2] || cities[cityto].isPeopleHere[3]))
		opee->drive(cityto), suc = true;
	else {
		for(int i=0;i<6;i++)
			if (this->handCards[i].cardType == 0) {
				if (this->handCards[i].nCityNum == cityto)
					hasto = true, toc = i;
				if (this->handCards[i].nCityNum == opee->nowCity)
					hasfrom = true, fromc = i;
			}
		if (hasto || hasfrom) {
			if (hasto && !hasfrom)czto = true;
			if (!hasto && hasfrom)czto = false;
			if (hasto && hasfrom)czto = MessageBox(hwnd, L"请选择扔掉目的地牌还是当前所在地牌\n目的地按是，所在地按否", L"调度员技能", MB_YESNO | MB_ICONQUESTION) == IDYES;
			if (czto)
				opee->drive(cityto), suc = true, discardCard(this->handCards[toc], 1);
			else
				opee->drive(cityto), suc = true, discardCard(this->handCards[fromc], 1);
		}
		else
			suc = false;
	}
	if (suc) {
		MessageBox(hwnd, L"调度员成功移动目标人至目标地点", L"调度员技能", 0);
		opee->remainMove++;
		this->remainMove--;
	}
	else {
		MessageBox(hwnd, L"调度员未能成功移动目标人至目标地点", L"调度员技能", MB_ICONEXCLAMATION);
		opee->remainMove++;
	}
}
void checkRemove() {
	for (int col = 0; col < 4; col++) {
		bool flag = true;
		if (colors[col].cureStatus != 1)continue;
		for (int i = 1; i <= 48&&flag; i++)
			for (int j = 0; j < 3&&flag; j++)
				if (cities[i].hasVirus[j] && cities[i].colVirus[j] == col)
					flag = false;
		if (flag)
			colors[col].cureStatus = 2;
	}
}
#endif
