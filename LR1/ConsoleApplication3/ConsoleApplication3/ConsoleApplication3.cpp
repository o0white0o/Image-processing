#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <vector> 
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	/*-----------------== Часть 1 ==-----------------*/
	namedWindow("LR1");
	String imageName("LR1_2.jpg");
	if (argc > 1)
	{
		imageName = argv[1];
	}
	/// Считывание изображения в оттенках серого
	Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	/// Проверка входных значений
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}
	/// Вывод на экран первоначального изображения
	imshow("Before", image);

	int hist_size = 256;														// Число элементов в гистограмме
	int hist_width = 3;															// Для автоподбора ширины изображений
	float range_0[] = { 0,256 };
	const float* ranges = { range_0 };
	int hist_w = hist_size*hist_width; int hist_h = 400;						// Ширина и высота полотна для нанесения гистограммы
	int bin_w = round((double)hist_w / hist_size);								// Ширина элементов гистограммы											
	/// Построение гистограммы
	Mat hist;
	bool uniform = true, accumulate = false;
	calcHist(&image, 1, 0, Mat(), hist, 1, &hist_size, &ranges, uniform, accumulate);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

	Mat copyHist,hist_without_Norm;																// Копия гистограммы
	hist.copyTo(copyHist);
	hist.copyTo(hist_without_Norm);
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(hist.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// Вывод гистограммы исходного изображения
	imshow("Histogram of the initial image", histImage);

	float b = 255, a = 0;														// b - верхняя граница, a - нижняя граница исходной гистограммы
	float d = 255, c = 0;														// d - верхняя граница, c - нижняя граница результирующей гистограммы
	double FivePercent = round((double)(image.cols*image.rows)*0.05);			// 5% от общего числа пикселей исходного изображения
	double sum_for_del = 0;														// количество удаленных пикселей

	for (; sum_for_del < FivePercent;)
		if (copyHist.at<float>(a) > copyHist.at<float>(b))						//Сравниваем последний и первый элемент
		{
			sum_for_del += copyHist.at<float>(b);
			copyHist.at<float>(b) = 0;
			b--;
		}
		else
		{
			sum_for_del += copyHist.at<float>(a);
			copyHist.at<float>(a) = 0;
			a++;
		}

	Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
	normalize(copyHist, copyHist, 0, histImage2.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage2, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(copyHist.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// Вывод гистограммы после отбрасывания 5%
	imshow("Histogram after 5%", histImage2);

	for (int i = 0; i < image.rows*image.cols; i++)
	{
		image.at<unsigned char>(i) = (double(image.at<unsigned char>(i)) - a)*((d - c) / (b - a)) + c;
	}
	imshow("After", image);
	/// Построение результирующей гистограммы
	Mat hist_rez;
	calcHist(&image, 1, 0, Mat(), hist_rez, 1, &hist_size, &ranges, uniform, accumulate);
	Mat histImage3(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
	normalize(hist_rez, hist_rez, 0, histImage3.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage3, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(hist_rez.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// Вывод результирующей гистограммы
	imshow("Resulting histogram", histImage3);
	/*-----------------== Часть 2 ==-----------------*/
	Mat image2 = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	vector<float> CDF;
	CDF.push_back(hist_without_Norm.at<float>(0));
	for (int i = 1; i < hist_without_Norm.rows*hist_without_Norm.cols; i++)
	{
		CDF.push_back(CDF.at(i - 1) + hist_without_Norm.at<float>(i) );
	}
	for (int i = 0; i < image2.rows*image2.cols; i++)
	{
		image2.at<unsigned char>(i) = (((CDF.at(double(image2.at<unsigned char>(i)))-1)/ (image2.rows*image2.cols -1))*255);
	}
	imshow("After2", image2);
	waitKey(0);
	return 0;
}
