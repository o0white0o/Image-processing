// LR2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <math.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

#define To_Float static_cast<float>

void Matrix_multiplication(Mat &G, Mat image, int K[9]);
void Matrix_addition(Mat &G, Mat Gx, Mat Gy);
void Delete(Mat &m);
void Formulation(Mat Gx, Mat Gy, Mat G, String S);

int main(int argc, char** argv)
{
	int Sobel_operator_x[9] = { 1,0,-1,2,0,-2,1,0,-1 };
	int Sobel_operator_y[9] = { 1,2,1,0,0,0,-1,-2,-1 };
	int Prewitt_operator_x[9] = { -1,0,1,-1,0,1,-1,0,1 };
	int Prewitt_operator_y[9] = { 1,1,1,0,0,0,-1,-1,-1 };
	int Roberts_operator_x[9] = { 1,0,0,0,-1,0,0,0,0 };
	int Roberts_operator_y[9] = { 0,1,0,-1,0,0,0,0,0 };

	namedWindow("LR2");
	String imageName("LR2_3.jpg");
	if (argc > 1)
	{
		imageName = argv[1];
	}
	/// Считывание изображения в оттенках серого
	Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	Mat image_with_frame(image.rows+2, image.cols+2, CV_8UC1, Scalar(255, 255, 255));	// CV_8UC1 - 2^8 uchar = 0 до 255
	Mat Gx(image.rows, image.cols, CV_16SC1, Scalar(255, 255, 255));					// CV_16SC1 -  2^16 short = -32 768 до 32 767
	Mat Gy(image.rows, image.cols, CV_16SC1, Scalar(255, 255, 255));
	Mat G(image.rows, image.cols, CV_16SC1, Scalar(255, 255, 255));
	/// Вывод исходного изображения
	imshow("Input", image);

	/// Копия изображения с белой рамкой
	for (int i = 1; i <= image.rows; i++)
		for (int j = 1; j <= image.cols; j++)
			image_with_frame.at<uchar>(i, j) = image.at<uchar>(i-1, j-1);
	/// Заполнение белой рамки соседними цветами
	// Заполнение вехний и нижний границы
	for (int j = 0; j < image_with_frame.cols; j++)
	{
		image_with_frame.at<uchar>(0, j) = image_with_frame.at<uchar>(1, j);
		image_with_frame.at<uchar>(image_with_frame.rows-1, j) = image_with_frame.at<uchar>(image_with_frame.rows-2, j);
	}
	// Заполнение левой и правой границы
	for (int i = 0; i < image_with_frame.rows; i++)
	{
		image_with_frame.at<uchar>(i, 0) = image_with_frame.at<uchar>(i, 1);
		image_with_frame.at<uchar>(i, image_with_frame.cols - 1) = image_with_frame.at<uchar>(i, image_with_frame.cols - 2);
	}
	/*-----------=== Часть 1 (Оператор Собеля) ===-----------*/
	Matrix_multiplication(Gx, image_with_frame, Sobel_operator_x);
	Matrix_multiplication(Gy, image_with_frame, Sobel_operator_y);
	Formulation(Gx, Gy, G, "Sobel operator");
	/// Обнуление переменных
	Delete(Gx);
	Delete(Gy);
	Delete(G);

	/*-----------=== Часть 2 (Оператор Прюитт) ===-----------*/
	Matrix_multiplication(Gx, image_with_frame, Prewitt_operator_x);
	Matrix_multiplication(Gy, image_with_frame, Prewitt_operator_y);
	Formulation(Gx, Gy, G, "Prewitt operator");
	/// Обнуление переменных
	Delete(Gx);
	Delete(Gy);
	Delete(G);

	/*-----------=== Часть 3 (Оператор Робертса) ===-----------*/
	Matrix_multiplication(Gx, image_with_frame, Roberts_operator_x);
	Matrix_multiplication(Gy, image_with_frame, Roberts_operator_y);
	Formulation(Gx, Gy, G, "Roberts cross");

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
/// Умножение матриц
void Matrix_addition(Mat &G, Mat Gx, Mat Gy)
{
	for (int i = 0; i < G.rows * G.cols; i++)
		G.at<short>(i) = sqrt((To_Float(Gx.at<short>(i)))*(To_Float(Gx.at<short>(i)))
			+ (To_Float(Gy.at<short>(i)))*(To_Float(Gy.at<short>(i))));
}

/// Обнуление Mat (Белый фон)
void Delete(Mat &m)
{
	for (int i = 0; i < m.cols*m.rows; i++)
		switch (m.type())
		{
		case CV_8UC1:	m.at<uchar>(i) = 255;
			break;
		case CV_8SC1:	m.at<char>(i) = 127;
			break;
		case CV_16UC1:	m.at<ushort>(i) = 65535;
			break;
		case CV_16SC1:	m.at<short>(i) = 32767;
			break;
		default:
			break;
		}
}

/// Формализация
void Formulation(Mat Gx, Mat Gy, Mat G, String S)
{
	Matrix_addition(G, Gx, Gy);
	Mat G_uchar(G.rows, G.cols, CV_8UC1, Scalar(255, 255, 255));
	Mat Gx_uchar(Gx.rows, Gx.cols, CV_8UC1, Scalar(255, 255, 255));
	Mat Gy_uchar(Gy.rows, Gy.cols, CV_8UC1, Scalar(255, 255, 255));
	Gx.convertTo(Gx_uchar, CV_8UC1);
	Gy.convertTo(Gy_uchar, CV_8UC1);
	G.convertTo(G_uchar, CV_8UC1);
	/// Вывод полученных изображений на экран
	imshow(S + " : Gx", Gx_uchar);
	imshow(S + " : Gy", Gy_uchar);
	imshow(S + " : G", G_uchar);
}