#include <opencv2/opencv.hpp>
#include "watcher.h"
#include "grab_screen.h"

using namespace std;
using namespace cv;

int Watcher::get_time() {
	elapsedHook = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - DSTimer).count();
	if (60 >= elapsedHook > 0) {
		return 60 - elapsedHook;
	}
	else {
		return 0;
	}
}

void Watcher::run() {
	if (GetAsyncKeyState('P') & 0x8000) {
		hookStates = 0;
		DSTimer -= chrono::minutes(5);
	}

	/*temp to grab faces
	int seconds = chrono::duration_cast<std::chrono::seconds>(chrono::steady_clock::now() - startTime).count();
	cout << seconds << endl;
	if (seconds % 60 == 0 and seconds != lastSave) {
		imwrite("training/" + to_string(fileCount) + ".png", current);
		fileCount += 1;
		lastSave = seconds;
	}
	*/

	current = getMat(hwnd, 40, 40, position.x, position.y);
	cvtColor(current, current, COLOR_BGR2GRAY);
	threshold(current, current, 130, 255, THRESH_BINARY);

	ssim_score = templateMatch(current, hook_template);

	if (ssim_score < .45) {
		ssim_score = templateMatch(current, cage_template);
	}

	if (ssim_score >= 0.45) {
		DSTimer -= chrono::minutes(5);

		if (chrono::steady_clock::now() - flashTimer > chrono::seconds(5)) {
			Mat frame = getMat(hwnd, 80, 4, position.x + 64, position.y + 40);
			uchar* framedata = frame.data;

			int reds = 0;

			for (int row = 0; row < frame.rows; ++row) {
				for (int col = 0; col < frame.cols; ++col) {
					if (framedata[(row * frame.cols + col) * 4] < 15 and framedata[(row * frame.cols + col) * 4 + 1] < 15 and framedata[(row * frame.cols + col) * 4 + 2] > 190) {
						reds++;
					}
				}
			}

			if (reds > 10) {
				hookStates += 1;
				flashTimer = chrono::steady_clock::now();
				imwrite("Snap Shots/" + to_string(position.y) + "_" + to_string(hookStates) + ".png", frame);
			}

		}

		countdown = true;
	}
	else if (ssim_score < 0.3 and countdown) {
		DSTimer = chrono::steady_clock::now();
		countdown = false;

		if (hookStates == 3) {
			DSTimer -= chrono::minutes(5);
		}
	}
}
