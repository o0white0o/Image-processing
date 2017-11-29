#pragma once
class _Block
{
	int i_old, j_old;
	int i_new, j_new;
	float SAD;
	float SSD;
public:
	_Block(int i_old, int j_old, int i_new, int j_new, float SAD, float SSD);
	_Block();
	~_Block();
	float ret_SAD();
	float ret_SSD();
	int ret_i_old();
	int ret_j_old();
	int ret_i_new();
	int ret_j_new();
};

