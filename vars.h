#pragma once
#include "headers.h"


// 1. Basic math structures
struct Vector2 { float x, y; };
struct Vector3 { float x, y, z; }; // You already have this, but good to keep here

// 2. The Math: Converts 3D coordinates to 2D Screen pixels
// windowWidth/Height should be the size of the Assault Cube window
bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight) {
    float clipX = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
    float clipY = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
    float clipW = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

    if (clipW < 0.1f) return false; // Object is behind you

    Vector2 ndc;
    ndc.x = clipX / clipW;
    ndc.y = clipY / clipW;

    screen.x = (windowWidth / 2 * ndc.x) + (ndc.x + windowWidth / 2);
    screen.y = -(windowHeight / 2 * ndc.y) + (ndc.y + windowHeight / 2);
    return true;
}

// 3. The Window: Creates the invisible drawing layer
HWND CreateOverlay(int width, int height) {
    // This creates a window that is Topmost, Layered (transparent), and Transparent (click-through)
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        L"Static", L"ESP Overlay", WS_POPUP,
        0, 0, width, height, NULL, NULL, NULL, NULL
    );

    // Set Black (0,0,0) as the "Invisible" color
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOW);
    return hwnd;
}

// 4. The Pen: Simple function to draw the box
void DrawESPBox(HDC hdc, int x, int y, int width, int height) {
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); // Red, 2px thick
    HGDIOBJ oldObj = SelectObject(hdc, hPen);

    // Draw the 4 sides of the box
    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x + width, y);
    LineTo(hdc, x + width, y + height);
    LineTo(hdc, x, y + height);
    LineTo(hdc, x, y);

    SelectObject(hdc, oldObj);
    DeleteObject(hPen);
}