#ifndef INCLUDE_VERSOR_HPP_862E9B59_D25C_4D1A_824C_377951DC4E69
#define INCLUDE_VERSOR_HPP_862E9B59_D25C_4D1A_824C_377951DC4E69

#include <array>
#include <iosfwd>
#include <cstdint>

#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>


template <typename T, std::uint32_t N>
class versor {
public:
    using size_type = decltype (N);
    using value_type = T;
    using iterator = value_type*;
    using const_iterator = value_type const*;
    using rest_type = versor<T, N-1>;

public:
    // NOTE: C++14 style initializer for C++ASMP
    versor() restrict(amp, cpu) : first(), rest() { }
    template <typename First>
    versor(First first) restrict(amp, cpu)
        : first(static_cast<value_type>(first))
        , rest()
        {
        }
    template <typename First, typename... Rest>
    versor(First first, Rest... rest) restrict(amp, cpu)
        : first(static_cast<value_type>(first))
        , rest(static_cast<value_type>(rest)...)
        {
        }

public:
    constexpr std::uint32_t size() const restrict(amp, cpu) { return N; }

public:
    auto begin() const restrict(amp, cpu) { return reinterpret_cast<const_iterator>(this); }
    auto begin()       restrict(amp, cpu) { return reinterpret_cast<iterator      >(this); }
    auto end  () const restrict(amp, cpu) { return reinterpret_cast<const_iterator>(this) + N; }
    auto end  ()       restrict(amp, cpu) { return reinterpret_cast<iterator      >(this) + N; }

    auto  front() const restrict(amp, cpu) { return *begin(); }
    auto& front()       restrict(amp, cpu) { return *begin(); }
    auto  back () const restrict(amp, cpu) { return *begin() + N-1; }
    auto& back ()       restrict(amp, cpu) { return *begin() + N-1; }
    
public:
    auto  operator [] (size_type i) const restrict(amp, cpu) { return *(begin() + i); }
    auto& operator [] (size_type i)       restrict(amp, cpu) { return *(begin() + i); }

public:
    auto& negate() restrict(amp, cpu) {
        first = -first;
        rest.negate();
        return * this;
    }
    auto& operator += (versor rhs) restrict(amp, cpu) {
        first += rhs.first;
        rest += rhs.rest;
        return *this;
    }
    auto& operator -= (versor rhs) restrict(amp, cpu) {
        first -= rhs.first;
        rest -= rhs.rest;
        return *this;
    }
    auto& operator *= (versor rhs) restrict(amp, cpu) {
        first *= rhs.first;
        rest *= rhs.rest;
        return *this;
    }
    auto& operator *= (value_type s) restrict(amp, cpu) {
        first *= s;
        rest *= s;
        return *this;
    }
    auto& operator /= (versor rhs) restrict(amp, cpu) {
        first /= rhs.first;
        rest /= rhs.rest;
        return *this;
    }

public:
    friend auto operator + (versor v) restrict(amp, cpu) {
        return *this;
    }
    friend auto operator - (versor v) restrict(amp, cpu) {
        return v.negate();
    }
    friend auto operator + (versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs += rhs;
    }
    friend auto operator - (versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs -= rhs;
    }
    friend auto operator * (versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs *= rhs;
    }
    friend auto operator * (versor v, value_type s) restrict(amp, cpu) {
        return v *= s;
    }
    friend auto operator * (value_type s, versor v) restrict(amp, cpu) {
        return v *= s;
    }
    friend auto operator / (versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs /= rhs;
    }

public:
    friend auto inner(versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs.first * rhs.first + inner(lhs.rest, rhs.rest);
    }
    friend auto norm2(versor lhs) restrict(amp, cpu) {
        return inner(lhs, lhs);
    }

public:
    friend bool operator == (versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs.first == rhs.first && lhs.rest == rhs.rest;
    }
    friend bool operator <  (versor lhs, versor rhs) restrict(amp, cpu) {
        return lhs.first < rhs.first && lhs.rest < rhs.rest;
    }

public:
    template <typename Ch>
    friend auto& operator << (std::basic_ostream<Ch>& output, versor v) {
        for (size_type i = 0; i < v.size(); ++i) {
            if (0 == i) {
                output.put('(');
            }
            else {
                output.put(' ');
            }
            output << v[i];
        }
        return output.put(')');
    }

public:
    value_type first;
    rest_type rest;
};

template <typename T>
class versor<T, 0> {
public:
    auto& negate() restrict(amp, cpu) { return *this; }
    auto& operator += (versor) restrict(amp, cpu) { return *this; }
    auto& operator -= (versor) restrict(amp, cpu) { return *this; }
    auto& operator *= (versor) restrict(amp, cpu) { return *this; }
    auto& operator *= (T) restrict(amp, cpu) { return *this; }
    auto& operator /= (versor) restrict(amp, cpu) { return *this; }

public:
    friend auto inner(versor, versor) restrict(amp, cpu) { return 0; }
    friend bool operator == (versor, versor) restrict(amp, cpu) { return true; }
    friend bool operator <  (versor, versor) restrict(amp, cpu) { return true; }

public:
    template <typename Ch>
    friend auto& operator << (std::basic_ostream<Ch>& output, versor) {
        return output;
    }
};

template <typename T>
inline versor<T, 3> cross(versor<T, 3> lhs, versor<T, 3> rhs) {
    return versor<T, 3>(lhs[1] * rhs[2] - lhs[2] * rhs[1],
                        lhs[2] * rhs[0] - lhs[0] * rhs[2],
                        lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

inline auto make_amp_array(auto first, auto... rest) {
    using type = decltype (first);
    constexpr std::size_t N = 1 + sizeof...(rest);
    return concurrency::array<type, 1>(N, std::array<type, N>{ first, rest... }.begin());
}

#endif/*INCLUDE_VERSOR_HPP_862E9B59_D25C_4D1A_824C_377951DC4E69*/
