// LR4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace cv;

int main()
{
	namedWindow("LR4");
	String filename("LR4.avi");		// Имя видеофрагмента
	CvCapture* capture = cvCreateFileCapture(filename.c_str());
	Mat_<uchar> image = cvarrToMat(cvQueryFrame(capture));
	waitKey(0);
	return 0;
}

