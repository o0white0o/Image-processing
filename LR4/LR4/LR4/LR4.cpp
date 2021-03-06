// LR4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define Block	9
#define Search_Area  27
#define Step	3
#define _SAD	true
#define _SSD	false

using namespace cv;
using namespace std;

struct _Pixel
{
	int br;
	float SAD;
	float SSD;
};

template <typename _Tp>
Mat_<uchar> Add_to_right(Mat_<_Tp> input, int col);
template <typename _Tp>
Mat_<uchar> Add_to_down(Mat_<_Tp> input, int col);
template <typename _Tp>
Mat_<uchar> Convert(Mat_<_Tp> input);
int How_much_to_add(int Until_which_date);

int main()
{
	namedWindow("LR4");
	String filename("LR4.avi");		// Имя видеофрагмента
	VideoCapture capture = VideoCapture(filename);
	Mat_<uchar> frame;				// Для считывания кадра
	Mat_<uchar> t, t_1;				// Два последовательных кадра 
	vector<_Block> Blocks;			// Массив содержащий информацию про блоки
	vector<vector<_Pixel>> Pixels;	// Матрица содержащая информацию про восстановленные пиксели
	capture >> frame;
	cvtColor(frame, t_1, CV_BGR2GRAY );
	// Дополняем изображение для равномерного разбиения по блокам
	int How =						// Количество пикселей для добавления справа
		How_much_to_add(t_1.cols);
	t_1 = Add_to_right(t_1, How);
	How =							// Количество пикселей для добавления снизу
		How_much_to_add(t_1.rows);
	t_1 = Add_to_down(t_1, How);
	imshow("t_1", t_1);
	capture >> frame;
	cvtColor(frame, t, CV_BGR2GRAY);
	imshow("t", t);
	Mat_<Vec3b> vector				// Для векторов
		= Mat_<Vec3b>(t.rows, t.cols, Vec3b(255,255,255));
	//vector = frame;
	cout << t_1.cols << " " << vector.cols << " " << t_1.rows << " " << vector.rows << " ";
	for(int i = 0; i <= t_1.rows-Block;i+=Step)
	{			
		int i_window = i;	
		int counter = Search_Area;
		while (counter > 0 && i_window > 0)
		{
			i_window--;
			counter--;
		}
		int count_i = Search_Area + i - i_window;
		for (int j = 0; j <= t_1.cols - Block; j += Step)
		{
			Blocks.push_back(_Block());					// Информация по текущему блоку	
			int j_window = j;
			counter = Search_Area;
			while (counter > 0 && j_window > 0)
			{
				j_window--;
				counter--;
			}
			int count_j = Search_Area + j - j_window;
			for (int row_search = i_window, _count_= count_i; row_search < t.rows - Block && _count_>0; _count_--, row_search++)
			{
				
				for (int col_search = j_window, _count= count_j; col_search < t.cols - Block && _count>0; _count--, col_search++)
				{
					int SAD = 0;
					int SSD = 0;
					for (int row = 0; row < Block; row++)
					{
						for (int col = 0; col < Block; col++)
						{
							SAD += abs(t_1.at<uchar>(i + row, j + col) - t.at<uchar>(row_search + row, col_search + col));
							SSD += pow((t_1.at<uchar>(i + row, j + col) - t.at<uchar>(row_search + row, col_search + col)), 2);
						}
						if (Blocks.at(Blocks.size() - 1).ret_SAD() < SAD)
							break;
					}
					if (_SAD)
					{
						if (Blocks.at(Blocks.size() - 1).ret_SAD() > SAD)
						{
							Blocks.pop_back();
							Blocks.push_back(_Block(i, j, row_search, col_search, SAD, SSD));
						}
					}
					else if (_SSD)
					{
						if (Blocks.at(Blocks.size() - 1).ret_SSD() > SSD)
						{
							Blocks.pop_back();
							Blocks.push_back(_Block(i, j, row_search, col_search, SAD, SSD));
						}
					}
					else
					{
						std::cout << "Error 1.1.: No comparison mode selected" << endl;
						cv::waitKey(0);
						return 0;
					}
				}
			}
			int RED = rand() % 256;
			int GREEN = rand() % 256;
			int BLUE = rand() % 256;
			int final_Block = Blocks.size() - 1;
			line(vector, Point(Blocks.at(final_Block).ret_j_old(), Blocks.at(final_Block).ret_i_old()),
				Point(Blocks.at(final_Block).ret_j_new(), Blocks.at(final_Block).ret_i_new()),
				Scalar(RED, GREEN, BLUE), 1, 8, 0);
		}
	}
	Mat_<uchar> replica =					// Восстановленный кадр
		Mat_<uchar>(t_1.rows, t_1.cols);
	for (int i = 0; i < Blocks.size(); i++)
	{
		for(int row = 0; row < Block; row++)
			for (int col = 0; col < Block; col++)
			{
				replica.at<uchar>(Blocks.at(i).ret_i_old() + row, Blocks.at(i).ret_j_old() + col)
					= t.at<uchar>(Blocks.at(i).ret_i_new() + row, Blocks.at(i).ret_j_new() + col);
			}
	}
	imshow("Vectors", vector);
	imshow("Replica", replica);
	std::cout << Blocks.size();
	cv::waitKey(0);
	return 0;
}

/**
* Создание изображения с рамкой
*
*	К исходному изображению добавляется по
1 пикселю с каждой стороны. Каждый из которых
заполняется соседним оттенком
* @input Mat_<typename> исходное изображение
* @return Mat_<uchar>
*/
template <typename _Tp>
Mat_<uchar> Add_to_right(Mat_<_Tp> input,  int col)
{
	Mat_<_Tp> output 
		= Mat(input.rows, input.cols + col, CV_8UC1, Scalar(255, 255, 255));
	// Копия изображения с белой рамкой:
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			output.at<_Tp>(i, j) = input.at<_Tp>(i, j);
	return Convert(output);
}

/**
* Создание изображения с рамкой
*
*	К исходному изображению добавляется по
1 пикселю с каждой стороны. Каждый из которых
заполняется соседним оттенком
* @input Mat_<typename> исходное изображение
* @return Mat_<uchar>
*/
template <typename _Tp>
Mat_<uchar> Add_to_down(Mat_<_Tp> input, int col)
{
	Mat_<_Tp> output
		= Mat(input.rows+col, input.cols, CV_8UC1, Scalar(255, 255, 255));
	// Копия изображения с белой рамкой:
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			output.at<_Tp>(i, j) = input.at<_Tp>(i, j);
	return Convert(output);
}

/**
* Преобразование Mat_<typename> в Mat_<uchar>
*
*	Исходная матрица изображения нормализуется
в формат оттенков пикселей от 0 до 255.
Затем выводится в качестве выходного параметра.
* @input Mat_<typename> исходное изображение
* @return Mat_<uchar>
*/
template <typename _Tp>
Mat_<uchar> Convert(Mat_<_Tp> input)
{
	Mat_<uchar> output(input.rows, input.cols);
	normalize(input, input, 0,			// Линейное растяжение 
		255, NORM_MINMAX, -1, Mat());
	output = input;
	return output;
}

int How_much_to_add(int Until_which_date)
{
	int I_current = 0;
	while (I_current + Block < Until_which_date)
	{
		I_current = I_current + Step;
	}
	I_current += Block;
	return I_current - Until_which_date;
}