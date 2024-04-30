#include <opencv2/opencv.hpp>
#include "watcher.h"
#include "grab_screen.h"

using namespace std;
using namespace cv;

double templateMatch(Mat img1, Mat img2) {
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Mat result;

	result.create(img1.cols, img1.rows, CV_32FC1);
	matchTemplate(img1, img2, result, TM_CCOEFF_NORMED);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	return maxVal;
}

void Watcher::startThread() {
	if (!is_running) {
		is_running = true;
		watch_thread = std::thread(&Watcher::_run, this);
	}
}

int Watcher::get_time() {
	elapsedHook = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - DSTimer).count();
	if (60 >= elapsedHook > 0) {
		return 60 - elapsedHook;
	}
	else {
		return 0;
	}
}

void Watcher::_run() {
	while (is_running) {
		if (GetAsyncKeyState('P') & 0x8000) {
			hookStates = 0;
			DSTimer -= chrono::minutes(5);
		}

		current = getMat(hwnd, 40, 40, position.x, position.y);
		cvtColor(current, current, COLOR_BGR2GRAY);
		threshold(current, current, 130, 255, THRESH_BINARY);
		ssim_score = templateMatch(current, template_image);

		this_thread::sleep_for(chrono::milliseconds(300));

		if (ssim_score >= 0.3) {
			if (!countdown) {
				DSTimer -= chrono::minutes(5);
				hookTimer = chrono::steady_clock::now();
				cout << ssim_score << endl;

				if (markHookState) {
					hookStates += 1;
					markHookState = false;
				}
			}
			else if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - hookTimer).count() > 60) {
				hookStates += 1;
				hookTimer = chrono::steady_clock::now();
			}


			countdown = true;
		}
		else if (ssim_score < 0.3 && countdown) {
			DSTimer = chrono::steady_clock::now();
			countdown = false;
			markHookState = true;

			if (hookStates == 3) {
				DSTimer -= chrono::minutes(5);
			}
		}
	}
}