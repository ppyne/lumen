#pragma once

#include <memory>

#include "../platform/window.hpp"

class Application {
public:
    Application(int width, int height);
    void run();
    void quit();

private:
    std::unique_ptr<PlatformWindow> window_;
    bool running_ = true;
};
