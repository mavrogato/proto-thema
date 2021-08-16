
#include <fstream>

#include <gtest/gtest.h>

#include "win32-gdi.hpp"

class win32_gdi_test : public testing::Test {
protected:
    virtual void SetUp() {
        // this->mia = bitmap<1>(1,1);
        // this->tessera = bitmap<4>(1,1);
        // this->octo = bitmap<8>(1,1);
        // this->dekaexi = bitmap<16>(1,1);
        // this->eikositessera = bitmap<24>(1,1);
        // this->triandaduo = bitmap<32>(1,1);
    }

protected:
    // bitmap<1>  mia = {1,1};
    // bitmap<4>  tessera = {1,1};
    // bitmap<8>  octo = {1,1};
    // bitmap<16> dekaexi = {1,1};
    // bitmap<24> eikositessera = {1,1};
    // bitmap<32> triandaduo = {1,1};
};

TEST_F(win32_gdi_test, bitmap1) {
    auto mia = bitmap<1>(1, 256);

    ASSERT_EQ(sizeof (BITMAPINFOHEADER) +
              sizeof (bitmap<1>::color_table_type) +
              sizeof (void*) +
              sizeof (HBITMAP), sizeof (mia));

    ASSERT_NE(nullptr, mia.operator HBITMAP());
    ASSERT_NE(nullptr, mia.pixels());

    ASSERT_EQ(sizeof (BITMAPFILEHEADER), mia.head().size());
    ASSERT_EQ(mia.sx() * mia.cy(), mia.size());

    {
        auto view = mia.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
                                       {
                                           view[idx] = 0x00000080;
                                       });
    }

    std::ofstream output("bitmap1_test.bmp", std::ios::binary);
    ASSERT_EQ(true, output.is_open());
    output.write(&mia.head().front(), mia.head().size());
    output.write(&mia.info().front(), mia.info().size());
    output.write(reinterpret_cast<char const*>(mia.pixels()), mia.size());
}

TEST_F(win32_gdi_test, bitmap4) {
    auto tessera = bitmap<4>(1, 256);

    ASSERT_EQ(sizeof (BITMAPINFOHEADER) +
              sizeof (bitmap<4>::color_table_type) +
              sizeof (void*) +
              sizeof (HBITMAP), sizeof (tessera));

    ASSERT_NE(nullptr, tessera.operator HBITMAP());
    ASSERT_NE(nullptr, tessera.pixels());

    ASSERT_EQ(sizeof (BITMAPFILEHEADER), tessera.head().size());
    ASSERT_EQ(tessera.sx() * tessera.cy(), tessera.size());

    {
        auto view = tessera.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
                                       {
                                           view[idx] = 0x000000f0;
                                       });
    }

    std::ofstream output("bitmap4_test.bmp", std::ios::binary);
    ASSERT_EQ(true, output.is_open());
    output.write(&tessera.head().front(), tessera.head().size());
    output.write(&tessera.info().front(), tessera.info().size());
    output.write(reinterpret_cast<char const*>(tessera.pixels()), tessera.size());
}

TEST_F(win32_gdi_test, bitmap8) {
    auto octo = bitmap<8>(1, 256);

    ASSERT_EQ(sizeof (BITMAPINFOHEADER) +
              sizeof (bitmap<8>::color_table_type) +
              sizeof (void*) +
              sizeof (HBITMAP), sizeof (octo));

    ASSERT_NE(nullptr, octo.operator HBITMAP());
    ASSERT_NE(nullptr, octo.pixels());

    ASSERT_EQ(sizeof (BITMAPFILEHEADER), octo.head().size());
    ASSERT_EQ(octo.sx() * octo.cy(), octo.size());

    {
        auto view = octo.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
                                       {
                                           view[idx] = 0x000000ff;
                                       });
    }

    std::ofstream output("bitmap8_test.bmp", std::ios::binary);
    ASSERT_EQ(true, output.is_open());
    output.write(&octo.head().front(), octo.head().size());
    output.write(&octo.info().front(), octo.info().size());
    output.write(reinterpret_cast<char const*>(octo.pixels()), octo.size());
}

TEST_F(win32_gdi_test, bitmap16) {
    auto dekaexi = bitmap<16>(1, 256);

    ASSERT_EQ(sizeof (BITMAPINFOHEADER) +sizeof (void*) + sizeof (HBITMAP),
              sizeof (dekaexi));

    ASSERT_NE(nullptr, dekaexi.operator HBITMAP());
    ASSERT_NE(nullptr, dekaexi.pixels());

    ASSERT_EQ(sizeof (BITMAPFILEHEADER), dekaexi.head().size());
    ASSERT_EQ(dekaexi.sx() * dekaexi.cy(), dekaexi.size());

    {
        auto view = dekaexi.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
                                       {
                                           view[idx] = 0x0000ffff;
                                       });
    }

    std::ofstream output("bitmap16_test.bmp", std::ios::binary);
    ASSERT_EQ(true, output.is_open());
    output.write(&dekaexi.head().front(), dekaexi.head().size());
    output.write(&dekaexi.info().front(), dekaexi.info().size());
    output.write(reinterpret_cast<char const*>(dekaexi.pixels()), dekaexi.size());
}

TEST_F(win32_gdi_test, bitmap24) {
    auto eikositessera = bitmap<24>(1, 256);

    ASSERT_EQ(sizeof (BITMAPINFOHEADER) +sizeof (void*) + sizeof (HBITMAP),
              sizeof (eikositessera));

    ASSERT_NE(nullptr, eikositessera.operator HBITMAP());
    ASSERT_NE(nullptr, eikositessera.pixels());

    ASSERT_EQ(sizeof (BITMAPFILEHEADER), eikositessera.head().size());
    ASSERT_EQ(eikositessera.sx() * eikositessera.cy(), eikositessera.size());

    {
        auto view = eikositessera.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
                                       {
                                           view[idx] = 0x00ffffff;
                                       });
    }

    std::ofstream output("bitmap24_test.bmp", std::ios::binary);
    ASSERT_EQ(true, output.is_open());
    output.write(&eikositessera.head().front(), eikositessera.head().size());
    output.write(&eikositessera.info().front(), eikositessera.info().size());
    output.write(reinterpret_cast<char const*>(eikositessera.pixels()),
                 eikositessera.size());
}

TEST_F(win32_gdi_test, bitmap32) {
    auto triandaduo = bitmap<32>(1, 256);

    ASSERT_EQ(sizeof (BITMAPINFOHEADER) +sizeof (void*) + sizeof (HBITMAP),
              sizeof (triandaduo));

    ASSERT_NE(nullptr, triandaduo.operator HBITMAP());
    ASSERT_NE(nullptr, triandaduo.pixels());

    ASSERT_EQ(sizeof (BITMAPFILEHEADER), triandaduo.head().size());
    ASSERT_EQ(triandaduo.sx() * triandaduo.cy(), triandaduo.size());

    {
        auto view = triandaduo.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
                                       {
                                           view[idx] = 0x00ffffff;
                                       });
    }

    std::ofstream output("bitmap32_test.bmp", std::ios::binary);
    ASSERT_EQ(true, output.is_open());
    output.write(&triandaduo.head().front(), triandaduo.head().size());
    output.write(&triandaduo.info().front(), triandaduo.info().size());
    output.write(reinterpret_cast<char const*>(triandaduo.pixels()), triandaduo.size());
}

TEST_F(win32_gdi_test, topdown_dib) {
    bitmap<32> tbit(256, -256);

    ASSERT_NE(nullptr, tbit);
    {
        auto view = tbit.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp) {
                view[idx] = idx[0] | (idx[1] << 8);
            });
    }
    std::ofstream("top-down.bmp", std::ios::binary) << tbit;
}

TEST_F(win32_gdi_test, block_transfer) {
    constexpr int seeds[] = {
        2,    3,    5,    7,    11,   13,   17,   19,   23,   29,   31,   37,
        41,   43,   47,   53,   59,   61,   67,   71,   73,   79,   83,   89,
        97,   101,  103,  107,  109,  113,  127,  131,  137,  139,  149,  151,
        157,  163,  167,  173,  179,  181,  191,  193,  197,  199,  211,  223,
        227,  229,  233,  239,  241,  251,  257,  263,  269,  271,  277,  281,
        283,  293,  307,  311,  313,  317,  331,  337,  347,  349,  353,  359,
        367,  373,  379,  383,  389,  397,  401,  409,  419,  421,  431,  433,
        439,  443,  449,  457,  461,  463,  467,  479,  487,  491,  499,  503,
        509,  521,  523,  541,  547,  557,  563,  569,  571,  577,  587,  593,
        599,  601,  607,  613,  617,  619,  631,  641,  643,  647,  653,  659,
        661,  673,  677,  683,  691,  701,  709,  719,  727,  733,  739,  743,
        751,  757,  761,  769,  773,  787,  797,  809,  811,  821,  823,  827,
        829,  839,  853,  857,  859,  863,  877,  881,  883,  887,  907,  911,
        919,  929,  937,  941,  947,  953,  967,  971,  977,  983,  991,  997,
        1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
        1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163,
        1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249,
        1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321,
        1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439,
        1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
        1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601,
        1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693,
        1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783,
        1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877,
        1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987,
        1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069,
        2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141, 2143,
        2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267,
        2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347,
        2351, 2357, 2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
        2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543,
        2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657,
        2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699, 2707, 2711, 2713,
        2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801,
        2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903,
        2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011,
        3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109,
    };
    constexpr auto cx = 2 * 3 * 5 * 7 * 11;
    constexpr auto cy = 13 * 17 * 19;
    auto dev32 = device(bitmap<32>(cx, -cy));
    {
        auto view = concurrency::array_view<std::uint32_t, 1>(cx * cy, dev32.pixels());
        concurrency::parallel_for_each(view.extent, [seeds, view](auto idx) restrict(amp) {
                auto i = idx[0];
                for (auto m : seeds) {
                    if (0 == i % m && 1 != i / m) {
                        view[idx] = 0x00ffffff;
                        break;
                    }
                }
            });
    }
    auto dev1 = device(bitmap<1>(cx, -cy));
    BitBlt(dev1, 0, 0, cx, cy, dev32, 0, 0, NOTSRCCOPY);
    *(dev1.pixels()) ^= 0x00000040;
    // {
    //     using namespace concurrency;
    //     int wx = dev1.wx();
    //     auto view = array_view<std::uint32_t, 2>(cy, wx, dev1.pixels());
    //     parallel_for_each(extent<2>{cy/2, wx},
    //                       [cy, view](auto idx) restrict(amp) {
    //                           auto rdx = index<2>{cy - idx[0] - 1, idx[1]};
    //                           auto tmp = view[idx];
    //                           view[idx] = view[rdx];
    //                           view[rdx] = tmp;
    //                       });
    // }
    std::ofstream("prime-19th.bmp", std::ios::binary) << dev1;
    auto tmp = device(bitmap<32>(2*3*5*7*11*13*17, -19));
    std::memcpy(tmp.pixels(), dev32.pixels(), 2*3*5*7*11*13*17*19 * 4);
    auto dev1s = device(bitmap<1>(2*3*5*7*11*13*17, -19));
    BitBlt(dev1s, 0, 0, dev1s.cx(), dev1s.cy(), tmp, 0, 0, NOTSRCCOPY);
    *(dev1s.pixels()) ^= 0x00000040;
    std::ofstream("prime-19th-align.bmp", std::ios::binary) << dev1s;
}

TEST_F(win32_gdi_test, subword_access) {
    bitmap<1> bmp(32, -256);
    {
        auto view = bmp.view();
        concurrency::parallel_for_each(view.extent, [view](auto idx) restrict(amp)
            {
                view[idx] = (0x80000000) >> idx[0];
            });
    }
    std::ofstream("output.bmp", std::ios::binary) << bmp;
}
