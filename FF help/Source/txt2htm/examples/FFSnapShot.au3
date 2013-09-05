#include "FastFind.au3"

AutoItSetOption("WinTitleMatchMode", 4)

$FFhWnd = WinGetHandle("[ACTIVE]")
FFSetWnd($FFhWnd)

FFSnapShot(0, 0, 300, 150)
FFSaveBMP(@YEAR & "-" & @MON & "-" & @MDAY & "_" & @HOUR & @MIN & @SEC, false)