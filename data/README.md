# Data Contract

This directory holds module-specific input vectors, parameters, Golden outputs, and tensor metadata.

## Recommended files

```text
input.bin
params_*.bin
golden.bin
meta.txt
```

Do **not** hard-code machine-specific absolute data paths in HLS source code or the testbench. Paths should remain repository-relative.

## Required metadata

For every binary tensor, record at least:

```text
name
semantic meaning
dtype
shape
layout
element count
byte count
packing rule, if packed
generator/reference source
```

Example:

```text
name        : input.bin
dtype       : BF16
shape       : [65, 2048]
layout      : token-major contiguous
elements    : 133120
bytes       : 266240
AXI packing : 32 BF16 values / 512-bit word
source      : Host conversion from traced FP32 activation
```

## Validation rules

The testbench should verify expected file sizes before reading binary vectors.

Use a comparison rule that matches the module:

- Integer / control / packing logic: bit-exact.
- BF16 against matching BF16 reference: bit-exact when conversion rules are identical.
- Reduced precision against FP32 Golden: report max absolute error, mean absolute error, RMSE, and meaningful thresholds.

Keep large model tensors out of Git when appropriate. The data interface contract should remain version-controlled even when the actual binary vectors are generated locally.
