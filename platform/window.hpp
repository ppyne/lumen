#pragma once

#include <memory>

class PlatformWindow {
public:
    virtual ~PlatformWindow() = default;

    virtual void pollEvents() = 0;
    virtual void swapBuffers() = 0;
    virtual bool shouldClose() const = 0;
    virtual void makeCurrent() = 0;
};

std::unique_ptr<PlatformWindow> createPlatformWindow(int width, int height);
