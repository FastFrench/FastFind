#include "FastFind.au3"

$FFhWnd = WinGetHandle("[ACTIVE]")
FFSetWnd($FFhWnd)
FFSnapShot()

Local $aCoords = FFBestSpot(20, 25, 150, 0, 0, 0x00FFFFFF, False)

If Not @error Then
	MsgBox(0, "Coords", $aCoords[0] & ", " & $aCoords[1])
Else
	MsgBox(0, "Coords", "Match not found.")
EndIf