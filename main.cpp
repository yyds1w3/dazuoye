/**
 * 文件名: main.cpp
 * 描述: 程序的入口文件
 */

#include <QApplication>
#include "main_window.h"

int main(int argc, char *argv[]) {
        // 创建 Qt 应用程序核心对象
    // 它负责管理应用程序的控制流和主要设置
    QApplication a(argc, argv);
    
    // 创建并显示主窗口
    // 这里并没有使用 new (堆内存)，而是直接在栈上创建，main 函数结束时自动销毁
    MainWindow w;
    w.show(); // 必须调用 show() 窗口才会显示出来
    
    // 进入 Qt 的事件循环 (Event Loop)
    // 程序会在这里“暂停”，等待用户的点击、键盘输入等事件，直到调用 quit()
    return a.exec(); 
}