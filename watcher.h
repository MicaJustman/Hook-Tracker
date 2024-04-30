#pragma once

#include <thread>
#include <Windows.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Watcher {
    public:
        bool is_running = false;
        bool countdown = false;
        bool markHookState = true;
        const Point position;
        Mat template_image;
        Mat current;
        HWND hwnd;
        thread watch_thread;
        double ssim_score = 0.0;
        chrono::steady_clock::time_point DSTimer = chrono::steady_clock::now() - chrono::minutes(5);
        chrono::steady_clock::time_point hookTimer;
        chrono::seconds elapsedOffHook;
        int elapsedHook;
        int hookStates = 0;

        Watcher(Point position, cv::Mat template_image, HWND hwnd)
            : position(position),
            template_image(template_image),
            hwnd(hwnd)
        {
            startThread();
        }

        void startThread();
        int get_time();

    private:
        void _run();
};
