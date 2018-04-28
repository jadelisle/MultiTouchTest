#include "stdafx.h"
#include "TargetWindow.h"
#include "TargetWindowList.h"

static int nextFreeTargetWindowSlot = 0;
static TargetWindow* targetWindows[NUM_TARGET_WINDOWS] = { nullptr };

TargetWindowList::TargetWindowList()
{
}

TargetWindowList::~TargetWindowList()
{
}

void TargetWindowList::addTargetWindow(TargetWindow* t)
{
    targetWindows[nextFreeTargetWindowSlot++] = t;
}

TargetWindow* TargetWindowList::targetWindowFromHwnd(HWND hwnd)
{
    for (TargetWindow* t : targetWindows) {
        if (t->getHwnd() == hwnd) {
            return t;
        }
    }

    return nullptr;
}

void TargetWindowList::destroyAll()
{
    for (TargetWindow* t : targetWindows) {
        delete t;
    }
}