#pragma once

class TargetWindow;

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

