# Fait jouer FNF 3DS tout seul dans Azahar (fenêtre réduite) et affiche son journal.
# usage : powershell -File autotest.ps1 <chanson> [secondes]
param([string]$song = "sensational", [int]$seconds = 40)
$A = if ($env:AZAHAR_DIR) { $env:AZAHAR_DIR } else { "$PSScriptRoot\..\..\azahar\azahar-windows-msvc-2126.2" }
$src = "$PSScriptRoot\..\sd\3ds\fnf3ds"
$app = (Resolve-Path "$PSScriptRoot\..\fnf3ds.3dsx").Path
$sd = "$A\user\sdmc\3ds\fnf3ds"

Get-Process azahar -ErrorAction SilentlyContinue | Stop-Process -Force
for ($r = 0; $r -lt 30 -and (Get-Process azahar -ErrorAction SilentlyContinue); $r++) { Start-Sleep 1 }
Start-Sleep 1
New-Item -ItemType Directory -Force $sd | Out-Null
robocopy $src $sd /E /XO /NFL /NDL /NJH /NJS /NP | Out-Null
if (-not (Test-Path "$A\user\sdmc\3ds\dspfirm.cdc")) { [IO.File]::WriteAllBytes("$A\user\sdmc\3ds\dspfirm.cdc", [byte[]](0) * 16) }
Get-ChildItem $sd -Filter "shot_*.bmp" -ErrorAction SilentlyContinue | Remove-Item
if (Test-Path "$sd\log.txt") { Remove-Item "$sd\log.txt" }
[IO.File]::WriteAllText("$sd\autotest.txt", "$song $seconds")

$p = Start-Process "$A\azahar.exe" -ArgumentList ('"' + $app + '"') -PassThru -WindowStyle Minimized
$done = $false
for ($i = 0; $i -lt ($seconds + 90); $i++) {
    Start-Sleep 1
    if ($p.HasExited) { break }
    if ((Test-Path "$sd\log.txt") -and (Select-String -Path "$sd\log.txt" -Pattern "AUTOTEST FINI" -Quiet)) { $done = $true; break }
}
Start-Sleep 1
Stop-Process $p -Force -ErrorAction SilentlyContinue
Remove-Item "$sd\autotest.txt" -ErrorAction SilentlyContinue
if (Test-Path "$sd\log.txt") { Get-Content "$sd\log.txt" } else { "pas de journal" }
if (-not $done) { "PAS FINI (plantage ou trop long)" }
