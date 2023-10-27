# LVGL_APP_TEMPLATE
lvgl应用模板,PC模拟使用SDL,通过修改*Makefile*文件内`S_SIMULATOR`变量，来切换模拟还是交叉编译.
若选择交叉编译，请修改编译器路径和相关安装指令(在*Makefile*文件内).
**注意**：交叉编译使用`FBDEV`和`EVDEV`，请根据实际情况修改*lv_drv_conf.h*.