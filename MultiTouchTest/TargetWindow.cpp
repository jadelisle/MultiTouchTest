#include "stdafx.h"
#include "resource.h"
#include "TargetWindow.h"

#include <windowsx.h>
#include <stdio.h>

#define NUM_TARGET_WINDOWS 2
static int nextFreeTargetWindowSlot = 1;
static ATOM windowClass = (ATOM) 0;
static WCHAR szTargetWindowClass[256];
static TargetWindow* targetWindows[NUM_TARGET_WINDOWS] = { nullptr };

LRESULT CALLBACK TargetWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static TargetWindow* targetWindowFromHwnd(HWND hwnd);

TargetWindow::TargetWindow() :
    _title(NULL),
    _hwnd((HWND) -1),
    _is_active(FALSE)
{
}

TargetWindow::~TargetWindow()
{
}

void TargetWindow::init(HINSTANCE hInstance, HWND parent, const WCHAR* title,
    int x, int y, int width, int height)
{
    this->_title = title;

    if (!windowClass) {
        LoadString(hInstance, IDS_TARGETWINDOW, szTargetWindowClass, 256);

        WNDCLASSEXW wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = TargetWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszClassName = szTargetWindowClass;

        windowClass = RegisterClassEx(&wcex);
    }

    this->_hwnd = CreateWindowW(szTargetWindowClass, this->_title,
        WS_CHILD | WS_BORDER, x, y, width, height,
        parent, nullptr, hInstance, nullptr);

    RegisterTouchWindow(this->_hwnd, 0);
    ShowWindow(this->_hwnd, SW_SHOW);
    UpdateWindow(this->_hwnd);

    targetWindows[nextFreeTargetWindowSlot++] = this;
}

void TargetWindow::onMouseMove(int x, int y, WPARAM wParam)
{
}

void TargetWindow::onPointerUpdate(int x, int y, WPARAM wParam)
{
}

void TargetWindow::onPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(this->_hwnd, &ps);
    // paint stuff here
    EndPaint(this->_hwnd, &ps);
}

void TargetWindow::onEraseBackground(HDC hdc)
{
    RECT rect;
    GetClientRect(this->_hwnd, &rect);
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    
    if (this->_is_active) {
        SetDCBrushColor(hdc, RGB(0, 255, 0));
    }
    else {
        SetDCBrushColor(hdc, RGB(0, 0, 255));
    }

    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
}

void TargetWindow::destroyAll()
{
    for (TargetWindow* t : targetWindows) {
        delete t;
    }
}

LRESULT CALLBACK TargetWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TargetWindow *t = targetWindowFromHwnd(hWnd);

    if (t == nullptr) {
        return 0;
    }

    switch (message) {
        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            t->onMouseMove(x, y, wParam);
        }
        break;

        case WM_POINTERUPDATE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            t->onPointerUpdate(x, y, wParam);
        }
        break;

        case WM_ERASEBKGND:
        {
            t->onEraseBackground((HDC) wParam);
            return 1L;
        }

        case WM_PAINT:
        {
            t->onPaint();
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
    }

    return 0;
}

static TargetWindow* targetWindowFromHwnd(HWND hwnd)
{
    for (TargetWindow* t : targetWindows) {
        if (t->getHwnd() == hwnd) {
            return t;
        }
    }
    
    return nullptr;
}