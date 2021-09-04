#ifndef INCLUDE_WIN32_USER_DAF470F2_7C30_46D6_9A12_35401A05A4B3
#define INCLUDE_WIN32_USER_DAF470F2_7C30_46D6_9A12_35401A05A4B3

#include <windows.h>
#include <windowsx.h>

#include "logging.hpp"

#pragma comment(lib, "user32.lib")

inline auto wndclass(auto... args) noexcept {
    class wndclass_t : public WNDCLASS {
    public:
        wndclass_t(WNDCLASS&& wc) noexcept
            : WNDCLASS(wc)
            , atom_(EXO(RegisterClass(this)))
            {
            }
        ~wndclass_t() noexcept {
            if (this->atom_) {
                EXO(UnregisterClass(this->lpszClassName, this->hInstance));
                this->atom_ = 0;
            }
        }

        wndclass_t(wndclass_t const&) = delete;
        wndclass_t& operator = (wndclass_t const&) = delete;

    public:
        operator ATOM() const noexcept {
            return this->atom_;
        }

    private:
        ATOM atom_;
    };
    return wndclass_t(WNDCLASS(args...));
}

inline auto wndclass(auto proc, char const* classname = __DATE__ " " __TIME__) noexcept {
    static auto static_bridge_proc = [proc](auto... args) noexcept {
        return proc(args...);
    };
    return wndclass(CS_HREDRAW | CS_VREDRAW,
                    [](auto... args) noexcept { return static_bridge_proc(args...); },
                    0,
                    0,
                    EXO(GetModuleHandle(nullptr)),
                    EXO(LoadIcon(nullptr, IDI_APPLICATION)),
                    EXO(LoadCursor(nullptr, IDC_CROSS)),
                    static_cast<HBRUSH>(EXO(GetStockObject(DKGRAY_BRUSH))),
                    nullptr,
                    classname);
}

inline HWND window(auto const& wc) noexcept {
    if (wc) {
        if (auto hwnd = EXO(CreateWindowEx(WS_EX_LAYERED,
                                           wc.lpszClassName,
                                           TEXT("default caption"),
                                           WS_OVERLAPPEDWINDOW,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           nullptr, nullptr, wc.hInstance, nullptr))) {
            return hwnd;
        }
    }
    return nullptr;
}

inline auto Refresh(HWND hwnd, BOOL repaint = FALSE) {
    return InvalidateRect(hwnd, nullptr, repaint) && UpdateWindow(hwnd);
}


#endif/*INCLUDE_WIN32_USER_DAF470F2_7C30_46D6_9A12_35401A05A4B3*/
