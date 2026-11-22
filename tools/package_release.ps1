# Makes the release files from an already converted mod (sd/3ds/fnf3ds/cyber) and a fresh `make`:
#   release/CyberSensation-3DS.cia        engine only (data on the SD card)
#   release/CyberSensation-3DS-full.cia   engine + converted mod in the romfs, nothing to copy
#   release/CyberSensation-3DS.3dsx       Homebrew Launcher version (data on the SD card)
#   release/CyberSensation-3DS-SD.zip     the converted mod, to unzip at the root of the SD card
# usage: powershell -File tools/package_release.ps1      (needs makerom, see build_cia.ps1)
$root = Resolve-Path "$PSScriptRoot\.."
$rel = "$root\release"
New-Item -ItemType Directory -Force $rel | Out-Null
& "$PSScriptRoot\build_cia.ps1"
& "$PSScriptRoot\build_cia.ps1" -Full
Move-Item -Force "$root\CyberSensation-3DS.cia" "$rel\CyberSensation-3DS.cia"
Move-Item -Force "$root\CyberSensation-3DS-full.cia" "$rel\CyberSensation-3DS-full.cia"
Copy-Item -Force "$root\fnf3ds.3dsx" "$rel\CyberSensation-3DS.3dsx"
# zip with the SD layout 3ds/fnf3ds/cyber/... (no scores, logs or screenshots)
$stage = "$root\build\sd_zip"
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
robocopy "$root\sd\3ds\fnf3ds\cyber" "$stage\3ds\fnf3ds\cyber" /E /XF scores.txt *.bmp log.txt /NFL /NDL /NJH /NJS /NP | Out-Null
$zip = "$rel\CyberSensation-3DS-SD.zip"
if (Test-Path $zip) { Remove-Item $zip }
Add-Type -AssemblyName System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory($stage, $zip, [IO.Compression.CompressionLevel]::Optimal, $false)
Get-ChildItem $rel | Select-Object Name, @{n = "MB"; e = { [math]::Round($_.Length / 1MB) } }
