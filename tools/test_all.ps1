# Joue toutes les chansons de songs.lst dans Azahar, garde une capture par chanson et un résumé.
# usage : powershell -File test_all.ps1 [secondes] [dossier de sortie]
param([int]$seconds = 15, [string]$out = "$PSScriptRoot\..\test_all")
$A = if ($env:AZAHAR_DIR) { $env:AZAHAR_DIR } else { "$PSScriptRoot\..\..\azahar\azahar-windows-msvc-2126.2" }
$sd = "$A\user\sdmc\3ds\fnf3ds"
New-Item -ItemType Directory -Force $out | Out-Null
$ids = Get-Content "$PSScriptRoot\..\sd\3ds\fnf3ds\cyber\songs.lst" | Where-Object { $_ } | ForEach-Object { $_.Split('|')[0] }
$summary = @()
foreach ($id in $ids) {
    $log = & powershell -ExecutionPolicy Bypass -File "$PSScriptRoot\autotest.ps1" $id $seconds
    $fini = ($log | Select-String "AUTOTEST FINI" | Select-Object -Last 1)
    $status = if ($fini) { $fini.Line } else { "PLANTAGE / PAS FINI" }
    $summary += "$id`t$status"
    $safe = $id -replace '[^\w\-]', '_'
    foreach ($n in 200, 700) {
        $bmp = "$sd\shot_{0:D5}.bmp" -f $n
        if (Test-Path $bmp) { Copy-Item $bmp "$out\${safe}_$n.bmp" -Force }
    }
    "$id : $status"
}
$summary | Set-Content "$out\resume.txt" -Encoding utf8
"TOUT FINI"
