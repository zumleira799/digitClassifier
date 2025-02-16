#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include "./imageProcessor.h"

#define WIDTH 640
#define HEIGHT 640

#define drawRadius 32

void drawCircle(int* pixArr, int radius, int xCenter, int yCenter, int pixArrSizeX, int pixArraySizeY){
    int cyD = radius;
    for(int i = yCenter+radius; i >= yCenter-radius; i--){
        int d = sqrt((radius*radius) - (cyD*cyD));
        cyD--;
        for(int k = xCenter-d; k <= xCenter+d; k++){
            if(i*pixArrSizeX + k >= pixArraySizeY*pixArrSizeX || k >= pixArrSizeX || k <= 0){
                continue;
            }
            pixArr[i*pixArrSizeX + k] = 0xffffff;
        }
    }
}



int main(int argc, char** agrv){
    Display *display;
    Window window;
    XEvent event;
    int screen;

    display = XOpenDisplay(NULL);

    if(display == NULL){
        fprintf(stderr, "Cannot open X display\n");
        exit(-1);
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WIDTH, HEIGHT, 1, BlackPixel(display, screen), BlackPixel(display, screen));

    XSelectInput(display, window, ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask);

    XMapWindow(display, window);
    
    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, WhitePixel(display, screen));
    XSetBackground(display, gc, BlackPixel(display, screen));

    unsigned int pixels[WIDTH * HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            pixels[y * WIDTH + x] = 0x000000;
        }
    }

    XImage *image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, (char*)pixels, WIDTH, HEIGHT, 32, 0);
    char isDown = 0;
    while(1){
        XNextEvent(display, &event);
        if(event.type == Expose){
            XPutImage(display, window, gc, image, 0, 0, 0, 0, WIDTH, HEIGHT);
        }
        if(event.type == ButtonPress && event.xbutton.button == Button1){
            isDown = 1;
            printf("Mouse down at (%d, %d)\n", event.xbutton.x, event.xbutton.y);
            drawCircle(pixels, drawRadius, event.xbutton.x, event.xbutton.y, WIDTH, HEIGHT);
            XPutImage(display, window, gc, image, 0, 0, 0, 0, WIDTH, HEIGHT);
        }
        if(event.type == ButtonRelease && event.xbutton.button == Button1){
            isDown = 0;
        }
        if(event.type == MotionNotify && isDown){
            //printf("Mouse dragging at (%d, %d)\n", event.xmotion.x, event.xmotion.y);
            drawCircle(pixels, drawRadius, event.xbutton.x, event.xbutton.y, WIDTH, HEIGHT);
            XPutImage(display, window, gc, image, 0, 0, 0, 0, WIDTH, HEIGHT);
        }
        if(event.type == KeyPress){
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            if (keysym == XK_Return) {
                int number = predictNumber(pixels, WIDTH, HEIGHT);
                printf("This number is %d\n", number);
                /*
                for (int y = 0; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        pixels[y * WIDTH + x] = 0x000000;
                    }
                }
                */
                XPutImage(display, window, gc, image, 0, 0, 0, 0, WIDTH, HEIGHT);
            }
        }
    }
    
    XCloseDisplay(display);
    return 0;

}