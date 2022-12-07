#include <iostream>
#include <Quick.h>

static const WCHAR* g_szClassName = L"BrowserWnd";
static quick_view_context* g_view{};

LRESULT CALLBACK
quick_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    {
        if (wParam == VK_ESCAPE)
        {
            g_view->navigate(g_view, L"https://www.google.com");
        }

        break;
    }
    case WM_CLOSE:
    {
        assert(g_view);
        DestroyWindow(g_view->p_hwnd);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
    {
        auto ctrl = quick_get_webview_controller();

        if (ctrl) {
            RECT bounds;
            GetClientRect(hWnd, &bounds);
            auto& ctrl_unref = ctrl;
            if (ctrl_unref) ctrl_unref->put_Bounds(bounds);
        };

        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

static int new_quick_app(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nShowCmd
) {
    WNDCLASSEX wc{};
    HWND hwnd{};
    MSG Msg{};

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = quick_wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) return -1;

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"Navigateur",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return -2;

    ShowWindow(hwnd, nShowCmd);

    g_view = quick_new_context(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}

int __stdcall WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nShowCmd
) {
#ifndef NDEBUG
    open_log_file();

    atexit([]() -> void {
        if (get_log_fd()) {
            fflush(get_log_fd());
            fclose(get_log_fd());
        }
        });
#endif

    return new_quick_app(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}