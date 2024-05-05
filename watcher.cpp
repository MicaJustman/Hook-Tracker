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

	current = getMat(hwnd, 40, 40, position.x, position.y);
	cvtColor(current, current, COLOR_BGR2GRAY);
	threshold(current, current, 130, 255, THRESH_BINARY);
	ssim_score = templateMatch(current, template_image);

	if (ssim_score >= 0.3) {
		if (!countdown) {
			DSTimer -= chrono::minutes(5);
			hookTimer = chrono::steady_clock::now();

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
	else if (ssim_score < 0.3 and countdown) {
		DSTimer = chrono::steady_clock::now();
		countdown = false;
		markHookState = true;

		if (hookStates == 3) {
			DSTimer -= chrono::minutes(5);
		}
	}
}
