#pragma once

class TargetWindow
{
public:
    TargetWindow();
    ~TargetWindow();

    void init(HINSTANCE hInstance, HWND parent, const WCHAR* title);
    void onMouseMove(int x, int y, WPARAM wParam);
    void onPointerUpdate(int x, int y, WPARAM wParam);
    void onTouch();
    void onPaint();
    void onEraseBackground(HDC hdc);

    HWND getHwnd() const;

private:
    const WCHAR* _title;
    HWND _hwnd;
    BOOL _is_active;
    int _last_err;
};

inline HWND TargetWindow::getHwnd() const
{
    return this->_hwnd;
}

