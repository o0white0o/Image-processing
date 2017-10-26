/// Лабораторная работа №3 
/// по курсу Методы и средства цифровой обработки информации
/// Выполено студенткой группы 13541/1 Калугиной М.О.
/// Преподаватель Абрамов Н.А.

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <math.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

#define To_Float static_cast<float>

void Matrix_multiplication(Mat &G, Mat image, int K[9]);
Mat Frame(Mat input);
Mat Convert(Mat input);

int main()
{
	int Laplacian3x3[9] = { -1,-1,-1,-1,8,-1,-1,-1,-1 };
	int Gauss_filter[9] = { 1,2,1,2,4,2,1,2,1 };

	namedWindow("LR3");
	String imageName("LR3_2.png");
	// Считывание изображения в оттенках серого

	Mat image_input = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	Mat image_С1(image_input.rows,					// Изображение для нахождения границ
		image_input.cols, CV_16SC1, Scalar(255, 255, 255));
	Mat image_С2(image_input.rows,					// Изображение для повышения резкости
		image_input.cols, CV_32FC1, Scalar(255, 255, 255));
	// Вывод исходного изображения на экран
	imshow("input", image_input);

	/*-----------=== Часть 1 ===-----------*/
	// Применение Лапласяна без сглаживания
	Mat image_with_frame = Frame(image_input);		// Создание изображения с рамкой
	Matrix_multiplication(image_С1, image_with_frame, Laplacian3x3);
	imshow("OL without smoothing", Convert(image_С1));
	// Применение фильтра Гауса 3х3  
	// для сглаживания изображения
	image_with_frame = Frame(image_input);			// Создание изображения с рамкой
	Matrix_multiplication(image_С1, image_with_frame, Gauss_filter);
	normalize(image_С1, image_С1, -32767,			// Линейное растяжение 
		32767, NORM_MINMAX, -1, Mat());
	for (int i = 0; i < image_С1.rows*image_С1.cols; i++)
		image_С1.at<short>(i) = round(float(image_С1.at<short>(i)) / 16);
	imshow("Gauss filter", Convert(image_С1));
	// Применение Лапласиана для выделения границ
	image_with_frame = Frame(Convert(image_С1));	// Создание изображения с рамкой
	Matrix_multiplication(image_С1, image_with_frame, Laplacian3x3);
	imshow("OL with smoothing", Convert(image_С1));
	
	/*-----------=== Часть 2 ===-----------*/
	// Повышение резкости
	normalize(image_С1, image_С1, 10,				// от 1 до 1.3 
		13, NORM_MINMAX, -1, Mat());
	for (int i = 0; i < image_С2.rows*image_С2.cols; i++)
		image_С2.at<float>(i) =						// input * Kоэф_резкости
		round(To_Float(image_input.at<uchar>(i))*To_Float(image_С1.at<short>(i)) / 10);
	imshow("High Boost Filter", Convert(image_С2));
	waitKey(0);
	return 0;
}

///	[k0 k1 k2]
///	[k3 k4 k5]
///	[k6 k7 k8]
void Matrix_multiplication(Mat &G, Mat image, int K[9])
{
	for (int i = 1; i < image.rows - 1; i++)
		for (int j = 1; j < image.cols - 1; j++)
			G.at<short>(i - 1, j - 1) =
			K[0] * To_Float(image.at<uchar>(i - 1, j - 1)) + K[1] * To_Float(image.at<uchar>(i - 1, j)) + K[2] * To_Float(image.at<uchar>(i - 1, j + 1))
			+ K[3] * To_Float(image.at<uchar>(i - 1, j - 1)) + K[4] * To_Float(image.at<uchar>(i, j)) + K[5] * To_Float(image.at<uchar>(i - 1, j + 1))
			+ K[6] * To_Float(image.at<uchar>(i + 1, j + 1)) + K[7] * To_Float(image.at<uchar>(i + 1, j)) + K[8] * To_Float(image.at<uchar>(i + 1, j + 1));
}

/// Создания рамки
/// input - исходное изображение размером MxN
/// output - выходное изображение с рамкой M+2xN+2
/// Рамка заполнена соседними пикселями
Mat Frame(Mat input)
{
	Mat output(input.rows + 2, input.cols + 2,
		CV_8UC1, Scalar(255, 255, 255));
	// Копия изображения с белой рамкой:
	for (int i = 1; i <= input.rows; i++)
		for (int j = 1; j <= input.cols; j++)
			output.at<uchar>(i, j) = input.at<uchar>(i - 1, j - 1);
	// Заполнение белой рамки соседними цветами:
	// Заполнение вехний и нижний границы
	for (int j = 0; j < output.cols; j++)
	{
		output.at<uchar>(0, j) = output.at<uchar>(1, j);
		output.at<uchar>(output.rows - 1, j) = output.at<uchar>(output.rows - 2, j);
	}
	// Заполнение левой и правой границы
	for (int i = 0; i < output.rows; i++)
	{
		output.at<uchar>(i, 0) = output.at<uchar>(i, 1);
		output.at<uchar>(i, output.cols - 1) = output.at<uchar>(i, output.cols - 2);
	}
	return output;
}

/// CV_16SC1 to CV_8UC1
Mat Convert(Mat input)
{
	Mat output(input.rows, input.cols,
		CV_8UC1, Scalar(255, 255, 255));
	normalize(input, input, 0,			// Линейное растяжение 
		255, NORM_MINMAX, -1, Mat());
	input.convertTo(output, CV_8UC1);
	return output;
}
