param([string]$Compiler='cc')
$ErrorActionPreference='Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
Push-Location $repoRoot
try {
  node tests/dashboard_test.cjs
  if ($LASTEXITCODE -ne 0) { throw 'Dashboard tests failed' }
  node tests/hardware_contract_test.cjs
  if ($LASTEXITCODE -ne 0) { throw 'Hardware contract failed' }
  New-Item -ItemType Directory -Path 'build' -Force | Out-Null
  & $Compiler '-std=c99' 'tests/control_core_test.c' '-o' 'build/control_core_test.exe'
  if ($LASTEXITCODE -ne 0) { throw 'Native core test compilation failed' }
  & './build/control_core_test.exe'
  if ($LASTEXITCODE -ne 0) { throw 'Native core tests failed' }
} finally { Pop-Location }
