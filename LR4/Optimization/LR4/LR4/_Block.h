#pragma once
class _Block
{
	int i_old, j_old;
	int i_new, j_new;
	int SAD;
	int SSD;
public:
	_Block(int i_old, int j_old, int i_new, int j_new, int SAD, int SSD);
	_Block(int i_old, int j_old);
	~_Block();
	void reload(int i_new, int j_new, int SAD, int SSD);
	int ret_SAD();
	int ret_SSD();
	int ret_i_old();
	int ret_j_old();
	int ret_i_new();
	int ret_j_new();
};

