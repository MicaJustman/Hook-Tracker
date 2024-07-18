#include <iostream>
#include <SFML/Graphics.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include "watcher.h"
#include "grab_screen.h"
#include <filesystem>


using namespace std;
using namespace cv;

//int fileCount = 0;

int main()
{
    /*
    for (const auto& entry : filesystem::directory_iterator("training")) {
        if (entry.is_regular_file()) {
            fileCount++;
        }
    }
    */

    HWND hwnd = GetDesktopWindow();
    LPCWSTR windowTitle = L"DeadByDaylight  ";

    RECT windowRect;
    GetClientRect(hwnd, &windowRect);
    int width = windowRect.right;
    int height = windowRect.bottom;

    int split = 70;
    Mat hook_template = imread("hook.png");
    cvtColor(hook_template, hook_template, COLOR_RGB2GRAY);
    threshold(hook_template, hook_template, 130, 255, THRESH_BINARY);

    Mat cage_template = imread("cage.png");
    cvtColor(cage_template, cage_template, COLOR_RGB2GRAY);
    threshold(cage_template, cage_template, 130, 255, THRESH_BINARY);

    Mat skull_template = imread("skull.png");
    cvtColor(skull_template, skull_template, COLOR_RGB2GRAY);
    threshold(skull_template, skull_template, 130, 255, THRESH_BINARY);

    Mat stop_template = imread("stop.png");
    cvtColor(stop_template, stop_template, COLOR_RGB2GRAY);
    Mat start_template = imread("start.png");
    cvtColor(start_template, start_template, COLOR_RGB2GRAY);

    std::vector<Point> positions;
    for (int y = 559; y < 559 + split * 4; y += split) {
        positions.emplace_back(82, y);
    }

    //position adjustment because DBD does nothing with consistancy
    positions[0].y -= 1;
    positions[3].y += 1;

    Watcher watchers[4] = {
        Watcher(positions[0], hook_template, cage_template, skull_template, hwnd),
        Watcher(positions[1], hook_template, cage_template, skull_template, hwnd),
        Watcher(positions[2], hook_template, cage_template, skull_template, hwnd),
        Watcher(positions[3], hook_template, cage_template, skull_template, hwnd)
    };

    sf::RenderWindow window(
        sf::VideoMode(160, 290),
        "Borderless Window",
        sf::Style::None
    );

    HWND hwndOver = window.getSystemHandle();

    SetWindowLong(hwndOver, GWL_EXSTYLE,
        GetWindowLong(hwndOver, GWL_EXSTYLE)
        | WS_EX_LAYERED
        | WS_EX_TRANSPARENT);

    SetWindowPos(hwndOver, HWND_TOPMOST, 70, 535, 160, 290, SWP_NOSIZE);
    SetLayeredWindowAttributes(hwndOver, RGB(0, 0, 0), 0, LWA_COLORKEY);
    SetForegroundWindow(hwnd);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    float radius1 = 28.0;
    sf::CircleShape circle1(radius1);
    float radius2 = 26.0;
    sf::CircleShape circle2(radius2);
    sf::Color type;
    circle2.setFillColor(sf::Color::Black);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);

    Mat current;
    bool ingame = false;

    while (window.isOpen()) {
        //chrono::steady_clock::time_point start = chrono::steady_clock::now();
        window.clear();

        if (GetAsyncKeyState('I') & 0x8000) {
            ingame = true;
        }

        if (GetAsyncKeyState('O') & 0x8000) {
            ingame = false;
        }

        if (!ingame) {
            current = getMat(hwnd, 800, 60, 563, 964);
            cvtColor(current, current, COLOR_RGB2GRAY);

            if (templateMatch(current, start_template) > .95) {
                ingame = true;
            }
        }

        if (GetForegroundWindow() == FindWindow(NULL, windowTitle) and ingame) {
            current = getMat(hwnd, 270, 80, 80, 100);
            cvtColor(current, current, COLOR_RGB2GRAY);

            if (templateMatch(current, stop_template) > .9) {
                ingame = false;
            }

            for (int i = 0; i < 4; i++) {
                watchers[i].run();

                if (!ingame or (GetAsyncKeyState('P') & 0x8000)) {
                    watchers[i].reset();
                }
            }

            for (int i = 0; i < 4; i++) {
                if (watchers[i].hookStates == 0) {
                    type = sf::Color::Green;
                }
                else if (watchers[i].hookStates == 1) {
                    type = sf::Color::Blue;
                }
                else {
                    type = sf::Color::Red;
                }

                circle1.setPosition(watchers[i].position.x + 21 - radius1 - 70, watchers[i].position.y + 20 - radius1 - 535);
                circle2.setPosition(watchers[i].position.x + 21 - radius2 - 70, watchers[i].position.y + 20 - radius2 - 535);
                circle1.setFillColor(type);
                window.draw(circle1);
                window.draw(circle2);

                text.setString(to_string(watchers[i].get_time()));
                text.setPosition(watchers[i].position.x - 10, watchers[i].position.y - 545);
                window.draw(text);
            } 
        }
        
        window.display();
        //cout << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() << endl;
    }

    return 0;
}