#include "application.hpp"

#include <chrono>
#include <thread>

#include "gl.hpp"

Application::Application(int width, int height)
    : window_(createPlatformWindow(width, height)) {
}

void Application::run() {
    while (running_ && !window_->shouldClose()) {
        window_->pollEvents();
        window_->makeCurrent();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window_->swapBuffers();

        // Avoid a busy spin when idle; keep it light for responsiveness.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Application::quit() {
    running_ = false;
}
