
#include <iostream>
#include <fstream>
#include <array>
#include <tuple>
#include <map>
#include <vector>
#include <functional>
#include <cstdint>
#include <cstdlib>

#include <experimental/generator>

#include <windows.h>

#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>

#include "logging.hpp"
#include "versor.hpp"

#include "win32-gdi.hpp"
#include "win32-user.hpp"


int win32_main_loop(auto&& proc) noexcept {
    EXO(AllowSetForegroundWindow(ASFW_ANY));
    EXO(SetProcessDPIAware());
    // EXO(EnableMouseInPointer(TRUE));

    if (auto wc = wndclass(proc)) {
        if (auto hwnd = window(wc)) {
            for (auto flag : {
                    FEEDBACK_TOUCH_CONTACTVISUALIZATION,
                    FEEDBACK_PEN_BARRELVISUALIZATION,
                    FEEDBACK_PEN_TAP,
                    FEEDBACK_PEN_DOUBLETAP,
                    FEEDBACK_PEN_PRESSANDHOLD,
                    FEEDBACK_PEN_RIGHTTAP,
                    FEEDBACK_TOUCH_TAP,
                    FEEDBACK_TOUCH_DOUBLETAP,
                    FEEDBACK_TOUCH_PRESSANDHOLD,
                    FEEDBACK_TOUCH_RIGHTTAP,
                    FEEDBACK_GESTURE_PRESSANDTAP,
                })
            {
                BOOL enabled = FALSE;
                EXO(SetWindowFeedbackSetting(hwnd, EXO(flag), 0, sizeof(enabled), &enabled));
            }
            EXO(SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA));
            EXO(ShowWindow(hwnd, SW_SHOW));
            EXO(UpdateWindow(hwnd));
            for (;;) {
                MSG msg;
                if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (WM_QUIT == msg.message) {
                        exo << "The message loop finished." << std::endl;
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
    return -1;
}

int main() {
    //!!!
    auto gen = []() -> std::experimental::generator<versor<int, 2>*> {
        for (;;) {
            versor<int, 2> pt;
            co_yield &pt;
            exo << pt << std::endl;
        }
    }();
    //!!!
    if (auto backbuffer = device(bitmap<32>())) {
        std::map<std::uint16_t, std::vector<versor<int, 2>>> touches;
        auto pointer_proc = [&gen](auto... args) noexcept {
            auto [hwnd, message, wParam, lParam] = std::make_tuple(args...);
            POINTER_INFO info;
            GetPointerInfo(GET_POINTERID_WPARAM(wParam), &info);
            ScreenToClient(hwnd, &info.ptPixelLocation);
            versor<int, 2>& pt = reinterpret_cast<versor<int, 2>&>(info.ptPixelLocation);
            //!!!
            *(*std::begin(gen)) = pt;
            //!!!
            return 0;
        };
        auto mouse_proc = [](auto pt) noexcept {
            exo << pt << std::endl;
            return 0;
        };
        auto proc = [&backbuffer, pointer_proc, mouse_proc](auto... args) noexcept -> LRESULT {
            auto [hwnd, message, wParam, lParam] = std::make_tuple(args...);
            switch (message) {
            case WM_POINTERDOWN:
            case WM_POINTERUPDATE:
            case WM_POINTERUP: {
                pointer_proc(args...);
                return 0;
            }
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MOUSEMOVE: {
                if (WM_LBUTTONDOWN == message) {
                    SetCapture(hwnd);
                }
                if (WM_LBUTTONUP == message) {
                    ReleaseCapture();
                }
                if (GetCapture() == hwnd) {
                    mouse_proc(versor<int, 2>(GET_X_LPARAM(lParam),
                                              GET_Y_LPARAM(lParam)));
                }
                return 0;
            }
            case WM_SIZE: {
                backbuffer.rettach(bitmap<32>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                Refresh(hwnd);
                return 0;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);
                BitBlt(ps.hdc, 0, 0, backbuffer.cx(), backbuffer.cy(),
                       backbuffer, 0, 0, SRCCOPY);
                EndPaint(hwnd, &ps);
                return 0;
            }
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }}
            return DefWindowProc(args...);
        };
        return win32_main_loop(std::move(proc));
    }
    return -1;
}
