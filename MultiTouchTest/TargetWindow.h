#pragma once

class TargetWindow
{
public:
    TargetWindow();
    ~TargetWindow();

    void init(HINSTANCE hInstance, const WCHAR* title);
    void onMouseMove(int x, int y);
    void onPointerUpdate();
    void onTouch();
    void onPaint();

    HWND getHwnd() const;

    static void destroyAll();

private:
    const WCHAR* _title;
    HWND _hwnd;
};

inline HWND TargetWindow::getHwnd() const
{
    return this->_hwnd;
}

