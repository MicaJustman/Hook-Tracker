#pragma once

#include <opencv2/opencv.hpp>
#include <Windows.h>

using namespace cv;

Mat getMat(HWND hwnd, int width, int height, int offset_width, int offset_height);

double templateMatch(Mat img1, Mat img2);