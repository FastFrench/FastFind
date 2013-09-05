#include "FastFind.au3"

$FFhWnd = WinGetHandle("[ACTIVE]")
FFSetWnd($FFhWnd)
FFSnapShot()

Local $aCoords = FFNearestSpot(10, 25, 0, 0, 0x00FFFFFF, False)

If Not @error Then
	MsgBox(0, "Coords", $aCoords[0] & ", " & $aCoords[1])
Else
	MsgBox(0, "Coords", "Match not found.")
EndIf