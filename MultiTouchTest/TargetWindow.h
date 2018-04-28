#pragma once

class TargetWindow
{
public:
    TargetWindow();
    ~TargetWindow();

    void init(HINSTANCE hInstance, HWND parent, const WCHAR* title,
        int x, int y, int width, int height);
    void onMouseMove(int x, int y, WPARAM wParam);
    void onPointerUpdate(int x, int y, WPARAM wParam);
    void onTouch();
    void onPaint();
    void onEraseBackground(HDC hdc);

    HWND getHwnd() const;

    static void destroyAll();

private:
    const WCHAR* _title;
    HWND _hwnd;
    BOOL _is_active;
};

inline HWND TargetWindow::getHwnd() const
{
    return this->_hwnd;
}

