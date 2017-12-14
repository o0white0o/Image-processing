#pragma once
class _Block
{
	int i_old, j_old;
	int i_new, j_new;
	double SAD;
	int SSD;
public:
	_Block(int i_old, int j_old, int i_new, int j_new, double SAD, int SSD);
	_Block(int i_old, int j_old);
	~_Block();
	void reload(int i_new, int j_new, double SAD, int SSD);
	double getSAD();
	int ret_SSD();
	int get_i_old();
	int get_j_old();
	int get_i_new();
	int get_j_new();
};

