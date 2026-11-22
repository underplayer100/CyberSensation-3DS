# Builds the .cia from fnf3ds.elf (run `make` first, and tools/make_meta.py once).
# usage: powershell -File tools/build_cia.ps1      (needs makerom in PATH or MAKEROM env var)
$root = Resolve-Path "$PSScriptRoot\.."
$makerom = if ($env:MAKEROM) { $env:MAKEROM } else { "makerom" }
New-Item -ItemType Directory -Force "$root\meta\romfs" | Out-Null
if (-not (Test-Path "$root\meta\romfs\.keep")) { Set-Content "$root\meta\romfs\.keep" "x" }
& $makerom -f cia -o "$root\CyberSensation-3DS.cia" -elf "$root\fnf3ds.elf" -rsf "$root\meta\app.rsf" `
    -icon "$root\meta\icon.smdh" -banner "$root\meta\banner.bnr" -DAPP_ROMFS="$root\meta\romfs" `
    -DAPP_TITLE=CyberSensation -DAPP_PRODUCT_CODE=CTR-P-FNFC -DAPP_UNIQUE_ID=0xF3D51 -DAPP_ENCRYPTED=false `
    -DAPP_SYSTEM_MODE=64MB -DAPP_SYSTEM_MODE_EXT=Legacy
