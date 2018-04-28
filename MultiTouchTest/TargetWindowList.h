#pragma once

class TargetWindow;

#define NUM_TARGET_WINDOWS 2

class TargetWindowList
{
public:
    ~TargetWindowList();

    static void addTargetWindow(TargetWindow* t);
    static TargetWindow* targetWindowFromHwnd(HWND hwnd);
    static void destroyAll();

private:
    TargetWindowList();
};

