#include "hls_module.h"

void hls_module_template(
    const axi512_t *input,
    axi512_t *output,
    int word_count
)
{
#pragma HLS INTERFACE m_axi port=input  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem1

#pragma HLS INTERFACE s_axilite port=input      bundle=control
#pragma HLS INTERFACE s_axilite port=output     bundle=control
#pragma HLS INTERFACE s_axilite port=word_count bundle=control
#pragma HLS INTERFACE s_axilite port=return     bundle=control

    // Placeholder datapath only.
    // Replace this loop with the real operator implementation.
    for (int i = 0; i < word_count; ++i)
    {
#pragma HLS PIPELINE II=1
        output[i] = input[i];
    }
}
