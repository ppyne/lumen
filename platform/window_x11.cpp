#if defined(__linux__) && !defined(__APPLE__)

#include "window.hpp"

#include <stdexcept>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

namespace {

class X11Window : public PlatformWindow {
public:
    X11Window(int width, int height) {
        // X11 window + GLX context (fallback-friendly for Phase 1).
        display_ = XOpenDisplay(nullptr);
        if (!display_) {
            throw std::runtime_error("Failed to open X11 display");
        }

        int screen = DefaultScreen(display_);

        int attribs[] = {
            GLX_RGBA,
            GLX_DOUBLEBUFFER,
            GLX_DEPTH_SIZE, 24,
            None
        };

        XVisualInfo* vi = glXChooseVisual(display_, screen, attribs);
        if (!vi) {
            throw std::runtime_error("Failed to choose X11 visual");
        }

        Colormap cmap = XCreateColormap(display_, RootWindow(display_, screen), vi->visual, AllocNone);

        XSetWindowAttributes swa = {};
        swa.colormap = cmap;
        swa.event_mask = ExposureMask | StructureNotifyMask;

        window_ = XCreateWindow(
            display_,
            RootWindow(display_, screen),
            0, 0,
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height),
            0,
            vi->depth,
            InputOutput,
            vi->visual,
            CWColormap | CWEventMask,
            &swa);

        XStoreName(display_, window_, "Lumen");

        wm_delete_ = XInternAtom(display_, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display_, window_, &wm_delete_, 1);

        XMapWindow(display_, window_);

        context_ = glXCreateContext(display_, vi, nullptr, GL_TRUE);
        glXMakeCurrent(display_, window_, context_);

        glViewport(0, 0, width, height);

        XFree(vi);
    }

    ~X11Window() override {
        if (context_) {
            glXMakeCurrent(display_, None, nullptr);
            glXDestroyContext(display_, context_);
        }
        if (window_) {
            XDestroyWindow(display_, window_);
        }
        if (display_) {
            XCloseDisplay(display_);
        }
    }

    void pollEvents() override {
        while (XPending(display_)) {
            XEvent event;
            XNextEvent(display_, &event);
            switch (event.type) {
                case ClientMessage:
                    if (static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_) {
                        should_close_ = true;
                    }
                    break;
                case ConfigureNotify:
                    if (event.xconfigure.width > 0 && event.xconfigure.height > 0) {
                        makeCurrent();
                        glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void swapBuffers() override {
        glXSwapBuffers(display_, window_);
    }

    bool shouldClose() const override {
        return should_close_;
    }

    void makeCurrent() override {
        glXMakeCurrent(display_, window_, context_);
    }

private:
    Display* display_ = nullptr;
    Window window_ = 0;
    GLXContext context_ = nullptr;
    Atom wm_delete_ = None;
    bool should_close_ = false;
};

}  // namespace

std::unique_ptr<PlatformWindow> createPlatformWindow(int width, int height) {
    return std::make_unique<X11Window>(width, height);
}

#endif  // __linux__
