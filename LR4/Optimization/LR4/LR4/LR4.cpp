#include "stdafx.h"
#define Block	12			// Размер блока для поиска
#define Search_Area  30		// Размер окна для поиска в каждую из сторон
#define Step	12			// Шаг прохода по кадру t-1
#define BLOCK_INFO false	// Для вывода дополнительнйо информации по блокам
#define VIDEO	false		// true - работа с видеофайлом, false - работа с изображениями

using namespace cv;
using namespace std;

template <typename _Tp>
Mat_<uchar> Convert(Mat_<_Tp> input);
template <typename _Tp>
_Block Search_Block(Mat_<_Tp> t_1, Mat_<_Tp> t, int row, int col,int count_row, int count_col, int i_old, int j_old);
template <typename _Tp>
void Fill(Mat_<_Tp> &input, int Color);
template <typename _Tp>
void Cut(Mat_<_Tp> t_1, Mat_<_Tp> t,_Block bl);
template <typename _Tp, typename Tp>
Mat_<Tp> Reload(Mat_<Tp> inp, Mat_<_Tp> img, Mat_<_Tp> acc, _Block bl);
int main()
{
	namedWindow("LR4");
	String filename("LR4_3.mp4");	// Имя видеофрагмента
	String img1("1.png");			// Имя видеофрагмента
	String img2("2.png");			// Имя видеофрагмента
	vector<_Block> Blocks_info;		// Массив содержащий информацию про блоки
	Mat_<uchar>t_1,t;				// Два последовательных кадра
	if (VIDEO)						// Работа с видео фрагментом
	{
		VideoCapture capture = VideoCapture(filename);
		Mat_<uchar> frame;			// Для считывания кадра
		vector<Mat_<uchar>> Blocks	// Массив для хранения блоков размера Block x Block
			, Search_win;			// Массив для хранения окна для поиска
		capture >> frame;			// Считывание первого кадра
		cvtColor(frame, t_1, CV_BGR2GRAY);
		capture >> frame;			// Считываем второго кадр
		cvtColor(frame, t, CV_BGR2GRAY);
	}

	if (!VIDEO)						// Работа с изображениями
	{
		t_1 = imread(img1.c_str(), IMREAD_GRAYSCALE);
		t = imread(img2.c_str(), IMREAD_GRAYSCALE);
	}
	Mat_<Vec3b> vector				// Изображение для отрисовки векторов
		= Mat_<Vec3b>(t.rows, t.cols, Vec3b(255, 255, 255));
	Mat_<int> replica =				// Для накапливания оттенков востановленного кадра
		Mat_<int>(t_1.rows, t_1.cols);
	Mat_<uchar> accumulator =		// Аккумулятор количества перезаписи значения пикселя
		Mat_<uchar>(t_1.rows, t_1.cols);
	Fill(replica,0);
	Fill(accumulator,0);
	// Формируем 2 блока из кадра t_1 и t: 
	// 1 - содержащий блок размером Block Х Block
	// 2 - содержащий окная для поиска -Search_Area + Search_Area Х -Search_Area + Search_Area
	int count = 0;
	for (int row = 0; row < t_1.rows; row += Step)
	{
		if (t_1.rows - row < Block)
			row = t_1.rows - Block;
		// формирование окна для поиска по вертикали
		int i_window = max(0, row - Search_Area);
		int count_row = min(row - i_window,Search_Area)
			+min(Search_Area,t_1.rows-row);
		for (int col = 0; col < t_1.cols; col += Step)
		{
			if (t_1.cols - col < Block)
				col=t_1.cols - Block;
			// формирование окна для поиска по горизонтали
			int j_window = max(0,col-Search_Area);
			int count_col = min(col - j_window, Search_Area)
				+ min(Search_Area, t_1.cols - col);
			// Запись информации о текущем блоке
			_Block bl = Search_Block(t_1, t, i_window, j_window, count_row, count_col, row, col);
			if((bl.get_i_new()!= row || bl.get_j_new()!= col) && BLOCK_INFO)
				Cut(t_1, t, bl);
			if (BLOCK_INFO)
			{
				cout << ++count << "\ti,j_old= " << bl.get_i_old() << " " <<
					bl.get_j_old() << "\ti,j,new= " << bl.get_i_new() << " " <<
					bl.get_j_new() << "\tSAD= " << bl.getSAD() << endl;
			}
			// Отрисовка векторов:
			int RED = rand() % 256;
			int GREEN = rand() % 256;
			int BLUE = rand() % 256;
			line(vector, cv::Point(bl.get_j_old(),bl.get_i_old()),
				cv::Point(bl.get_j_new(), bl.get_i_new()),
				cv::Scalar(RED, GREEN, BLUE), 1, 8, 0);
			// Восстановление изображения
			replica = Reload(replica, t, accumulator, bl);
			if (col == t_1.cols - Block)
				break;
		}
		if (row == t_1.rows - Block)
			break;
	}
	// Подготовка восстановленного изображения к выводу на экран
	Mat_<uchar> rep =
		Mat_<uchar>(replica.rows, replica.cols);
	for (int i = 0; i < replica.rows*replica.cols; i++)
		rep.at<uchar>(i) = round(replica.at<int>(i) / accumulator.at<uchar>(i));
	// Вывод результатов на экран:
	imshow("t_1", t_1);
	imshow("t", t);
	imshow("Vectors", vector);
	imshow("Replica", rep);
	waitKey(0);
	return 0;
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

/**
* Поиска подходящего блока
*
*	В окне для поиска находится блок с наименьшим
значением SAD. Формируется переменная класса _Block
которая содержит координаты исходного блока,
координаты подходящего блока,
найденный SAD. 
Переменная выводится в качестве выходного параметра
* @t_1 - Mat_<typename> - исходное изображение,
содержащее блок для поиска
* @t - Mat_<typename> - исходное изображение,
содержащее окно для поиска
* @row - int - начальная координата окная для поиска по вертикали
* @col - int - начальная координата окная для поиска по горизонтали
* @count_row, @count_col - int - величина окна для поиска 
* @i_old, @j_old - int - координаты текущего блока
* @return _Block - информация по текущему блоку
*/
template <typename _Tp>
_Block Search_Block(Mat_<_Tp> t_1, Mat_<_Tp> t, int row, int col, int count_row, int count_col, int i_old, int j_old)
{
	_Block bl = _Block(i_old, j_old);
	double SAD = 0;
	// Проверка с аналогичном блоке на кадре t
	for (int i = 0; i < Block; i++)
	{
		for (int j = 0; j < Block; j++)
		{
			SAD += abs(t_1.at<_Tp>(i_old + i, j_old + j) - t.at<_Tp>(i_old + i, j_old + j));
		}
		if (bl.getSAD() < SAD)
			break; 
	}
	if ( SAD<10)
	{
		bl.reload(i_old, j_old, SAD, 99999);
		return bl;
	}
	for (int i = row; i <= row + count_row - Block;  i++)
		for (int j = col; j <= col + count_col - Block;  j++)
		{
			SAD = 0;
			// Расчет SAD для каждого блока в окне
			for (int ii = 0; ii < Block; ii++)
			{
				for (int jj = 0; jj < Block; jj++)
					SAD += abs(t_1.at<_Tp>(i_old + ii, j_old + jj) - t.at<_Tp>(i + ii, j + jj));
				if (bl.getSAD() < SAD)
					break; 
			}
			if (bl.getSAD() > SAD)
			{
				bl.reload(i, j, SAD, 9999);
				if (bl.getSAD() <= 10)
					return bl;
			}
			
		}
	return bl;
}

/**
* Заполение изображения заданным цветом
*
*	Исходная матрица изображения заполняется
	заданным цветом 
* @input Mat_<typename> исходное изображение
* @color int заданный цвет
*/
template <typename _Tp>
void Fill(Mat_<_Tp> &input, int color)
{
	for (int i = 0; i < input.rows*input.cols; i++)
		input.at<_Tp>(i) = color;
}

/**
* Вывод текущих блоков
*
*	Из кадра t-1 вырезается и выводится на экран 
текущий блок, из кадра t вырезается найденый 
блок с минимальным значенеим SAD
* @input _Block информация по текущему блоку
*/
template <typename _Tp>
void Cut(Mat_<_Tp> t_1, Mat_<_Tp> t, _Block bl)
{
	Mat_<_Tp> c1 =
		Mat_<_Tp>(Block, Block);
	Mat_<_Tp> c2 =
		Mat_<_Tp>(Block, Block);
	for (int i = 0; i < Block; i++)
		for (int j = 0; j < Block; j++)
		{
			c1.at<_Tp>(i, j) = t_1.at<_Tp>(bl.get_i_old() + i, bl.get_j_old() + j);
			c2.at<_Tp>(i, j) = t.at<_Tp>(bl.get_i_new() + i, bl.get_j_new() + j);
		}
	// Вывод блока из кадра t_1
	imshow("1 " + to_string(bl.get_i_old()) + " " + to_string(bl.get_j_old()) 
		+ " " + to_string(bl.get_i_new()) + " " + to_string(bl.get_j_new()), c1);
	// Вывод блока из кадра t
	imshow("2 " + to_string(bl.get_i_old()) + " " + to_string(bl.get_j_old())
		+ " " + to_string(bl.get_i_new()) + " " + to_string(bl.get_j_new()), c2);
}

template <typename _Tp, typename Tp>
Mat_<Tp> Reload(Mat_<Tp> inp, Mat_<_Tp> img, Mat_<_Tp> acc, _Block bl)
{
	for (int row = 0; row < Block; row++)
		for (int col = 0; col < Block; col++)
		{
			inp.at<Tp>(bl.get_i_old() + row, bl.get_j_old() + col)
				+= img.at<_Tp>(bl.get_i_new() + row, bl.get_j_new() + col);
			acc.at<_Tp>(bl.get_i_old() + row, bl.get_j_old() + col) += 1;
		}
	return inp;
}