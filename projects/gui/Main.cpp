#include "CAppWnd.h"
#include "CDirectX.h"
#include "CGui.h"

#include "Globals.h"

CAppWnd* g_pAppWnd = nullptr;
CDirectX* g_pDirectX = nullptr;
CGui* g_pGui = nullptr;

UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static POINTS g_WindowPosition;

    if (g_pGui->MsgProc(hWnd, uMsg, wParam, lParam))
    {
        return true;
    }

    switch (uMsg)
    {
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
            return 0;

        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);

        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        g_WindowPosition = MAKEPOINTS(lParam);
        return 0L;
    }

    default:
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    g_pAppWnd = new CAppWnd();
    g_pDirectX = new CDirectX();
    g_pGui = new CGui();

    if (!g_pAppWnd->Create(WndProc, hInstance, APP_NAME, APP_WND_CLASS, { 100, 100 }, APP_WND_SIZE))
    {
        MessageBox(NULL, "Could not create window", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    g_pDirectX->Initialize(VSYNC_ENABLED, g_pAppWnd->GetWindow());
    g_pGui->Initialize(g_pAppWnd->GetWindow(), g_pDirectX->GetDevice(), g_pDirectX->GetDeviceContext());
    g_pAppWnd->SetState(SW_SHOWNORMAL);

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        g_pDirectX->SetBuffersSize(g_ResizeWidth, g_ResizeHeight);
        g_pDirectX->BeginScene(0, 0, 0, 1);
        g_pGui->Render(APP_WND_SIZE);
        g_pDirectX->EndScene();
    }

    g_pGui->Shutdown();
    g_pDirectX->Shutdown();
    g_pAppWnd->Shutdown();

    delete g_pGui;
    delete g_pDirectX;
    delete g_pAppWnd;

    return EXIT_SUCCESS;
}
