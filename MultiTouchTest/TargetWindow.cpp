#include "stdafx.h"
#include "resource.h"
#include "TargetWindow.h"
#include "TargetWindowList.h"

#include <windowsx.h>
#include <stdio.h>

static ATOM windowClass = (ATOM) 0;
static WCHAR szTargetWindowClass[256];

LRESULT CALLBACK TargetWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

TargetWindow::TargetWindow() :
    _title(NULL),
    _hwnd(NULL),
    _is_active(FALSE),
    _last_err(-1)
{
}

TargetWindow::~TargetWindow()
{
}

void TargetWindow::init(HINSTANCE hInstance, HWND parent, const WCHAR* title)
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
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = szTargetWindowClass;
        wcex.hIconSm = NULL;

        windowClass = RegisterClassEx(&wcex);
    }

    this->_hwnd = CreateWindowW(szTargetWindowClass, nullptr,
        WS_CHILD | WS_BORDER, 0, 0, 100, 100,
        parent, nullptr, hInstance, nullptr);

    if (this->_hwnd == NULL) {
        this->_last_err = GetLastError();
    }

    RegisterTouchWindow(this->_hwnd, 0);
    ShowWindow(this->_hwnd, SW_SHOW);
    UpdateWindow(this->_hwnd);
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

LRESULT CALLBACK TargetWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TargetWindow *t = TargetWindowList::targetWindowFromHwnd(hWnd);

    switch (message) {
        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (t) t->onMouseMove(x, y, wParam);
        }
        break;

        case WM_POINTERUPDATE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (t) t->onPointerUpdate(x, y, wParam);
        }
        break;

        case WM_ERASEBKGND:
        {
            if (t) t->onEraseBackground((HDC) wParam);
            return 1L;
        }

        case WM_PAINT:
        {
            if (t) t->onPaint();
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
