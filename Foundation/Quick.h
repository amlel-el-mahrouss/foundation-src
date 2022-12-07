#pragma once

#include "Foundation.h"

#ifdef __cplusplus
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#endif

typedef WNDPROC quick_wndproc_t;

constexpr int EVENT_LIST_SIZE = 256;

struct quick_view_context {
	HWND  p_hwnd;
	LPWSTR s_version;

	void(*set_window_title)(struct quick_view_context* ctx, const WCHAR* title);
	void(*update_bounds)(RECT rect);
	void(*navigate)(struct quick_view_context* ctx, const WCHAR* url);
	void(*go_back)(struct quick_view_context* ctx, int how_many);
	void(*go_forward)(struct quick_view_context* ctx, int how_many);
};

CHACHA_EXPORT
ICoreWebView2* quick_get_webview(void);

CHACHA_EXPORT
ICoreWebView2Controller* quick_get_webview_controller(void);

CHACHA_EXPORT
struct quick_view_context*
quick_get_context(void);

CHACHA_EXPORT
struct quick_view_context* 
quick_new_context(HWND hwnd);

CHACHA_EXPORT
void 
quick_delete_context(struct quick_view_context* ctx);

CHACHA_EXPORT
bool
quick_set_fallback_proc(WNDPROC proc);

CHACHA_EXPORT
void
quick_set_context(struct quick_view_context* context);

#define INIT_COM assert(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
#define FINI_COM CoUninitialize()
