#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <vector> 
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	/*-----------------== ����� 1 ==-----------------*/
	namedWindow("LR1");
	String imageName("LR1_2.jpg");
	if (argc > 1)
	{
		imageName = argv[1];
	}
	/// ���������� ����������� � �������� ������
	Mat image = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	/// �������� ������� ��������
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}
	/// ����� �� ����� ��������������� �����������
	imshow("Before", image);

	int hist_size = 256;												// ����� ��������� � �����������
	int hist_width = 3;													// ��� ����������� ������ �����������
	float range_0[] = { 0,256 };
	const float* ranges = { range_0 };
	int hist_w = hist_size*hist_width; int hist_h = 400;				// ������ � ������ ������� ��� ��������� �����������
	int bin_w = round((double)hist_w / hist_size);						// ������ ��������� �����������											
	/// ���������� �����������
	Mat hist;
	bool uniform = true, accumulate = false;
	calcHist(&image, 1, 0, Mat(), hist, 1, &hist_size, &ranges, uniform, accumulate);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

	Mat copy_hist,hist_without_Norm;									// ����� �����������
	hist.copyTo(copy_hist);												// ��� ���������� 5%
	hist.copyTo(hist_without_Norm);										// ��� ������ ����� ������
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(hist.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// ����� ����������� ��������� �����������
	imshow("Histogram of the initial image", histImage);
	
	float b = 255, a = 0;												// b - ������� �������, a - ������ ������� �������� �����������
	float d = 255, c = 0;												// d - ������� �������, c - ������ ������� �������������� �����������
	double FivePercent = round((double)(image.cols*image.rows)*0.05);	// 5% �� ������ ����� �������� ��������� �����������
	double sum_for_del = 0;												// ���������� ��������� ��������

	for (; sum_for_del < FivePercent;)
		if (copy_hist.at<float>(a) > copy_hist.at<float>(b))			// ������������ ���������� � ������� �������
		{
			sum_for_del += copy_hist.at<float>(b);
			copy_hist.at<float>(b) = 0;
			b--;
		}
		else
		{
			sum_for_del += copy_hist.at<float>(a);
			copy_hist.at<float>(a) = 0;
			a++;
		}

	Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
	normalize(copy_hist, copy_hist, 0, histImage2.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage2, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(copy_hist.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// ����� ����������� ����� ������������ 5%
	imshow("Histogram after 5%", histImage2);
	/// ������ �������� � �������� ����������� 
	for (int i = 0; i < image.rows*image.cols; i++)
	{
		image.at<unsigned char>(i) = (double(image.at<unsigned char>(i)) - a)*((d - c) / (b - a)) + c;
	}
	/// ����� ����������� �����������
	imshow("After", image);
	/// ���������� �������������� �����������
	Mat hist_rez;														// �������������� �����������		
	calcHist(&image, 1, 0, Mat(), hist_rez, 1, &hist_size, &ranges, uniform, accumulate);
	Mat histImage3(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
	normalize(hist_rez, hist_rez, 0, histImage3.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage3, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(hist_rez.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// ����� �������������� �����������
	imshow("Resulting histogram", histImage3);
	/*-----------------== ����� 2 ==-----------------*/
	Mat image2 = imread(imageName.c_str(), IMREAD_GRAYSCALE);
	vector<float> CDF;
	CDF.push_back(hist_without_Norm.at<float>(0));
	for (int i = 1; i < hist_without_Norm.rows*hist_without_Norm.cols; i++)
	{
		CDF.push_back(CDF.at(i - 1) + hist_without_Norm.at<float>(i) );
	}
	/// ������ �������� ��������
	for (int i = 0; i < image2.rows*image2.cols; i++)
	{
		image2.at<unsigned char>(i) = round(((CDF.at(double(image2.at<unsigned char>(i)))-1)/ (image2.rows*image2.cols -1))*255);
	}
	/// ����� ����������� �����������
	imshow("After2", image2);
	/// ���������� �������������� ����������� ����� �����������
	Mat hist_equalization;												// �������������� �����������		
	calcHist(&image2, 1, 0, Mat(), hist_equalization, 1, &hist_size, &ranges, uniform, accumulate);
	Mat histImage4(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
	normalize(hist_equalization, hist_equalization, 0, histImage4.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < hist_size; i++)
		rectangle(histImage4, Point(i*bin_w, hist_h),
			Point((i + 1)*bin_w, hist_h - round(hist_equalization.at<float>(i - 1))),
			Scalar(100, 0, 0), 1, 8, 0.5);
	/// ����� �������������� �����������
	imshow("Resulting histogram befor equalization", histImage4);
	waitKey(0);
	return 0;
}
