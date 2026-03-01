#ifdef __APPLE__

#include "window.hpp"

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>

@interface LumenWindowDelegate : NSObject <NSWindowDelegate>
@property(nonatomic, assign) bool* shouldClose;
@end

@implementation LumenWindowDelegate
- (void)windowWillClose:(NSNotification*)notification {
    if (self.shouldClose) {
        *self.shouldClose = true;
    }
}
@end

@interface LumenOpenGLView : NSOpenGLView
@end

@implementation LumenOpenGLView
- (void)reshape {
    [super reshape];
    [[self openGLContext] makeCurrentContext];
    NSRect bounds = [self bounds];
    GLint width = (GLint)bounds.size.width;
    GLint height = (GLint)bounds.size.height;
    if (width > 0 && height > 0) {
        glViewport(0, 0, width, height);
    }
}
@end

namespace {

class CocoaWindow : public PlatformWindow {
public:
    CocoaWindow(int width, int height) {
        // Cocoa window + NSOpenGLView with a 3.2 core profile when available.
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        NSRect rect = NSMakeRect(0, 0, width, height);
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                         NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;

        window_ = [[NSWindow alloc] initWithContentRect:rect
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
        [window_ setTitle:@"Lumen"];

        NSOpenGLPixelFormatAttribute attrs[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFADepthSize, 24,
            0
        };

        NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        if (!pixelFormat) {
            // Fallback to a basic pixel format if 3.2 core is unavailable.
            NSOpenGLPixelFormatAttribute legacyAttrs[] = {
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFADepthSize, 24,
                0
            };
            pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:legacyAttrs];
        }

        view_ = [[LumenOpenGLView alloc] initWithFrame:rect pixelFormat:pixelFormat];
        [window_ setContentView:view_];
        [window_ makeFirstResponder:view_];

        context_ = [view_ openGLContext];
        [context_ makeCurrentContext];

        delegate_ = [LumenWindowDelegate new];
        delegate_.shouldClose = &should_close_;
        [window_ setDelegate:delegate_];

        [window_ center];
        [window_ makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];

        glViewport(0, 0, width, height);
    }

    ~CocoaWindow() override {
        [NSOpenGLContext clearCurrentContext];
        [window_ orderOut:nil];
        window_ = nil;
        view_ = nil;
        delegate_ = nil;
    }

    void pollEvents() override {
        while (true) {
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate dateWithTimeIntervalSinceNow:0.001]
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (!event) {
                break;
            }
            [NSApp sendEvent:event];
        }
    }

    void swapBuffers() override {
        [context_ flushBuffer];
    }

    bool shouldClose() const override {
        return should_close_;
    }

    void makeCurrent() override {
        [context_ makeCurrentContext];
    }

private:
    NSWindow* window_ = nil;
    NSOpenGLView* view_ = nil;
    NSOpenGLContext* context_ = nil;
    LumenWindowDelegate* delegate_ = nil;
    bool should_close_ = false;
};

}  // namespace

std::unique_ptr<PlatformWindow> createPlatformWindow(int width, int height) {
    return std::make_unique<CocoaWindow>(width, height);
}

#endif  // __APPLE__
