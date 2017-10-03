// LR2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	namedWindow("LR2");
	String imageName("LR1.jpg");
	if (argc > 1)
	{
		imageName = argv[1];
	}
	/// Считывание изображения в оттенках серого
	Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	Mat image_with_frame(image.rows+2, image.cols+2, CV_8UC1, Scalar(255, 255, 255));
	Mat Gx(image.rows, image.cols, CV_16UC1, Scalar(255, 255, 255));	// CV_16UC1 -  2^16 short = -32 768 до 32 767
	Mat Gy(image.rows, image.cols, CV_16UC1, Scalar(255, 255, 255));

	/// Копия изображения с белой рамкой
	for (int i = 1; i <= image.rows; i++)
		for (int j = 1; j <= image.cols; j++)
			image_with_frame.at<unsigned char>(i, j) = image.at<unsigned char>(i-1, j-1);
	
	/// Заполнение белой рамки соседними цветами
	// Заполнение вехний и нижний границы
	for (int j = 0; j < image_with_frame.cols; j++)
	{
		image_with_frame.at<unsigned char>(0, j) = image_with_frame.at<unsigned char>(1, j);
		image_with_frame.at<unsigned char>(image_with_frame.rows-1, j) = image_with_frame.at<unsigned char>(image_with_frame.rows-2, j);
	}
	// Заполнение левой и правой границы
	for (int i = 0; i < image_with_frame.rows; i++)
	{
		image_with_frame.at<unsigned char>(i, 0) = image_with_frame.at<unsigned char>(i, 1);
		image_with_frame.at<unsigned char>(i, image_with_frame.cols - 1) = image_with_frame.at<unsigned char>(i, image_with_frame.cols - 2);
	}
	/*for (int i = 0; i < image_with_frame.rows; i++)
	{
		for (int j = 0; j < image_with_frame.cols; j++)
			cout << static_cast<float>(image_with_frame.at<unsigned char>(i, j)) << " ";
		cout << endl;
	}*/
	imshow("1", image);
	imshow("2", image_with_frame);
	//Gx.at<short>(0) = -1000;
	//cout << Gx.at<short>(0)<< " "<< static_cast<float>(Gx.at<short>(0));

	waitKey(0);
    return 0;
}

