#ifdef _WIN32

#include "window.hpp"

#include <windows.h>
#include <GL/gl.h>

namespace {

class Win32Window : public PlatformWindow {
public:
    Win32Window(int width, int height) {
        // Win32 window + legacy WGL context (acceptable fallback for Phase 1).
        WNDCLASS wc = {};
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = &Win32Window::WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"LumenWindowClass";

        RegisterClass(&wc);

        RECT rect{0, 0, width, height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        hwnd_ = CreateWindowEx(
            0,
            wc.lpszClassName,
            L"Lumen",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            wc.hInstance,
            this);

        hdc_ = GetDC(hwnd_);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;

        int pf = ChoosePixelFormat(hdc_, &pfd);
        SetPixelFormat(hdc_, pf, &pfd);

        // Legacy context for Phase 1 (fallback allowed by spec).
        glrc_ = wglCreateContext(hdc_);
        wglMakeCurrent(hdc_, glrc_);

        glViewport(0, 0, width, height);
    }

    ~Win32Window() override {
        if (glrc_) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(glrc_);
        }
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    void pollEvents() override {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void swapBuffers() override {
        SwapBuffers(hdc_);
    }

    bool shouldClose() const override {
        return should_close_;
    }

    void makeCurrent() override {
        wglMakeCurrent(hdc_, glrc_);
    }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        Win32Window* self = nullptr;
        if (msg == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCT*>(lparam);
            self = reinterpret_cast<Win32Window*>(create->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        } else {
            self = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (self) {
            switch (msg) {
                case WM_CLOSE:
                    self->should_close_ = true;
                    DestroyWindow(hwnd);
                    return 0;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_SIZE: {
                    int width = LOWORD(lparam);
                    int height = HIWORD(lparam);
                    if (width > 0 && height > 0) {
                        self->makeCurrent();
                        glViewport(0, 0, width, height);
                    }
                    return 0;
                }
            }
        }

        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    HWND hwnd_ = nullptr;
    HDC hdc_ = nullptr;
    HGLRC glrc_ = nullptr;
    bool should_close_ = false;
};

}  // namespace

std::unique_ptr<PlatformWindow> createPlatformWindow(int width, int height) {
    return std::make_unique<Win32Window>(width, height);
}

#endif  // _WIN32
