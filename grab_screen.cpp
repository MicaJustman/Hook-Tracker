#include "grab_screen.h"
#include <Windows.h>

using namespace std;

double minVal; double maxVal; Point minLoc; Point maxLoc;
Mat result;

Mat getMat(HWND hwnd, int width, int height, int offset_width, int offset_height) {
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

double templateMatch(Mat img1, Mat img2) {
	result.create(img2.cols, img2.rows, CV_32FC1);
	matchTemplate(img1, img2, result, TM_CCOEFF_NORMED);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	return maxVal;
}