$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

v++ -c --mode hls `
    --config scripts/hls_config.cfg `
    --work_dir build/csynth

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
