#ifndef INCLUDE_LOGGING_HPP_6F155CFC_E243_4B1B_8F71_AA993B4C47A3
#define INCLUDE_LOGGING_HPP_6F155CFC_E243_4B1B_8F71_AA993B4C47A3

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>


namespace logging {

inline auto logger() {
    thread_local auto instance = &std::clog;
    return instance;
}
  
template <typename Ch>
inline auto& operator << (std::basic_ostream<Ch>& output,
                          std::chrono::time_point<std::chrono::system_clock> const& value) {
    using namespace std::chrono;
    auto t = system_clock::to_time_t(value);
    output << std::put_time(std::localtime(&t), "%FT%T");

    auto u = duration_cast<microseconds>(value.time_since_epoch()).count();
    auto prev_fill = output.fill('0');
    {
        output << '.' << std::setw(6) << u % 1000000; // TODO: use "period"
    }
    output.fill(prev_fill);

    return output;
}

template <typename Ch>
inline auto& put_thread_id(std::basic_ostream<Ch>& output) {
    auto prev_fill = output.fill('0');
    auto prev_flags = output.setf(std::ios_base::hex, std::ios_base::basefield);
    {
        output << std::setw(4) << std::this_thread::get_id();
    }
    output.flags(prev_flags);
    output.fill(prev_fill);
    return output;
}

// TODO: to reform as a real manipulator
template <typename Ch>
inline auto& put_header(std::basic_ostream<Ch>& output, char const* header) {
    output << std::chrono::system_clock::now() << ':';
    output << put_thread_id << ':';
    output << '[' << header << "]:";
    return output;
}

template <typename Ch, std::size_t N>
inline std::size_t find_last_of(Ch const(&str)[N], Ch ch) {
    for (std::size_t i = N-1; i != 0; --i) {
        if (ch == str[i])
            return i + 1;
    }
    return 0;
}

} // end of namespace logging

#define EXO_FILETITLE_OFFSET (logging::find_last_of(__FILE__, '\\')

#define EXO_STRING(x) #x
#define EXO_TO_STRING(x) EXO_STRING(x)
#define EXO_HEADER EXO_FILETITLE_OFFSET + (__FILE__ "(" EXO_TO_STRING(__LINE__) ")"))
#define EXO_SITE EXO_HEADER

#define exo logging::put_header(*logging::logger(), EXO_HEADER)
#define EXO(expr) [](auto&& ref) -> auto&& {				\
        exo << #expr << " {" << ref << '[' << typeid (ref).name() << ']' << "}" << std::endl; \
        return std::forward<decltype (ref)>(ref);                       \
    } (std::move(expr))

#endif/*INCLUDE_LOGGING_HPP_6F155CFC_E243_4B1B_8F71_AA993B4C47A3*/
