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
	stored = current;
	cvtColor(current, current, COLOR_BGR2GRAY);
	threshold(current, current, 130, 255, THRESH_BINARY);

	ssim_score = templateMatch(current, hook_template);

	if (ssim_score < .45) {
		ssim_score = templateMatch(current, cage_template);
	}

	if (ssim_score >= 0.45) {
		if (chrono::steady_clock::now() - flashTimer > chrono::seconds(5)) {
			uchar* framedata = stored.data;
			int reds = 0;

			for (int row = 0; row < stored.rows; ++row) {
				for (int col = 0; col < stored.cols; ++col) {
					if (framedata[(row * stored.cols + col) * 4] < 30 and framedata[(row * stored.cols + col) * 4 + 1] < 30 and framedata[(row * stored.cols + col) * 4 + 2] > 150) {
						reds++;
					}
				}
			}

			if (reds > 100) {
				cout << reds << endl;
				flashTimer = chrono::steady_clock::now();
				imwrite("Snap Shots/" + to_string(position.y) + "_" + to_string(hookStates) + ".png", stored);
				hookStates += 1;
				countdown = true;
			}
		}

	}
	else if (ssim_score < 0.3 and countdown) {
		DSTimer = chrono::steady_clock::now();
		countdown = false;

		if (hookStates == 3) {
			DSTimer -= chrono::minutes(5);
		}
	}
}
