#pragma once

#include "MultiTouchTest.h"

class TargetWindow
{
public:
    TargetWindow();
    ~TargetWindow();

    void init(HINSTANCE hInstance, HWND parent);

    void onPointerEnter(int x, int y, WPARAM wParam);
    void onPointerLeave(int x, int y, WPARAM wParam);
    void onPointerUpdate(int x, int y, WPARAM wParam);
    BOOL onTouch(WPARAM wParam, LPARAM lParam);

    void onPaint();
    void onEraseBackground(HDC hdc);

    HWND getHwnd() const;

private:
    int indexFromTouchId(int id);
    void resetTrackingData();

    int _tracking_points[MAX_TOUCH_POINTS][2];
    int _index_id_map[MAX_TOUCH_POINTS];

    HWND _hwnd;
    BOOL _is_active;
    int _last_err;
    HDC _mem_dc;
    HBITMAP _mem_bitmap;
};

inline HWND TargetWindow::getHwnd() const
{
    return this->_hwnd;
}

