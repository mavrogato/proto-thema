
#include <tuple>
#include <array>

#include <gtest/gtest.h>

#include "win32-user.hpp"

class win32_user_test : public testing::Test {
protected:
    static inline auto WndProc = [](auto... args) noexcept -> LRESULT {
        auto [ hwnd, message, wParam, lParam ] = std::make_tuple(args...);
        switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(args...);
    };
};

TEST_F(win32_user_test, class_registration) {
#if 0
    {
        auto wc1 = wndclass(WndProc);
        EXPECT_EQ(0u, GetLastError());
        EXPECT_NE(0, static_cast<ATOM>(wc1));

        auto wc2 = wndclass(WndProc);
        EXPECT_EQ(1410u, GetLastError());
        EXPECT_EQ(0, static_cast<ATOM>(wc2));

        EXPECT_NE(wc1, wc2);

        auto wc3 = wndclass(WndProc, "19ddd719-b38a-444d-876c-fb15a8bbd6e1");
        EXPECT_EQ(0u, GetLastError());
        EXPECT_NE(0, static_cast<ATOM>(wc3));

        EXPECT_NE(wc1, wc3);
    }
#endif
}
