#ifndef HLS_MODULE_TEMPLATE_H
#define HLS_MODULE_TEMPLATE_H

#include <ap_int.h>

using axi512_t = ap_uint<512>;

// Minimal reusable HLS top-level interface.
//
// Default transport contract:
//   - 512-bit AXI memory words
//   - BF16-oriented packing: 32 x 16-bit values / word
//   - runtime word_count
//
// Rename this function and extend the arguments when creating a real module.
void hls_module_template(
    const axi512_t *input,
    axi512_t *output,
    int word_count
);

#endif
