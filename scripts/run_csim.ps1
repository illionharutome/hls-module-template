$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

vitis-run --mode hls --csim `
    --config scripts/hls_config.cfg `
    --work_dir build/csim

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
