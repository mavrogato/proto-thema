#ifndef INCLUDE_GENERATOR_HPP_9FF66F69_AF5C_4B40_97EF_C8603791C38C
#define INCLUDE_GENERATOR_HPP_9FF66F69_AF5C_4B40_97EF_C8603791C38C

#include <coroutine>
#include <memory>
#include <iterator>

template <class T>
struct generator {
    struct promise_type {
        T const* value_;

        generator get_return_object() noexcept { return generator{*this}; }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend()   noexcept { return {}; }
        void unhandled_exception() { throw; }
        std::suspend_always yield_value(T const& value) noexcept {
            this->value_ = std::addressof(value);
            return {};
        }
        void return_void() noexcept { }
    };
    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using differnce_type    = std::ptrdiff_t;
        using value_type        = T;
        using reference         = T const&;
        using pointer           = T const*;

        std::coroutine_handle<promise_type> coro_ = nullptr;

        iterator() = default;
        explicit iterator(std::coroutine_handle<promise_type> coro) noexcept : coro_(coro) { }
        iterator& operator++() {
            if (this->coro_.done()) {
                this->coro_ = nullptr;
            }
            else {
                this->coro_.resume();
            }
            return *this;
        }
        void operator++(int) {
            ++*this;
        }
        [[nodiscard]] bool operator==(iterator const& rhs) const noexcept {
            return this->coro_ == rhs.coro_;
        }
        [[nodiscard]] bool operator!=(iterator const& rhs) const noexcept {
            return this->coro_ != rhs.coro_;
        }
        [[nodiscard]] reference operator*() const noexcept {
            return *this->coro_.promise().value_;
        }
        [[nodiscard]] pointer operator->() const noexcept {
            return this->coro_.promise().value_;
        }
    };
    [[nodiscard]] iterator begin() {
        if (this->coro_) {
            if (this->coro_.done()) {
                return {};
            }
            else {
                this->coro_.resume();
            }
        }
        return iterator{this->coro_};
    }
    [[nodiscard]] iterator end() noexcept { return {}; }

    [[nodiscard]] bool empty() noexcept { return this->coro_.done(); }

    explicit generator(promise_type& prom) noexcept
        : coro_(std::coroutine_handle<promise_type>::from_promise(prom))
        {
        }
    generator() = default;
    generator(generator&& rhs) noexcept
        : coro_(std::exchange(rhs.coro_, nullptr))
        {
        }
    ~generator() noexcept {
        if (this->coro_) {
            this->coro_.destroy();
        }
    }
    generator& operator=(generator const&) = delete;
    generator& operator=(generator&& rhs) {
        if (this != &rhs) {
            this->coro_ = std::exchange(rhs.coro_, nullptr);
        }
        return *this;
    }
private:
    std::coroutine_handle<promise_type> coro_ = nullptr;
};

#endif/*INCLUDE_GENERATOR_HPP_9FF66F69_AF5C_4B40_97EF_C8603791C38C*/
