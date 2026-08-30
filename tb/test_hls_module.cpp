#include <iostream>
#include <vector>

#include "hls_module.h"

int main()
{
    constexpr int WORD_COUNT = 8;

    std::vector<axi512_t> input(WORD_COUNT);
    std::vector<axi512_t> output(WORD_COUNT);

    // Small deterministic artificial case.
    for (int i = 0; i < WORD_COUNT; ++i)
    {
        axi512_t word = 0;

        // Fill every 16-bit lane with a deterministic pattern.
        for (int lane = 0; lane < 32; ++lane)
        {
            const ap_uint<16> value =
                static_cast<unsigned>(i * 32 + lane);

            word.range(
                lane * 16 + 15,
                lane * 16
            ) = value;
        }

        input[i] = word;
    }

    hls_module_template(
        input.data(),
        output.data(),
        WORD_COUNT
    );

    int mismatch = 0;

    for (int i = 0; i < WORD_COUNT; ++i)
    {
        if (output[i] != input[i])
        {
            ++mismatch;
            std::cerr
                << "Mismatch at AXI word "
                << i << "\n";
        }
    }

    if (mismatch == 0)
    {
        std::cout
            << "PASS: HLS template CSim smoke test.\n";
        return 0;
    }

    std::cout
        << "FAIL: " << mismatch
        << " mismatched AXI words.\n";

    return 1;
}
