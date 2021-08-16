#ifndef INCLUDE_WIN32_GDI_HPP_87BC5B6F_2490_4964_84FC_C451F85618C6
#define INCLUDE_WIN32_GDI_HPP_87BC5B6F_2490_4964_84FC_C451F85618C6

#include <array>
#include <utility>
#include <iosfwd>

#include <cstdint>
#include <cstdlib>

#include <windows.h>
#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>

#include "logging.hpp"

#pragma comment(lib, "gdi32.lib")

template <std::size_t N, typename R = char>
inline auto& bytes_ref(void* ptr) noexcept {
    return *reinterpret_cast<std::array<R, N>*>(ptr);
}
template <std::size_t N, typename R = char>
inline auto const& bytes_ref(void const* ptr) noexcept {
    return *reinterpret_cast<std::array<R, N> const*>(ptr);
}
template <std::size_t N>
inline void shallow_swap(void* lhs, void* rhs) noexcept {
    return std::swap(bytes_ref<N>(lhs), bytes_ref<N>(rhs));
}

template <std::uint16_t BPP, size_t DEPTH = (1ull << BPP)>
class bitmap_base {
public:
    static_assert(BPP == 1 || BPP == 4 || BPP == 8);

    using color_table_type = std::array<RGBQUAD, DEPTH>;

    static constexpr color_table_type grayscale = []() noexcept {
        color_table_type ret { };
        for (std::size_t i = 0; i < ret.size(); ++i) {
            auto brightness = static_cast<std::uint8_t>(255*i / (DEPTH-1));
            ret[i].rgbBlue  = brightness;
            ret[i].rgbGreen = brightness;
            ret[i].rgbRed   = brightness;
        }
        return ret;
    }();

public:
    bitmap_base(color_table_type colors = grayscale) noexcept
        : colors_(colors)
        {
        }

public:
    auto const& colors() const noexcept { return this->colors_; }
    auto&       colors()       noexcept { return this->colors_; }

private:
    color_table_type colors_;
};

template <> class bitmap_base<16> { public: bitmap_base() noexcept { } };
template <> class bitmap_base<24> { public: bitmap_base() noexcept { } };
template <> class bitmap_base<32> { public: bitmap_base() noexcept { } };


template <std::uint16_t BPP>
class bitmap : public BITMAPINFOHEADER, public bitmap_base<BPP> {
private:
    bitmap(BITMAPINFOHEADER&& info, bitmap_base<BPP>&& base = bitmap_base<BPP>()) noexcept
        : BITMAPINFOHEADER(info)
        , bitmap_base<BPP>(std::move(base))
        , pixels_(nullptr)
        , handle_(EXO(CreateDIBSection(nullptr,
                                       reinterpret_cast<BITMAPINFO const*>(this),
                                       DIB_RGB_COLORS,
                                       reinterpret_cast<void**>(&this->pixels_),
                                       nullptr,
                                       0)))
        {
        }

public:
    bitmap(long cx = 1, long cy = 1, bitmap_base<BPP>&& base = bitmap_base<BPP>()) noexcept
        : bitmap(BITMAPINFOHEADER(sizeof (BITMAPINFOHEADER), cx, cy, 1, BPP, BI_RGB),
                 std::move(base))
        {
        }

public:
    ~bitmap() noexcept {
        if (this->handle_) {
            EXO(DeleteObject(this->handle_));
            this->handle_ = nullptr;
        }
    }

public:
    bitmap(bitmap const&) = delete;
    bitmap& operator = (bitmap const&) = delete;

    bitmap(bitmap&& other) noexcept
        : bitmap()
        {
            shallow_swap<sizeof (bitmap)>(this, &other);
        }

    bitmap& operator = (bitmap&& other) noexcept {
        if (this != &other) {
            shallow_swap<sizeof (bitmap)>(this, &other);
        }
        return *this;
    }

public:
    std::uint32_t const* pixels() const noexcept { return this->pixels_; }
    std::uint32_t*       pixels()       noexcept { return this->pixels_; }

    operator HBITMAP() const noexcept { return this->handle_; }

public:
    std::uint32_t cx() const noexcept { return this->biWidth; }
    std::uint32_t cy() const noexcept { return std::abs(this->biHeight); }
    std::uint32_t wx() const noexcept { return (31 + BPP * this->cx()) >> 5; }
    std::uint32_t sx() const noexcept { return this->wx() << 2; }

    std::uint32_t size() const noexcept { return this->sx() * this->cy(); }

public:
    auto view() {
        return concurrency::array_view<std::uint32_t, 2>(cy(), wx(), pixels());
    }

public:
    auto head() const noexcept {
        constexpr std::uint32_t head_size = sizeof (BITMAPFILEHEADER);
        constexpr std::uint32_t info_size = sizeof (*this) - sizeof (pixels_) - sizeof (handle_);
        BITMAPFILEHEADER head = {
            *reinterpret_cast<WORD const*>("BM"),
            head_size + info_size + this->size(),
            0,
            0,
            head_size + info_size,
        };
        return bytes_ref<sizeof (head)>(&head);
    }
    auto const& info() const noexcept {
        constexpr std::uint32_t info_size = sizeof (*this) - sizeof (pixels_) - sizeof (handle_);
        return bytes_ref<info_size>(this);
    }

public:
    template <typename Ch>
    friend auto& operator << (std::basic_ostream<Ch>& output, bitmap const& x) noexcept {
        output &&
            output.write(&x.head().front(), x.head().size()) &&
            output.write(&x.info().front(), x.info().size()) &&
            output.write(reinterpret_cast<char const*>(x.pixels()), x.size());
        return output;
    }

private:
    std::uint32_t*   pixels_;
    HBITMAP          handle_;
};

// inline auto context(HDC hdc, auto deleter = DeleteDC) {
//     class scoped_context {
//     public:
//         scoped_context(HDC hdc) noexcept
//             : hdc_(hdc)
//             , hbitmap_(nullptr)
//             {
//             }
//         ~scoped_context() noexcept {
//             if (this->hdc_) {
//                 DeleteDC(this->hdc_);
//                 this->hdc_ = nullptr;
//             }
//         }

//     public:
//         scoped_context(scoped_context const&) = delete;
//         scoped_context& operator = (scoped_context const&) = delete;

//         scoped_context(scoped_context&& src) noexcept
//             : hdc_(nullptr)
//             {
//                 shallow_swap(this, &src);
//             }
//         scoped_context& operator = (scoped_context&& other) noexcept {
//             if (this != &other) {
//                 shallow_swap(this, &src);
//             }
//             return *this;
//         }

//     public:
//         operator HDC() const noexcept { return this->hdc_; }

//     public:
//         auto select(HBITMAP hbitmap) {
//             auto prev = SelectObject(this->hdc_, this->hbitmap_);
//             if (!this->hbitmap_) {
//                 this->hbitmap_ = prev;
//             }
//             return prev;
//         }

//     private:
//         HDC hdc_;
//         HBITMAP hbitmap_;
//     };
//     return scoped_context(hdc);
// }

template <std::uint16_t BPP>
inline auto device(bitmap<BPP>&& src) noexcept {
    class context : public bitmap<BPP> {
    public:
        context(bitmap<BPP>&& src) noexcept
            : bitmap<BPP>(std::move(src))
            , hdc_(EXO(CreateCompatibleDC(nullptr)))
            , prv_(EXO(SelectObject(this->hdc_, bitmap<BPP>::operator HBITMAP())))
            {
            }
        ~context() noexcept {
            if (this->hdc_) {
                EXO(SelectObject(this->hdc_, this->prv_));
                this->prv_ = nullptr;
                EXO(DeleteDC(this->hdc_));
                this->hdc_ = nullptr;
            }
        }

    public:
        operator HDC() const noexcept { return this->hdc_; }
        operator bool() const noexcept { return this->hdc_ != nullptr; }

    public:
        auto& rettach(bitmap<BPP>&& other) {
            if (this != &other) {
                bitmap<BPP>::operator = (std::move(other));
                EXO(SelectObject(this->hdc_, bitmap<BPP>::operator HBITMAP()));
            }
            return *this;
        }

    private:
        HDC hdc_;
        HGDIOBJ prv_;
    };

    return context(std::move(src));
}

#endif/*INCLUDE_WIN32_GDI_HPP_87BC5B6F_2490_4964_84FC_C451F85618C6*/
