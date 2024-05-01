#include <iostream>
#include <SFML/Graphics.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include "watcher.h"


using namespace std;
using namespace cv;

int main()
{
    HWND hwnd = GetDesktopWindow();
    RECT windowRect;
    GetClientRect(hwnd, &windowRect);
    int width = windowRect.right;
    int height = windowRect.bottom;

    int split = 70;
    Mat template_image = imread("hook.png", IMREAD_GRAYSCALE);

    std::vector<Point> positions;
    for (int y = 559; y < 559 + split * 4; y += split) {
        positions.emplace_back(82, y);
    }

    Watcher watchers[4] = {
        Watcher(positions[0], template_image, hwnd),
        Watcher(positions[1], template_image, hwnd),
        Watcher(positions[2], template_image, hwnd),
        Watcher(positions[3], template_image, hwnd)
    };

    sf::RenderWindow window(
        sf::VideoMode(160, 280),
        "Borderless Window",
        sf::Style::None
    );

    HWND hwndOver = window.getSystemHandle();

    SetWindowLong(hwndOver, GWL_EXSTYLE,
        GetWindowLong(hwndOver, GWL_EXSTYLE)
        | WS_EX_LAYERED
        | WS_EX_TRANSPARENT);

    SetWindowPos(hwndOver, HWND_TOPMOST, 70, 545, 160, 280, SWP_NOSIZE);
    SetLayeredWindowAttributes(hwndOver, RGB(0, 0, 0), 0, LWA_COLORKEY);
    SetForegroundWindow(hwnd);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    int radius1 = 28.0;
    sf::CircleShape circle1(radius1);
    int radius2 = 26.0;
    sf::CircleShape circle2(radius2);
    sf::Color type;
    circle2.setFillColor(sf::Color::Black);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        window.clear();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
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

            circle1.setPosition(watchers[i].position.x + 21 - radius1 - 70, watchers[i].position.y + 20 - radius1 - 545);
            circle2.setPosition(watchers[i].position.x + 21 - radius2 - 70, watchers[i].position.y + 20 - radius2 - 545);
            circle1.setFillColor(type);
            window.draw(circle1);
            window.draw(circle2);
            
            text.setString(to_string(watchers[i].get_time()));
            text.setPosition(watchers[i].position.x + 54, watchers[i].position.y - 550);
            window.draw(text);
        }

        window.display();
        Sleep(900);
    }
    return 0;
}