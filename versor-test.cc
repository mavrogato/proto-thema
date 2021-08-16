
#include <gtest/gtest.h>

#include "versor.hpp"
#include <complex>
#include <array>

class versor_test : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(versor_test, initializer) {
    versor<double, 3> v3d;
    ASSERT_EQ(0, v3d[0]);
    ASSERT_EQ(0, v3d[1]);
    ASSERT_EQ(0, v3d[2]);

    versor<float, 10> v10f;
    ASSERT_EQ(true, std::all_of(v10f.begin(), v10f.end(), [](float x) { return 0 == x; }));

    versor<int, 4> v4i(1, 2, 3, 4);
    ASSERT_EQ(1, v4i[0]);
    ASSERT_EQ(2, v4i[1]);
    ASSERT_EQ(3, v4i[2]);
    ASSERT_EQ(4, v4i[3]);

    versor<std::complex<double>, 2> v2c(std::complex<double>(1, 2),
                                        std::complex<double>(3, 4));
    ASSERT_EQ(1.0, v2c[0].real());
    ASSERT_EQ(2.0, v2c[0].imag());
    ASSERT_EQ(3.0, v2c[1].real());
    ASSERT_EQ(4.0, v2c[1].imag());

    ASSERT_EQ((versor<double, 3>(1, 2, 3)),
              (versor<double, 3>(1.0, 2.0, 3.0)));
}

TEST_F(versor_test, initializer_amp) {
    std::array<versor<double, 3>, 10> buf = { };
    {
        auto view = concurrency::array_view<versor<double, 3>, 1>(10, buf);
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp) {
                view[idx] = versor<double, 3>(1, 2, 3);
            });
    }
    for (auto item : buf) {
        ASSERT_EQ((versor<double, 3>(1, 2, 3)), item);
    }
}

TEST_F(versor_test, vectorized_arithmetic) {
    using vec = versor<int, 3>;
    ASSERT_EQ(vec(4, 4, 4), vec(1, 2, 3) + vec(3, 2, 1));
    ASSERT_EQ(vec(3, 2, 1), vec(4, 4, 4) - vec(1, 2, 3));
    ASSERT_EQ(vec(3, 4, 3), vec(1, 2, 3) * vec(3, 2, 1));
    ASSERT_EQ(vec(1, 2, 3), vec(1, 4, 9) / vec(1, 2, 3));
}

TEST_F(versor_test, vectorized_arithmetic_amp) {
    using vec = versor<float, 3>;
    std::array<vec, 4> ret;
    {
        auto lhs = make_amp_array(vec{ 1, 2, 3 },
                                  vec{ 4, 4, 4 },
                                  vec{ 1, 2, 3 },
                                  vec{ 1, 4, 9 });
        auto rhs = make_amp_array(vec{ 3, 2, 1 },
                                  vec{ 1, 2, 3 },
                                  vec{ 3, 2, 1 },
                                  vec{ 1, 2, 3 });
        auto view = concurrency::array_view<vec, 1>(4, &ret.front());
        concurrency::parallel_for_each(
            view.extent,
            [&lhs, &rhs, view](auto idx) restrict(amp)
            {
                switch (idx[0]) {
                case 0: view[idx] = lhs[idx] + rhs[idx]; break;
                case 1: view[idx] = lhs[idx] - rhs[idx]; break;
                case 2: view[idx] = lhs[idx] * rhs[idx]; break;
                case 3: view[idx] = lhs[idx] / rhs[idx]; break;
                }
            });
    }
    ASSERT_EQ(vec(4, 4, 4), ret[0]);
    ASSERT_EQ(vec(3, 2, 1), ret[1]);
    ASSERT_EQ(vec(3, 4, 3), ret[2]);
    ASSERT_EQ(vec(1, 2, 3), ret[3]);
}

TEST_F(versor_test, helmholz_algebra) {
    using vec = versor<double, 3>;
    ASSERT_EQ(vec(3, 6, 9), vec(1, 2, 3) * 3);
    ASSERT_EQ(vec(6, 4, 2), 2 * vec(3, 2, 1));
    ASSERT_EQ(50, norm2(vec(3, 4, 5)));
    ASSERT_EQ(20, inner(vec(1, 2, 3), vec(2, 3, 4)));
    ASSERT_EQ(vec(0, 0, 1), cross(vec(1, 0, 0), vec(0, 1, 0)));
}

TEST_F(versor_test, helmholz_algebra_amp) {
    using vec = versor<double, 3>;
    std::array<vec, 10> ret;
    {
        {
            auto view = concurrency::array_view<vec, 1>(10, &ret.front());
            concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp) {
                    view[idx] = vec(1, 2, 3) * idx[0];
                });
        }
        for (std::size_t i = 0; i < ret.size(); ++i) {
          ASSERT_EQ(vec(1.0 * i, 2.0 * i, 3.0 * i), ret[i]);
        }
    }
    {
        {
            auto view = concurrency::array_view<vec, 1>(10, &ret.front());
            concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp) {
                    view[idx] = idx[0] * vec(1, 2, 3);
                });
        }
        for (std::size_t i = 0; i < ret.size(); ++i) {
          ASSERT_EQ(vec(i * 1.0, i * 2.0, i * 3.0), ret[i]);
        }
    }
    {
        {
            auto view = concurrency::array_view<vec, 1>(10, &ret.front());
            concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp) {
                    double x = idx[0];
                    view[idx] = vec(norm2(vec(x, 2*x, 3*x)),
                                    norm2(vec(x, 2*x, 3*x)),
                                    norm2(vec(x, 2*x, 3*x)));
                });
        }
        for (std::size_t i = 0; i < ret.size(); ++i) {
            ASSERT_EQ(vec(i*i*14.0, i*i*14.0, i*i*14.0), ret[i]);
        }
    }
}
