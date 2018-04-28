#pragma once

class TargetWindow;

#define NUM_TARGET_WINDOWS 2

class TargetWindowList
{
public:
    ~TargetWindowList();

    static void add(TargetWindow* t);
    static TargetWindow* at(int index);
    static TargetWindow* targetWindowFromHwnd(HWND hwnd);
    static void destroyAll();

private:
    TargetWindowList();
};

