#include "Quick.h"

using namespace Microsoft::WRL;

static ICoreWebView2* g_WebView{ nullptr };
static quick_view_context* g_Context{ nullptr };
static quick_wndproc_t g_FallbackProc{ nullptr };
static ICoreWebView2Controller* g_WebControl{ nullptr };

ICoreWebView2* quick_get_webview(void) { return g_WebView; }
ICoreWebView2Controller* quick_get_webview_controller(void) { return g_WebControl; }

static auto 
set_window_title(struct quick_view_context* ctx, const WCHAR* title) -> void {
    if (ctx && ctx->p_hwnd) SetWindowTextW(ctx->p_hwnd, title);
}

static auto 
navigate(struct quick_view_context* ctx, const WCHAR* url) -> void {
    auto view = quick_get_webview();
    if (url && view) view->Navigate(url);
    else if (view) view->Navigate(L"about://blank");
}

static auto 
update_bounds(RECT bounds) -> void {
    if (g_WebControl)
        quick_get_webview_controller()->put_Bounds(bounds);
}

struct quick_view_context* 
quick_new_context(HWND hwnd) {
    assert(!g_Context);

#ifdef DEBUG
    assert(hwnd != nullptr);
#else
    if (!hwnd) return nullptr;
#endif

    struct quick_view_context* web = new quick_view_context();
    if (!web) return nullptr;

    web->p_hwnd = hwnd;

    assert(SUCCEEDED(CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
               
                env->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>([&](HRESULT errcode, ICoreWebView2Controller* ctrl) -> HRESULT {
        g_WebControl = ctrl;
        g_WebControl->get_CoreWebView2(&g_WebView);
    
        assert(g_WebView);
        assert(g_WebControl);

        g_WebControl->AddRef();
        g_WebView->AddRef();

        RECT bounds{};
        GetClientRect(hwnd, &bounds);

        g_WebControl->put_Bounds(bounds);

        return S_OK;
        }).Get());

        env->get_BrowserVersionString(&web->s_version);

        return S_OK;
    }).Get())));

    web->set_window_title = set_window_title;
    web->update_bounds = update_bounds;
    web->navigate = navigate;

    return web;
}

void quick_delete_context(struct quick_view_context* web) {
    if (web) {
        web->navigate = nullptr;
        web->go_forward = nullptr;
        web->go_back = nullptr;

        CloseWindow(web->p_hwnd);
        DestroyWindow(web->p_hwnd);

        if (g_WebControl) g_WebControl->Release();
        if (g_WebView) g_WebView->Release();

        delete web;
    }
}

void
quick_set_context(struct quick_view_context* context) {
    assert(context);
    g_Context = context;
}

bool
quick_set_fallback_proc(WNDPROC proc) {
    if (proc && !g_FallbackProc) g_FallbackProc = proc;
    return proc == g_FallbackProc;
}

struct quick_view_context*
quick_get_context(void) {
    return g_Context;
}