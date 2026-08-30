# HLS Module Template

A minimal reusable AMD Vitis HLS scaffold extracted from a validated Qwen3-ASR RMSNorm development flow.

The purpose of this repository is simple: **copy it, rename the top function, replace the placeholder datapath, attach real input/golden data, and start C Simulation immediately.**

## Directory

```text
hls-module-template/
├── src/
│   ├── hls_module.h
│   └── hls_module.cpp
├── tb/
│   └── test_hls_module.cpp
├── data/
│   └── README.md
├── scripts/
│   ├── hls_config.cfg
│   ├── run_csim.ps1
│   └── run_csynth.ps1
├── .gitignore
└── README.md
```

## Preserved engineering conventions

This scaffold keeps the conventions already validated in the source project:

- AMD Vitis HLS 2026.1 command-line workflow.
- Temporary synthesis part: `xc7a100tcsg324-1`.
- Target clock: `10 ns`.
- Clock uncertainty: `12%`.
- AXI4 memory-mapped data ports plus AXI4-Lite control ports.
- 512-bit memory-word convention.
- 64-byte Host-side alignment convention.
- Explicit testbench / golden validation before trusting synthesis results.
- Generated HLS build products kept outside `src/`, `tb/`, and `data/`.

> `xc7a100tcsg324-1` is only the development/synthesis target inherited from the validated RMSNorm flow. Change it when moving to the final FPGA platform such as U250.

## AXI interface contract

The default skeleton exposes:

```cpp
void hls_module_template(
    const ap_uint<512> *input,
    ap_uint<512> *output,
    int word_count
);
```

Interface mapping:

```text
input      -> m_axi gmem0 + s_axilite control
output     -> m_axi gmem1 + s_axilite control
word_count -> s_axilite control
return     -> s_axilite control
```

Use separate `gmem` bundles for logically independent high-bandwidth buffers unless the design deliberately shares a memory port.

The placeholder datapath is only a pass-through loop with `#pragma HLS PIPELINE II=1`. It exists only to prove that the scaffold can compile and simulate.

## dtype and packing convention

The external transport type is deliberately kept as:

```cpp
ap_uint<512>
```

so the bus layer does not force one arithmetic dtype.

Current activation packing convention:

```text
AXI width : 512 bits = 64 bytes
BF16      : 32 values / AXI word
FP32      : 16 values / AXI word
```

For BF16 packing, lane `n` occupies:

```text
bits [16*n + 15 : 16*n]
```

A validated RMSNorm implementation used BF16 input/output while retaining FP32 internal computation and accumulation. Future operators may use a different internal dtype without changing the external AXI word width.

## shape and layout convention

Do not hard-code a real-model shape into the reusable skeleton unless the hardware architecture requires compile-time dimensions.

The Qwen3-ASR activation used to establish this workflow was:

```text
shape    : [65, 2048]
dtype    : FP32 before Host BF16 conversion
layout   : token-major contiguous
elements : 133120
```

For every new module, record the actual tensor shape and layout in `data/README.md` or a `meta.txt` beside the vectors.

If dimensions are runtime-controlled, expose them through AXI4-Lite arguments. If a dimension is compile-time fixed for optimization, document that decision explicitly.

## Host alignment

The current Host-side convention is **64-byte alignment**:

```text
512-bit AXI word = 64 bytes
```

Preserve this convention unless the runtime or target platform imposes a stronger alignment requirement.

## Data / Golden interface

Put test vectors under `data/` instead of embedding machine-specific absolute paths in source code.

Recommended convention:

```text
data/input.bin
data/params_*.bin
data/golden.bin
data/meta.txt
```

For every raw binary, record:

```text
name
dtype
shape
layout
element count
byte count
generator/reference source
```

The testbench should check file size before reading. A shape mismatch must fail loudly instead of silently producing misleading numerical results.

## Validation flow

Use the same order for each new module:

```text
1. Small deterministic/artificial case
        ↓
2. CSim must PASS
        ↓
3. Real-model input + trusted CPU/Python Golden
        ↓
4. Compare bit-exactly or with explicit numerical tolerances
        ↓
5. C Synthesis
        ↓
6. Inspect timing, latency/II and BRAM/DSP/FF/LUT
        ↓
7. Optimize only after functional validation is stable
        ↓
8. Re-run real-model validation after every arithmetic/packing optimization
```

Suggested comparison policy:

- Integer / packed control logic: prefer bit-exact comparison.
- BF16 against an equivalent BF16 software reference: prefer bit-exact comparison when conversion rules are identical.
- Reduced precision against FP32 Golden: report at least max absolute error, mean absolute error, RMSE, and application-relevant thresholds.

## C Simulation

Open a PowerShell with the Vitis 2026.1 environment initialized and run from the repository root:

```powershell
./scripts/run_csim.ps1
```

Equivalent command:

```powershell
vitis-run --mode hls --csim --config scripts/hls_config.cfg --work_dir build/csim
```

The included testbench is only a deterministic pass-through smoke test. For a real operator, replace or extend it with:

1. artificial edge cases;
2. binary input loading;
3. trusted Golden loading/calculation;
4. mismatch/error metrics;
5. non-zero exit code on failure.

## C Synthesis

Run:

```powershell
./scripts/run_csynth.ps1
```

Equivalent Vitis 2026.1 command:

```powershell
v++ -c --mode hls --config scripts/hls_config.cfg --work_dir build/csynth
```

After synthesis, inspect at least:

```text
Estimated clock / timing
Top-level latency
Loop II
BRAM18K
DSP
FF
LUT
Interface inference
```

A synthesis PASS is not a functional PASS. CSim / Golden validation remains the functional gate.

## Starting a new module

```text
1. Copy this repository or directory.
2. Rename `hls_module_template` in header/source/testbench/config.
3. Define the new function arguments and AXI bundles.
4. Replace the pass-through body with the operator logic.
5. Record dtype/shape/layout/packing in `data/README.md`.
6. Add artificial vectors and a trusted Golden.
7. Run CSim until PASS.
8. Run C Synthesis and inspect reports.
9. Only then begin performance/resource optimization.
```

## Conventions that should usually stay stable

```text
Directory split        src / tb / data / scripts
Host alignment         64 B
External packed bus    AXI512
Control                AXI4-Lite
Validation order       artificial -> real Golden -> synthesis
Build output            build/
```

Stable conventions reduce integration errors and repeated project setup work.

---

## 下一个模块接进来时，我需要改哪几个地方？

1. **模块名**：同步修改 `src/hls_module.h`、`src/hls_module.cpp`、`tb/test_hls_module.cpp` 和 `scripts/hls_config.cfg` 中的顶层函数名。
2. **顶层接口**：按新模块增加或删除输入、输出、参数，同时更新 `m_axi` / `s_axilite` pragma 和 testbench 调用。
3. **dtype / packing**：确定外部总线里的元素格式，例如 BF16 每个 AXI512 word 32 个元素，FP32 每个 word 16 个元素。
4. **shape / layout**：记录真实张量的维度、连续方式和元素顺序；运行时维度通过 AXI4-Lite 传入，固定维度则明确写成编译期约束。
5. **核心计算**：只替换 `src/hls_module.cpp` 中的 placeholder pass-through datapath；先保证正确，再做 pipeline/unroll/partition 等优化。
6. **testbench + Golden**：加入人工小样例和真实输入，定义 bit-exact 或误差阈值，并确保失败时返回非零退出码。
7. **综合配置**：根据实际目标板修改 `part`、clock，以及必要的接口/优化参数；随后重新检查 timing、latency、II、BRAM、DSP、FF、LUT。

做到这 7 点，新模块就真正接进来了。
