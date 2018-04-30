#include "stdafx.h"
#include "MultiTouchTest.h"
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

void TargetWindowList::add(TargetWindow* t)
{
    targetWindows[nextFreeTargetWindowSlot++] = t;
}

TargetWindow* TargetWindowList::at(int index)
{
    if (0 <= index && NUM_TARGET_WINDOWS > index) {
        return targetWindows[index];
    }

    return nullptr;
}

TargetWindow* TargetWindowList::targetWindowFromHwnd(HWND hwnd)
{
    for (TargetWindow* t : targetWindows) {
        if (t && t->getHwnd() == hwnd) {
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