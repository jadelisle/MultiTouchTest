#include "stdafx.h"
#include "resource.h"
#include "TargetWindow.h"

#include <windowsx.h>
#include <stdio.h>

LRESULT CALLBACK TargetWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define NUM_TARGET_WINDOWS 2
static int nextFreeTargetWindowSlot = 1;
static ATOM windowClass = (ATOM) 0;
static WCHAR szTargetWindowClass[256];

static TargetWindow* targetWindows[NUM_TARGET_WINDOWS] = { nullptr };
static TargetWindow* targetWindowFromHwnd(HWND hwnd);

TargetWindow::TargetWindow() :
    _title(NULL),
    _hwnd((HWND) -1)
{
}

TargetWindow::~TargetWindow()
{
}

void TargetWindow::init(HINSTANCE hInstance, const WCHAR* title)
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

    targetWindows[nextFreeTargetWindowSlot++] = this;
}

void TargetWindow::onMouseMove(int x, int y)
{

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
            t->onMouseMove(x, y);

            return DefWindowProc(hWnd, message, wParam, lParam);
        }
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