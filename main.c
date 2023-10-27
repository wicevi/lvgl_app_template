#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#if SIMULATOR == 1
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mousewheel.h"
#else
#include <pthread.h>
#include <signal.h>
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#endif
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

static void hal_init(void);
static int tick_thread(void *data);

int main(void)
{
    /*Initialize the HAL (lvgl, display, input devices, tick)*/
    hal_init();
    /*Create a Demo*/
    lv_demo_music();
    LV_LOG_USER("lv_demo_music running...");
    
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
    /*Initialize LVGL*/
    lv_init();

#if SIMULATOR == 1
    /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
    monitor_init();
#else
    /*Linux frame buffer device init*/
    fbdev_init();
#endif

    /* Tick init. */
#if SIMULATOR == 1
    SDL_CreateThread(tick_thread, "tick", NULL);
#else
    pthread_t tick_thread_id;
    pthread_attr_t pattr;
    
    pthread_attr_init(&pattr);
    pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tick_thread_id, &pattr, tick_thread, NULL);
    pthread_attr_destroy(&pattr);
#endif

    /*Create a display buffer*/
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t buf1_1[LV_HOR_RES * 100];
    static lv_color_t buf1_2[LV_HOR_RES * 100];
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, LV_HOR_RES * 100);

    /*Create a display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv); 
    disp_drv.draw_buf = &disp_buf1;
#if SIMULATOR == 1
    disp_drv.flush_cb = monitor_flush;
#else
    disp_drv.flush_cb = fbdev_flush;
#endif
    disp_drv.hor_res = LV_HOR_RES;
    disp_drv.ver_res = LV_VER_RES;
    disp_drv.antialiasing = 1;
    lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

    lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, th);

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

#if SIMULATOR == 1
    /* Add the mouse as input device
    * Use the 'mouse' driver which reads the PC's mouse*/
    mouse_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); 
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    indev_drv_1.read_cb = mouse_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);
    lv_indev_set_group(mouse_indev, g);

    keyboard_init();
    static lv_indev_drv_t indev_drv_2;
    lv_indev_drv_init(&indev_drv_2); 
    indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_2.read_cb = keyboard_read;
    lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
    lv_indev_set_group(kb_indev, g);

    mousewheel_init();
    static lv_indev_drv_t indev_drv_3;
    lv_indev_drv_init(&indev_drv_3); 
    indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_3.read_cb = mousewheel_read;
    lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
    lv_indev_set_group(enc_indev, g);
#else
    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); 
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *event_indev = lv_indev_drv_register(&indev_drv_1);
    lv_indev_set_group(event_indev, g);
#endif

}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data) {
    (void)data;

    while(1) {
    #if SIMULATOR == 1
        SDL_Delay(5);
    #else
        usleep(5000);
    #endif
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }

    return 0;
}
