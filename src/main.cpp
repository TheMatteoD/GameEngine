#include <glad/glad.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

#include <iostream>
#include <cstring>
#include <unistd.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

int main() {
    // Open The X11 Display
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display\n";
        return -1;
    }

    // Framebuffer Config
    static int visual_attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 8,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (!fbc) {
        std::cerr << "Failed to retrieve framebuffer config \n";
        return -1;
    }

    XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[0]);
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
    swa.event_mask = ExposureMask | KeyPressMask;

    Window win = XCreateWindow(display, RootWindow(display, vi->screen),
                                0, 0, 800, 600, 0, vi->depth, InputOutput,
                                vi->visual, CWColormap | CWEventMask, &swa);
    XStoreName(display, win, "GameEngine Window");
    XMapWindow(display, win);

    // OpenGL context
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 
        (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    GLXContext ctx = glXCreateContextAttribsARB(display, fbc[0], 0, True, context_attribs);
    glXMakeCurrent(display, win, ctx);

    // Load OpenGL using glad
    if (!gladLoadGL()){
        std::cerr << "Failed to initialize OpenGL context via glad\n";
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // Rendering Loop
    bool running = True;
    while (running) {
        while (XPending(display)) {
            XEvent xev;
            XNextEvent(display, &xev);
            if (xev.type == KeyPress) {
                running = false;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glXSwapBuffers(display, win);
        usleep(16000); // ~60Fps
    }

    glXMakeCurrent(display, 0, 0);
    glXDestroyContext(display, ctx);
    XDestroyWindow(display, win);
    XCloseDisplay(display);

    return 0;
}