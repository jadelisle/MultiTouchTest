#include "stdafx.h"
#include "resource.h"
#include "TargetWindow.h"
#include "TargetWindowList.h"
#include "TargetWindowProc.h"

#include <windowsx.h>
#include <stdio.h>

static ATOM windowClass = (ATOM) 0;
static WCHAR szTargetWindowClass[256];
static int drawRadius = 50;
static COLORREF drawColors[] = {
    RGB(153, 255, 51),
    RGB(153, 0, 0),
    RGB(0, 153, 0),
    RGB(255, 255, 0),
    RGB(255, 51, 204)
};

TargetWindow::TargetWindow() :
    _hwnd(NULL),
    _is_active(FALSE),
    _last_err(-1)
{
}

TargetWindow::~TargetWindow()
{
}

void TargetWindow::init(HINSTANCE hInstance, HWND parent)
{
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

    this->resetTrackingData();

    RegisterTouchWindow(this->_hwnd, 0);
    ShowWindow(this->_hwnd, SW_SHOW);
    UpdateWindow(this->_hwnd);
}

void TargetWindow::onPointerEnter(int x, int y, WPARAM wParam)
{
    printf("0x%08x Activating window\n", (UINT)this->_hwnd);
    printf("0x%08x WM_POINTERENTER ptrid: 0x%04x x: %d y: %d\n",
        (UINT) this->_hwnd, GET_POINTERID_WPARAM(wParam), x, y);
    fflush(stdout);
    this->_is_active = true;
    InvalidateRect(this->_hwnd, NULL, TRUE);
}

void TargetWindow::onPointerLeave(int x, int y, WPARAM wParam)
{
    printf("0x%08x WM_POINTERLEAVE ptrid: 0x%04x x: %d y: %d\n",
        (UINT) this->_hwnd, GET_POINTERID_WPARAM(wParam), x, y);
    printf("0x%08x Deactivating window\n", (UINT)this->_hwnd);
    fflush(stdout);
    this->_is_active = false;
    this->resetTrackingData();
    InvalidateRect(this->_hwnd, NULL, TRUE);
}

void TargetWindow::onPointerUpdate(int x, int y, WPARAM wParam)
{
    printf("0x%08x WM_POINTERUPDATE ptrid: 0x%04x x: %d y: %d\n",
        (UINT) this->_hwnd, GET_POINTERID_WPARAM(wParam), x, y);
    fflush(stdout);
}

BOOL TargetWindow::onTouch(WPARAM wParam, LPARAM lParam)
{
    UINT num_inputs;
    PTOUCHINPUT inputs;
    POINT pt;
    BOOL retval = FALSE;

    num_inputs = LOWORD(wParam);
    inputs = new TOUCHINPUT[num_inputs];

    if (inputs) {
        printf("0x%08x WM_TOUCH numPoints %d\n", (UINT)this->_hwnd, num_inputs);
        fflush(stdout);

        if (GetTouchInputInfo((HTOUCHINPUT)lParam, num_inputs, inputs, sizeof(TOUCHINPUT))) {
            for (int i = 0; i < (int) num_inputs; i++) {
                TOUCHINPUT ti = inputs[i];
                int index = this->indexFromTouchId(ti.dwID);

                if (0 != ti.dwID && index >= 0 && index < MAX_TOUCH_POINTS) {
                    pt.x = TOUCH_COORD_TO_PIXEL(ti.x);
                    pt.y = TOUCH_COORD_TO_PIXEL(ti.y);
                    ScreenToClient(this->_hwnd, &pt);

                    if (ti.dwFlags & TOUCHEVENTF_UP) {
                        this->_tracking_points[index][0] = -1;
                        this->_tracking_points[index][1] = -1;
                    } else {
                        this->_tracking_points[index][0] = pt.x;
                        this->_tracking_points[index][1] = pt.y;
                    }
                }
            }

            CloseTouchInputHandle((HTOUCHINPUT) lParam);
            retval = TRUE;
        }
    }

    delete [] inputs;

    return retval;
}

void TargetWindow::onPaint()
{
    int x;
    int y;
    PAINTSTRUCT ps;
    RECT r;
    HBITMAP old_bitmap;

    HDC hdc = BeginPaint(this->_hwnd, &ps);
    GetClientRect(this->_hwnd, &r);

    if (!this->_mem_dc) {
        this->_mem_dc = CreateCompatibleDC(hdc);
    }

    this->_mem_bitmap = CreateCompatibleBitmap(hdc, r.right, r.bottom);
    old_bitmap = (HBITMAP) SelectObject(this->_mem_dc, this->_mem_bitmap);

    if (this->_is_active) {
        FillRect(this->_mem_dc, &r, CreateSolidBrush(RGB(0, 255, 0)));
    } else {
        FillRect(this->_mem_dc, &r, CreateSolidBrush(RGB(0, 0, 255)));
    }

    for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
        SelectObject(this->_mem_dc, CreateSolidBrush(drawColors[i]));
        x = this->_tracking_points[i][0];
        y = this->_tracking_points[i][1];

        if (x > 0 && y > 0) {
            Ellipse(this->_mem_dc, x - drawRadius, y - drawRadius,
                x + drawRadius, y + drawRadius);
        }
    }

    BitBlt(hdc, 0, 0, r.right, r.bottom, this->_mem_dc, 0, 0, SRCCOPY);
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

int TargetWindow::indexFromTouchId(int id)
{
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
        if (-1 == this->_index_id_map[i]) {
            this->_index_id_map[i] = id;
            return i;
        } else {
            if (id == this->_index_id_map[i]) {
                return i;
            }
        }
    }

    return -1;
}

void TargetWindow::resetTrackingData()
{
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
        this->_tracking_points[i][0] = -1;
        this->_tracking_points[i][1] = -1;
        this->_index_id_map[i] = -1;
    }
}
