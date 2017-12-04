#include "stdafx.h"
#include "_Block.h"


_Block::_Block(int i_old=0, int j_old=0, int i_new=0, int j_new=0, int SAD=9999, int SSD=9999)
{
	this->i_old = i_old;
	this->j_old = j_old;
	this->SAD = SAD;
	this->SSD = SSD;
	this->i_new = i_new;
	this->j_new = j_new;
}
_Block::_Block(int i_old, int j_old)
{
	this->i_old = i_old;
	this->j_old = j_old;
	this->SAD = 99999;
	this->SSD = 99999;
	this->i_new = 0;
	this->j_new = 0;
}


_Block::~_Block()
{
}
void _Block::reload(int i_new, int j_new, int SAD, int SSD)
{
	this->SAD = SAD;
	this->SSD = SSD;
	this->i_new = i_new;
	this->j_new = j_new;
}

int _Block::ret_SAD()
{
	return this->SAD;
}

int _Block::ret_SSD()
{
	return this->SSD;
}
int _Block::ret_i_old()
{
	return this->i_old;
}
int _Block::ret_j_old()
{
	return this->j_old;
}
int _Block::ret_i_new()
{
	return this->i_new;
}
int _Block::ret_j_new()
{
	return this->j_new;
}