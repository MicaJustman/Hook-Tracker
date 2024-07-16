#pragma once

#include <Windows.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Watcher {
    public:
        bool countdown = false;
        const Point position;
        Mat hook_template;
        Mat cage_template;
        Mat current;
        HWND hwnd;
        double ssim_score = 0.0;
        double ssim_test = 0.0;
        chrono::steady_clock::time_point DSTimer = chrono::steady_clock::now() - chrono::minutes(5);
        chrono::steady_clock::time_point flashTimer = chrono::steady_clock::now() - chrono::minutes(5);
        chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
        chrono::seconds elapsedOffHook = chrono::minutes(0);
        int elapsedHook = 0;
        int hookStates = 0;
        //int lastSave = 0;

        Watcher(Point position, cv::Mat hook_template, cv::Mat cage_template, HWND hwnd)
            : position(position),
            hook_template(hook_template),
            cage_template(cage_template),
            hwnd(hwnd)
        {}

        int get_time();
        void run(); 
};
