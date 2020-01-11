#pragma once

#define NORMAL 0
#define FIRE 1
#define WATER 2
#define GLASS 3


class Skill {
public:
	int property;		//属性
	int power;			//威力（基础伤害）
	string name;		//技能名
	int PP;				//使用次数
	int PPmax;			//最大使用次数

	string music;
	string pic;
	void play();		//播放技能动画
	bool operator < (const int& a);
	bool operator > (const int& a);
};
Skill f_1;				//火苗
Skill w_1;				//水枪
Skill n_1;				//撞击
void skill_startup()
{
	f_1.property = FIRE;
	f_1.power = 15;
	f_1.name = "火苗";
	f_1.PP = 20;
	f_1.PPmax = 20;

	w_1.property = WATER;
	w_1.power = 15;
	w_1.name = "水枪";
	w_1.PP = 20;
	w_1.PPmax = 20;

	n_1.property = NORMAL;
	n_1.power = 12;
	n_1.name = "撞击";
	n_1.PP = 20;
	n_1.PPmax = 20;
}
bool Skill::operator< (const int& a)
{
	if (a == property) return 0;
	switch (a)
	{
	case FIRE:
		switch (property)
		{
		case WATER:return 0;
		case GLASS:return 1;
		default:break;
		}
		break;
	case WATER:
		switch (property)
		{
		case FIRE:return 1;
		case GLASS:return 0;
		default:break;
		}
		break;
	case GLASS:
		switch (property)
		{
		case WATER:return 1;
		case FIRE:return 0;
		default:break;
		}
		break;
	}
}
bool Skill::operator> (const int& a) 
{
	if (a == property || a == NORMAL || property == NORMAL) return 0;
	return !Skill::operator<(a);
}
void Skill::play()
{

}

