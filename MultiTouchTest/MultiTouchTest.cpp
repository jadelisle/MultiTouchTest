// MultiTouchTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MultiTouchTest.h"

#include <windowsx.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

WCHAR szButtonClass[MAX_LOADSTRING];
WCHAR szLeftMouseArea[MAX_LOADSTRING];
WCHAR szRightMouseArea[MAX_LOADSTRING];

char outputDebugBuffer[MAX_LOADSTRING * 2];

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hMainWnd;                                  // Handle to the main window instance

RECT rectLeft;
RECT rectRight;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void UpdateHitRects(RECT* rect);
BOOL TestForTouchInput();
void OutputDebugEventMessage(UINT msg, WPARAM wParam, LPARAM lParam);
void OutputDebugTouchCapabilties();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MULTITOUCHTEST, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_LEFTMOUSEAREA, szLeftMouseArea, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_RIGHTMOUSEAREA, szRightMouseArea, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MULTITOUCHTEST));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MULTITOUCHTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MULTITOUCHTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    hMainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, hInstance, nullptr);

    if (!hMainWnd)
    {
        return FALSE;
    }

    if (TestForTouchInput())
    {
        RegisterTouchWindow(hMainWnd, 0);
    }

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;

    switch (message)
    {
        case WM_CREATE:
        {
            LPCREATESTRUCT data = (LPCREATESTRUCT)lParam;
            SetRect(&rect, 0, 0, data->cx, data->cy);
            UpdateHitRects(&rect);
        }
        break;

        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            sprintf_s(outputDebugBuffer, "WM_MOUSEMOVE\tx: %d\ty: %d\n", x, y);
            OutputDebugStringA(outputDebugBuffer);

            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        case WM_POINTERUPDATE:
        {
            OutputDebugStringA("WM_POINTERUPDATE");
        }
        break;

        case WM_TOUCH:
        {
            OutputDebugStringA("WM_TOUCH");
        }
        break;

        case WM_SIZE:
        {
            SetRect(&rect, 0, 0, LOWORD(lParam), HIWORD(lParam));
            UpdateHitRects(&rect);
        }
        break;

        case WM_ERASEBKGND:
        {
            hdc = (HDC)wParam;
            GetClientRect(hWnd, &rect);
            SelectObject(hdc, GetStockObject(DC_BRUSH));
            SetDCBrushColor(hdc, RGB(255, 255, 255));
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            SetDCBrushColor(hdc, RGB(0, 0, 128));
            Rectangle(hdc, rectLeft.left, rectLeft.top, rectLeft.right, rect.bottom);
            SetDCBrushColor(hdc, RGB(0, 128, 0));
            Rectangle(hdc, rectRight.left, rect.top, rect.right, rect.bottom);
            return 1L;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void UpdateHitRects(RECT* rect)
{
    CopyRect(&rectLeft, rect);
    CopyRect(&rectRight, rect);

    rectLeft.right = rectLeft.left + ((rectLeft.right - rectLeft.left) / 3);
    rectRight.left = rectRight.right - ((rectRight.right - rectRight.left) / 3);
}

BOOL TestForTouchInput()
{
    int value = GetSystemMetrics(SM_DIGITIZER);
    int supportsMulti = value & NID_MULTI_INPUT;

    if (0 == value)
    {
        MessageBoxW(hMainWnd, L"No touch device found", L"Touch Device Status", MB_OK);
    }

    if (NID_INTEGRATED_TOUCH & value)
    {
        MessageBoxW(hMainWnd, L"Integrated touch device found", L"Touch Device Status", MB_OK);
    }

    return FALSE;
}

void OutputDebugEventMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{

}

void OutputDebugTouchCapabilties()
{

}