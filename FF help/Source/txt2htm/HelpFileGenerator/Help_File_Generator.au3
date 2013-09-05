#include <File.au3>
#include <GuiEdit.au3>
#include <GUIConstants.au3>
#include <WindowsConstants.au3>

Opt("GUIOnEventMode", True)

Global _
		$sUDF = IniRead("Defaults.ini", "MAIN", "Library", "My.au3"), _
		$sFuncPrefix = IniRead("Defaults.ini", "MAIN", "FuncPrefix", "_MY"), _
		$sOnSuccess = IniRead("Defaults.ini", "MAIN", "Success", ""), _
		$sOnFailure = IniRead("Defaults.ini", "MAIN", "Failure", "")

$Form1 = GUICreate("Help File Generator", 563, 441, -1, -1)
GUISetOnEvent($GUI_EVENT_CLOSE, '_Exit')
$Edit1 = GUICtrlCreateEdit("", 13, 96, 465, 337, -1, $WS_EX_CLIENTEDGE)
GUICtrlSetData($Edit1, "")
$Input1 = GUICtrlCreateInput($sFuncPrefix, 13, 24, 105, 21, -1, $WS_EX_CLIENTEDGE) ; Function Name
GUICtrlCreateLabel("Function Name", 5, 6, 76, 17)
$Input2 = GUICtrlCreateInput("", 13, 64, 105, 21, -1, $WS_EX_CLIENTEDGE) ; Function Description
GUICtrlCreateLabel("Function Description", 5, 46, 101, 17)
$Input8 = GUICtrlCreateInput("", 133, 24, 105, 21, -1, $WS_EX_CLIENTEDGE) ; Syntax
GUICtrlCreateLabel("Syntax", 125, 6, 36, 17)
$Input3 = GUICtrlCreateInput("1", 133, 64, 105, 21, -1, $WS_EX_CLIENTEDGE) ; Number of parameters
$Updown1 = GUICtrlCreateUpdown(-1)
GUICtrlSetLimit(-1, 20, 0)
GUICtrlCreateLabel("Number of Parameters", 125, 46, 109, 17)
$Input5 = GUICtrlCreateInput($sOnSuccess, 253, 24, 105, 21, -1, $WS_EX_CLIENTEDGE) ; On Success
GUICtrlCreateLabel("On Success", 245, 6, 62, 17)
$Input6 = GUICtrlCreateInput($sOnFailure, 253, 64, 105, 21, -1, $WS_EX_CLIENTEDGE) ; On Failure
GUICtrlCreateLabel("On Failure", 245, 46, 52, 17)
$Input9 = GUICtrlCreateInput("None.", 373, 24, 105, 21, -1, $WS_EX_CLIENTEDGE) ; Remarks
GUICtrlCreateLabel("Remarks", 365, 6, 46, 17)
$Input10 = GUICtrlCreateInput("None.", 373, 64, 105, 21, -1, $WS_EX_CLIENTEDGE) ; Related
GUICtrlCreateLabel("Related", 365, 46, 41, 17)
$Button2 = GUICtrlCreateButton("Open Help", 483, 96, 75, 25)
GUICtrlSetOnEvent(-1, '_OpenHelpTemplate')
$Button8 = GUICtrlCreateButton("Auto Fill Help", 483, 131, 75, 25)
GUICtrlSetOnEvent(-1, '_AutoFillHelp')
$Button9 = GUICtrlCreateButton("Insert Tab", 483, 166, 75, 25)
GUICtrlSetOnEvent(-1, '_InsertTab')
$Button5 = GUICtrlCreateButton("Write Help", 483, 201, 75, 25)
GUICtrlSetOnEvent(-1, '_WriteHelp')
_OpenHelpTemplate()
GUISetState(@SW_SHOW)

While 1
	Sleep(100)
WEnd

Func _OpenHelpTemplate()
	Local $sFilePath, $aTempArray, $sEditContents
	
	$sFilePath = @ScriptDir & "\Templates\HelpTemplate.txt"
	If Not FileExists($sFilePath) Then
		MsgBox(48, "Error", "Missing '.\Templates\HelpTemplate.txt'.")
		Return
	EndIf
	_FileReadToArray($sFilePath, $aTempArray)
	GUICtrlSetData($Edit1, "")
	For $i = 1 To UBound($aTempArray) - 1
		If $i == UBound($aTempArray) - 1 Then
			GUICtrlSetData($Edit1, $aTempArray[$i], 1)
		Else
			GUICtrlSetData($Edit1, $aTempArray[$i] & @CRLF, 1)
		EndIf
	Next
EndFunc   ;==>_OpenHelpTemplate

Func _AutoFillHelp()
	Local $sEditContents, $aFileArray, $sParam, $sParamDesc
	
	$sEditContents = GUICtrlRead($Edit1)
	If $sEditContents == "" Then
		_OpenHelpTemplate()
		$sEditContents = GUICtrlRead($Edit1)
		If $sEditContents = "" Then Return
	EndIf
	
	
	$sEditContents = StringStripCR($sEditContents)
	$aFileArray = StringSplit($sEditContents, @LF)
	GUICtrlSetData($Edit1, "")
	
	For $i = 1 To UBound($aFileArray) - 1
		Switch $aFileArray[$i]
			Case "#Function Name#"
				$aFileArray[$i] = GUICtrlRead($Input1)
			Case "#Description#"
				$aFileArray[$i] = GUICtrlRead($Input2)
			Case "#include#"
				$aFileArray[$i] = "#include <" & $sUDF & ">"
			Case "#Syntax#"
				$aFileArray[$i] = GUICtrlRead($Input8)
			Case "#Parm#"
				$aFileArray[$i] = ""
				For $x = 1 To GUICtrlRead($Input3)
					$sParam = InputBox("", "Parameter: " & $x, "")
					$sParamDesc = InputBox("", "Parameter Description: " & $x, "")
					$aFileArray[$i] &= $sParam & @CRLF & @TAB & $sParamDesc
					If $x <> GUICtrlRead($Input3) Then $aFileArray[$i] &= @CRLF
				Next
			Case "#Success#"
				$aFileArray[$i] = "Success:" & @TAB & GUICtrlRead($Input5)
			Case "#Failure#"
				$aFileArray[$i] = "Failure:" & @TAB & GUICtrlRead($Input6)
			Case "#Remarks#"
				$aFileArray[$i] = GUICtrlRead($Input9)
			Case "#Related#"
				$aFileArray[$i] = GUICtrlRead($Input10)
		EndSwitch
		
		If $i = UBound($aFileArray) - 1 Then
			GUICtrlSetData($Edit1, $aFileArray[$i], 1)
		Else
			GUICtrlSetData($Edit1, $aFileArray[$i] & @CRLF, 1)
		EndIf
	Next
EndFunc   ;==>_AutoFillHelp

Func _InsertTab()
	_GUICtrlEdit_ReplaceSel($Edit1, True, @TAB)
EndFunc   ;==>_InsertTab

Func _WriteHelp()
	Local $sFilePath, $sFileName, $sEditContents, $aFileArray
	
	$sFileName = GUICtrlRead($Input1) & ".txt"
	$sFilePath = @ScriptDir & "\Helpfile Documentation\"
	DirCreate($sFilePath)
	If $sFileName == ".txt" Then
		MsgBox(48, "Error", "You must enter a function name.")
		Return
	EndIf
	_FileCreate($sFilePath & $sFileName)
	If @error Then
		MsgBox(48, "Error", "Unable to create file.")
		Return
	EndIf
	
	$sEditContents = GUICtrlRead($Edit1)
	If $sEditContents == "" Then
		MsgBox(48, "Error", "You must first enter data to write to the file.")
		Return
	EndIf
	
	$sEditContents = StringStripCR($sEditContents)
	$aFileArray = StringSplit($sEditContents, @LF)
	$fSuccess = _FileWriteFromArray($sFilePath & $sFileName, $aFileArray, 1)
	If $fSuccess Then
		MsgBox(64, "Success", "File successfully written.")
	Else
		MsgBox(48, "Error", "There was an error while writing the file.")
	EndIf
EndFunc   ;==>_WriteHelp

Func _Exit()
	Exit
EndFunc   ;==>_Exit