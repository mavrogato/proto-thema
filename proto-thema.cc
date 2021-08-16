
#include <iostream>
#include <fstream>
#include <array>
#include <tuple>
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


int main_loop(auto&& proc) noexcept {
    EXO(AllowSetForegroundWindow(ASFW_ANY));
    EXO(SetProcessDPIAware());

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
                }) {
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
                        std::cout << "The message loop finished." << std::endl;
                        break;
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

int main() {
    if (auto backing = device(bitmap<32>())) {
        auto proc = [&backing](auto... args) noexcept -> LRESULT {
            auto [hwnd, message, wParam, lParam] = std::make_tuple(args...);

            switch (message) {
            case WM_SIZE: {
                backing.rettach(bitmap<32>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                return 0;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);
                BitBlt(ps.hdc, 0, 0, backing.cx(), backing.cy(),
                       backing, 0, 0, SRCCOPY);
                return 0;
            }
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }}
            return DefWindowProc(args...);
        };

        return main_loop(std::move(proc));
    }
    return 0;
}

