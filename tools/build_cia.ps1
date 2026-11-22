# Builds the .cia from fnf3ds.elf (run `make` first, and tools/make_meta.py once).
# usage: powershell -File tools/build_cia.ps1            .cia without game data (data on the SD card)
#        powershell -File tools/build_cia.ps1 -Full      .cia with the converted mod inside (romfs:/cyber)
# needs makerom in PATH or the MAKEROM env var
param([switch]$Full)
$root = Resolve-Path "$PSScriptRoot\.."
$makerom = if ($env:MAKEROM) { $env:MAKEROM } else { "makerom" }
New-Item -ItemType Directory -Force "$root\meta\romfs" | Out-Null
if (-not (Test-Path "$root\meta\romfs\.keep")) { Set-Content "$root\meta\romfs\.keep" "x" }
$romfs = "$root\meta\romfs"
$out = "$root\CyberSensation-3DS.cia"
if ($Full) {
    $data = "$root\sd\3ds\fnf3ds\cyber"
    if (-not (Test-Path "$data\songs.lst")) { throw "convert the mod first: $data\songs.lst is missing" }
    $romfs = "$root\build\romfs_full"
    if (Test-Path $romfs) { Remove-Item -Recurse -Force $romfs }
    New-Item -ItemType Directory -Force $romfs | Out-Null
    Copy-Item "$root\meta\romfs\*" $romfs -Recurse -Force
    # only what the game reads (no scores, logs or screenshots)
    robocopy $data "$romfs\cyber" /E /XF scores.txt *.bmp log.txt /NFL /NDL /NJH /NJS /NP | Out-Null
    $out = "$root\CyberSensation-3DS-full.cia"
}
& $makerom -f cia -o $out -elf "$root\fnf3ds.elf" -rsf "$root\meta\app.rsf" `
    -icon "$root\meta\icon.smdh" -banner "$root\meta\banner.bnr" -DAPP_ROMFS="$romfs" `
    -DAPP_TITLE=CyberSensation -DAPP_PRODUCT_CODE=CTR-P-FNFC -DAPP_UNIQUE_ID=0xF3D51 -DAPP_ENCRYPTED=false `
    -DAPP_SYSTEM_MODE=64MB -DAPP_SYSTEM_MODE_EXT=Legacy
if ($LASTEXITCODE -ne 0) { throw "makerom failed" }
"built $out ($([math]::Round((Get-Item $out).Length / 1MB)) MB)"
