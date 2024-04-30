#pragma once

#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <mutex>

using namespace cv;
using namespace std;

mutex globalMutex;

Mat getMat(HWND hwnd, int width, int height, int offset_width, int offset_height) {

	lock_guard<mutex> lock(globalMutex);

	HDC deviceContext = GetDC(hwnd);
	HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);
	HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);
	SelectObject(memoryDeviceContext, bitmap);
	BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, offset_width, offset_height, SRCCOPY);
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1;
	bi.biYPelsPerMeter = 2;
	bi.biClrUsed = 3;
	bi.biClrUsed = 4;

	Mat mat = Mat(height, width, CV_8UC4);
	GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteObject(bitmap);
	DeleteDC(memoryDeviceContext);
	ReleaseDC(hwnd, deviceContext);

	return mat;
}