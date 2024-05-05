#pragma once

#include <Windows.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Watcher {
    public:
        bool countdown = false;
        bool markHookState = true;
        const Point position;
        Mat template_image;
        Mat current;
        HWND hwnd;
        double ssim_score = 0.0;
        chrono::steady_clock::time_point DSTimer = chrono::steady_clock::now() - chrono::minutes(5);
        chrono::steady_clock::time_point hookTimer;
        chrono::seconds elapsedOffHook = chrono::minutes(0);
        int elapsedHook = 0;
        int hookStates = 0;

        Watcher(Point position, cv::Mat template_image, HWND hwnd)
            : position(position),
            template_image(template_image),
            hwnd(hwnd)
        {}

        int get_time();
        void run(); 
};
