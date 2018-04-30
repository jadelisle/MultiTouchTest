
#include "stdafx.h"
#include "resource.h"
#include "TargetWindowProc.h"
#include "TargetWindow.h"
#include "TargetWindowList.h"

#include <windowsx.h>

LRESULT CALLBACK TargetWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TargetWindow *t = TargetWindowList::targetWindowFromHwnd(hWnd);

    switch (message) {
        case WM_POINTERENTER:
        {
            if (t) t->onPointerEnter(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
        }
        break;

        case WM_POINTERLEAVE:
        {
            if (t) t->onPointerLeave(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
        }
        break;

        case WM_POINTERUPDATE:
        {
            if (t) t->onPointerUpdate(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
        }
        break;

        case WM_TOUCH:
        {
            if (t) {
                if (t->onTouch(wParam, lParam)) {
                    CloseTouchInputHandle((HTOUCHINPUT) lParam);
                } else {
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
        }
        break;

        case WM_ERASEBKGND:
        {
            if (t) t->onEraseBackground((HDC)wParam);
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
