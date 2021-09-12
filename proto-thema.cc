
#include <iostream>
#include <fstream>
#include <array>
#include <tuple>
#include <map>
#include <vector>
#include <ranges>
#include <functional>

#include <cstdint>
#include <cstdlib>

#include <windows.h>
#include <windowsx.h>

#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>

#include "generator.hpp"

#include "logging.hpp"
#include "versor.hpp"

#include "win32-gdi.hpp"
// #include "win32-user.hpp"


// int win32_main_loop(auto&& proc) noexcept {
//     EXO(AllowSetForegroundWindow(ASFW_ANY));
//     EXO(SetProcessDPIAware());
//     // EXO(EnableMouseInPointer(TRUE));

//     if (auto wc = wndclass(proc)) {
//         if (auto hwnd = window(wc)) {
//             for (auto flag : {
//                     FEEDBACK_TOUCH_CONTACTVISUALIZATION,
//                     FEEDBACK_PEN_BARRELVISUALIZATION,
//                     FEEDBACK_PEN_TAP,
//                     FEEDBACK_PEN_DOUBLETAP,
//                     FEEDBACK_PEN_PRESSANDHOLD,
//                     FEEDBACK_PEN_RIGHTTAP,
//                     FEEDBACK_TOUCH_TAP,
//                     FEEDBACK_TOUCH_DOUBLETAP,
//                     FEEDBACK_TOUCH_PRESSANDHOLD,
//                     FEEDBACK_TOUCH_RIGHTTAP,
//                     FEEDBACK_GESTURE_PRESSANDTAP,
//                 })
//             {
//                 BOOL enabled = FALSE;
//                 EXO(SetWindowFeedbackSetting(hwnd, EXO(flag), 0, sizeof(enabled), &enabled));
//             }
//             EXO(SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA));
//             EXO(ShowWindow(hwnd, SW_SHOW));
//             EXO(UpdateWindow(hwnd));
//             for (;;) {
//                 MSG msg;
//                 if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
//                     if (WM_QUIT == msg.message) {
//                         exo << "The message loop finished." << std::endl;
//                         return static_cast<int>(msg.wParam);
//                     }
//                     TranslateMessage(&msg);
//                     DispatchMessage(&msg);
//                 }
//                 else {
//                     WaitMessage();
//                 }
//             }
//         }
//     }
//     return -1;
// }

// int main() {
//     if (auto backbuffer = device(bitmap<32>())) {
//         auto touch = []() noexcept -> std::experimental::generator<versor<int, 2>*> {
//             for (;;) {
//                 versor<int, 2> pt;
//                 co_yield &pt;
//                 exo << pt << std::endl;
//             }
//         };
//         std::map<unsigned short, std::experimental::generator<versor<int, 2>*>> touches;

//         // std::map<std::uint16_t, std::vector<versor<int, 2>>> touches;
//         // auto pointer_proc = [&gen](auto... args) noexcept {
//         //     auto [hwnd, message, wParam, lParam] = std::make_tuple(args...);
//         //     POINTER_INFO info;
//         //     GetPointerInfo(GET_POINTERID_WPARAM(wParam), &info);
//         //     ScreenToClient(hwnd, &info.ptPixelLocation);
//         //     versor<int, 2>& pt = reinterpret_cast<versor<int, 2>&>(info.ptPixelLocation);
//         //     //!!!
//         //     *(*std::begin(gen)) = pt;
//         //     //!!!
//         //     return 0;
//         // };
//         auto proc = [&backbuffer, &touch, &touches](auto... args) noexcept -> LRESULT {
//             auto [hwnd, message, wParam, lParam] = std::make_tuple(args...);
//             switch (message) {
//             case WM_POINTERDOWN: {
//                 POINTER_INFO info;
//                 GetPointerInfo(GET_POINTERID_WPARAM(wParam), &info);
//                 ScreenToClient(hwnd, &info.ptPixelLocation);
//                 touches.insert({GET_POINTERID_WPARAM(wParam), touch()});
//                 **std::begin(touches[GET_POINTERID_WPARAM(wParam)]) =
//                     reinterpret_cast<versor<int, 2>&>(info.ptPixelLocation);
//                 return 0;
//             }
//             case WM_POINTERUPDATE:
//             case WM_POINTERUP: {
//                 //pointer_proc(args...);
//                 return 0;
//             }
//             case WM_SIZE: {
//                 backbuffer.rettach(bitmap<32>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
//                 Refresh(hwnd);
//                 return 0;
//             }
//             case WM_PAINT: {
//                 PAINTSTRUCT ps;
//                 BeginPaint(hwnd, &ps);
//                 BitBlt(ps.hdc, 0, 0, backbuffer.cx(), backbuffer.cy(),
//                        backbuffer, 0, 0, SRCCOPY);
//                 EndPaint(hwnd, &ps);
//                 return 0;
//             }
//             case WM_DESTROY: {
//                 PostQuitMessage(0);
//                 return 0;
//             }}
//             return DefWindowProc(args...);
//         };
//         return win32_main_loop(std::move(proc));
//     }
//     return -1;
// }

int main() {
    struct wndclass : WNDCLASS {
        wndclass(wndclass const&) = delete;
        wndclass& operator=(wndclass const&) = delete;
        wndclass(WNDCLASS&& wc) noexcept
            : WNDCLASS(wc)
            , atom_(EXO(RegisterClass(this)))
            {
            }
        ~wndclass() noexcept {
            if (this->atom_) {
                UnregisterClass(this->lpszClassName, this->hInstance);
            }
        }
        operator ATOM() const noexcept { return this->atom_; }

    private:
        ATOM atom_;
    };

    std::tuple<HWND, UINT, WPARAM, LPARAM> msg;
    auto const& [ hwnd, message, wParam, lParam ] = msg;

    std::vector<generator<bool>> sequences;

    sequences.push_back([&]() noexcept -> generator<bool> {
            while (message != WM_DESTROY) {
                co_yield false;
            }
            std::cout << "Now closing the window..." << std::endl;
            PostQuitMessage(0);
            co_yield true;
            co_return ;
        }());
    sequences.push_back([&]() noexcept -> generator<bool> {
            while (message != WM_LBUTTONDOWN) {
                co_yield false;
            }
            std::cout << versor<int, 2>(GET_X_LPARAM(lParam),
                                        GET_Y_LPARAM(lParam)) << std::endl;
            co_yield true;
        }());

    static auto bridge = [&](auto... args) noexcept -> LRESULT {
        msg = std::make_tuple(args...);
        bool handled = false;
        for (auto iter = sequences.begin(); iter != sequences.end(); ) {
            auto& sequence = *iter;
            if (!sequence.empty()) {
                if (*sequence.begin()) {
                    handled = true;
                }
            }
            if (sequence.empty()) {
                iter = sequences.erase(iter);
                std::cout << "removed: " << sequences.size() << std::endl;
            }
            else {
                ++iter;
            }
        }
        return handled ? 0 : DefWindowProc(args...);
    };

    if (wndclass wc = WNDCLASS {
            CS_HREDRAW | CS_VREDRAW,
            [](auto... args) noexcept -> LRESULT { return bridge(args...); },
            0,
            0,
            EXO(GetModuleHandle(nullptr)),
            EXO(LoadIcon(nullptr, IDI_APPLICATION)),
            EXO(LoadCursor(nullptr, IDC_CROSS)),
            EXO(static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH))),
            nullptr,
            TEXT("9cec910a-1e09-4b36-8ced-28854f6bcbcb"),
        })
    {
        if (HWND hwnd = CreateWindowEx(WS_EX_LAYERED,
                                       wc.lpszClassName,
                                       TEXT("default caption"),
                                       WS_OVERLAPPEDWINDOW,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       nullptr, nullptr, wc.hInstance, nullptr))
        {
            EXO(hwnd);
            EXO(SetLayeredWindowAttributes(hwnd, 0x000000, 0xff, LWA_ALPHA));
            EXO(ShowWindow(hwnd, SW_SHOW));
            EXO(UpdateWindow(hwnd));

            for (;;) {
                MSG msg;
                if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (WM_QUIT == msg.message) {
                        std::cout << "The message loop finished." << std::endl;
                        Sleep(2000);
                        return static_cast<int>(msg.wParam);
                    }
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else {
                    WaitMessage();
                }
            }
        }
    }
    return 0;
}

    
