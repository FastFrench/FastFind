;JPM;#RequireAdmin
;  AutoIt:  3.0.91 with "smart boolean comparison" and FileChangeDir
;Platform:  Tested on "American" Windows XP sp1
;  Author:  CyberSlug - philipgump@yahoo.com
;    Date:  31 Jan 2004
; Updated:  02 Feb 2004 jpm@autoitscript.com (conditional generation time based)
; Updated:  28 Feb 2004 jpm@autoitscript.com (StandardTable and ParamTable)
;   Added:  18 may 2004 jdeb@autoitscript.com
;           - support for UDF files.
;   Added:  31 Dec 2004 jdeb@autoitscript.com
;           - Commandline option "/RegenAll" to update all in stead of just the changes
;           - Changed the MakeRelatedLinks so it checks all sources.
;             This makes it possible to make references to in functions to keywords
;             and in UDF's to internal functions
;   Added:  3 Jan 2005 jdeb@autoitscript.com
;           - Button to open function examples.
;   Added:  14 jan 2005 jdeb@autoitscript.com
;           - Conversion of AU3 examples to Colored HTM versions by using SciTE.
;   Fixed:  06 apr 2005 jpm@autoitscript.com
;           - suppress mixing source AU3 examples and Colored HTM conversions files
;           - closing of Scite process used to colored examples
;			- colored example for new help page file not generated if Scite already open
;   Added:  18 sep 2005 jpm@autoitscript.com
;           - /AutoIT /UDFs command parameter for selective regeneration
;           - split AutoIt and UDF examples
;   Added:  19 may 2006 jpm@autoitscript.com
;           - ReturnTable valign=top on 1st column
;   Added:  04 december 2006 jpm@autoitscript.com
;           - use generated au3.api and au3.keyword.properties for right coloring
;   Fixed:  09 march 2007 jpm@autoitscript.com
;           - au3.api and au3.keyword.properties not properly copied
;   Changed: 9 may 2007  Jos
;			- Added keyword mainHtmlweb = "Web\" to txt2htm.ini
;			  When this keyword is BLANK the generation of these files is skipped.
;			- Updated All_txt2htm.au3 to generate the Web HTM files.
;			Also:
;			reload the Scite config without the need to close SciTE using the SciTE Director interface.
;			use the SciTE Director interface to Export the Examples to Htm avoid the need to load/use the external program.
;			Minimise SciTE during Export process to avoid the flashing screen and think its a bit faster.
;   Changed: 29 September 2007  Jos
;			Made changes to accommodate the structure definitions.
;   Added:   11 october 2007 jpm
;           - search MSDN : @@IncludeMsdnLink@@
;   Added: @scriptdir & "\txt2htm_error.log" which will contain the generated errors/warnings
;          to allow easy verification and review of the errors.
;==============================================================================
; Generate HTM files (which comprise the help file source docs)
; from specially formatted TXT files.
;==============================================================================

;Script is designed to optionally take command-line arguments.
;If args, they should be the text files you wish to convert to htm.
;  However, the working directory is used as output.
;If no args, script uses data from the 'txt2htm.ini' file....
;  The zero-arg method gives nice splashscreen logging!

#include "include\OutputLib.au3"

Global $ReGen_All = StringInStr($CmdLineRaw, "/RegenAll")
Global $ReGen_AutoIt = StringInStr($CmdLineRaw, "/AutoIt")
Global $ReGen_UDFs = StringInStr($CmdLineRaw, "/UDFs")
If $ReGen_AutoIt = 0 And $ReGen_UDFs = 0 Then
	$ReGen_AutoIt = 1
	$ReGen_UDFs = 1
EndIf

Opt("TrayIconDebug", 1)
Opt("WinTitleMatchMode", 2)

Global $CROSS_DIR
Global $CROSSLINK
Global $INPUT_DIR
Global $OUTPUT_DIR
Global $TEMP_LIST

$TEMP_LIST = "fileList.tmp"

Global $Input  ;array containing each line of the txt-file to convert
Global $hOut   ;file handle to the output file (overwrite mode)
Global $Filename   ;name of file being converted
Global $path   ;name of file being converted

Global $splash ;text of the splash screen
Global $log    ;log of events pasted into Notepad upon completion
Global $SciTEPgm = RegRead("HKLM\Software\Microsoft\Windows\Currentversion\App Paths\Scite.Exe", "")
; Use the generated version of SciTE4AutoIt3 when it exists.
If Not FileExists($SciTEPgm) and FileExists("..\..\..\install\SciTe\SciTE.exe") then $SciTEPgm = "..\..\..\install\SciTe\SciTE.exe"
;
; Get SciTE Director interface Window Handle
Opt("WinSearchChildren", 1)
Opt("WinTitleMatchMode", 4)
; get SciTE handle and when not found start SciTE
$SciTE_hwnd = WinGetHandle("DirectorExtension")
If @error Then
	;Start SciTE when not active
	$SciteClose = 1
	; When not found prompt for the SciTE.exe
	If $SciTEPgm = "" or Not FileExists($SciTEPgm) Then	FileOpenDialog("Couldn't find SciTE.exe... please select it.",@ScriptDir,"SciTE (SciTE.exe)",1)
	If FileExists($SciTEPgm) Then
		Run($SciTEPgm)
		WinWait("Classname=SciTEWindow")
	Else
		MsgBox(262144, "Cannot Find Scite.exe", "Please start SciTE manually and click OK")
		; cannot find SciTE
	EndIf
Else
	$SciteClose = 0
EndIf
; Ensure the handle is know also when Scite got started in this script.
$SciTE_hwnd = WinGetHandle("DirectorExtension")
; clear log
FileDelete(@scriptdir & "\txt2htm_error.log")
;
; Copy the latest SciTE  properties files to your own SciTE installation
SetSciTE_API_files()
; Jos: Reload SciTE properties without closing SciTE first.
SendSciTE_Command(0, $SciTE_hwnd, "reloadproperties:")

; FileDelete($INPUT_DIR & "\totallist.toc")

If $ReGen_All Then
	$rebuildall = 1 ; 0=changed    1 = all
Else
	$rebuildall = 0 ; 0=changed    1 = all
EndIf

;
Global $SciTEState = WinGetState("Classname=SciTEWindow")
WinSetState("Classname=SciTEWindow", "", @SW_MINIMIZE)
$splash = ""
$splash = $INPUT_DIR
_OutputBuildWrite ("txt2htm Conversion" & @CRLF)
;SplashTextOn("txt2htm Conversion", $splash,-1,-1,-1,-1,16)
$log = @CRLF & "*** txt2htm event log***" & @CRLF

If $ReGen_AutoIt Then
	; rebuild all changed TXT files.
	$CROSS_DIR = IniRead("txt2htm.ini", "Input", "libfunctions", "ERR")
	$CROSSLINK = IniRead("txt2htm.ini", "CrossLink", "AutoIt", "ERR")
	$INPUT_DIR = IniRead("txt2htm.ini", "Input", "functions", "ERR")
	$OUTPUT_DIR = IniRead("txt2htm.ini", "Output", "functions", "ERR")
	If Not FileExists($OUTPUT_DIR) Then DirCreate($OUTPUT_DIR)
	$Example_DIR = IniRead("txt2htm.ini", "Input", "Examples", "ERR")
	$ans = 1
	SplashUpdate("Scanning Functions...")
	Rebuild()
	$INPUT_DIR = IniRead("txt2htm.ini", "Input", "keywords", "ERR")
	$OUTPUT_DIR = IniRead("txt2htm.ini", "Output", "keywords", "ERR")
	If Not FileExists($OUTPUT_DIR) Then DirCreate($OUTPUT_DIR)
	$ans = 2
	SplashUpdate("Scanning Keywords...")
	Rebuild()
EndIf

If $ReGen_UDFs Then
	$CROSS_DIR = IniRead("txt2htm.ini", "Input", "functions", "ERR")
	$CROSSLINK = IniRead("txt2htm.ini", "CrossLink", "UDFs", "ERR")
	$INPUT_DIR = IniRead("txt2htm.ini", "Input", "libfunctions", "ERR")
	$OUTPUT_DIR = IniRead("txt2htm.ini", "Output", "libfunctions", "ERR")
	If Not FileExists($OUTPUT_DIR) Then DirCreate($OUTPUT_DIR)
	$Example_DIR = IniRead("txt2htm.ini", "Input", "LibExamples", "ERR")
	$ans = 3
	SplashUpdate("Scanning UDF's...")
	Rebuild()
EndIf
; Close SciTE again when it wasn't running initially
If $SciteClose = 1 Then
	SendSciTE_Command(0, $SciTE_hwnd, "quit:")
Else
	WinSetState("Classname=SciTEWindow", "", $SciTEState)
EndIf
RestoreSciTE_API_files()

Exit

Func Rebuild()
	;$rebuildall = MsgBox(4096 + 4, "", "Starting" & @LF & "Rebuild All= YES, Updated files only=NO")
	;$rebuildall = 7 - $rebuildall; 7 = NO 6 = YES
	;FileChangeDir($INPUT_DIR)
	;pipe the list of sorted file names to fileList.tmp:
	_RunCmd("dir " & $INPUT_DIR & "*.txt /b | SORT > " & $TEMP_LIST)
	;FileChangeDir(@ScriptDir)
	Local $hFileList
	$hFileList = FileOpen($TEMP_LIST, 0)  ;read mode
	If $hFileList = -1 Then
		MsgBox(4096, "Error", $TEMP_LIST & " could not be opened and/or found.")
		Exit
	EndIf

	While 1  ;loop thru each filename contained in fileList.tmp
		$Filename = FileReadLine($hFileList)
		If @error = -1 Then ExitLoop  ;EOF reached
		$path = $INPUT_DIR & $Filename
		If $Filename = "CVS"  Then ContinueLoop   ; Skip CVS
		If $Filename = "Changelog.txt"  Then ContinueLoop   ; Skip ChangeLog.txt
		If StringStripWS($path, 3) = "" Then ExitLoop

		If Not FileExists($path) Then
			SplashUpdate($path & " WAS not found; skipping it.")
			FileWriteLine(@scriptdir & "\txt2htm_error.log",$path & " WAS not found; skipping it.")
			ContinueLoop
		EndIf
		If Not FileToArray($path, $Input) Then
			SplashUpdate($path & " was not found; skipping it.")
			FileWriteLine(@scriptdir & "\txt2htm_error.log",$path & " was not found; skipping it.")
			ContinueLoop
		EndIf
		; Don't rebuild if the 2 target files are up-todate and WebFiles don't need to be build
		If $rebuildall = 0 _
				And isGreaterFileTime($INPUT_DIR, $Filename, $OUTPUT_DIR, "htm") _
				And isGreaterFileTime($Example_DIR, StringTrimRight($Filename, 3) & "au3", $OUTPUT_DIR, "htm") Then ContinueLoop
		; Don't rebuild if the 4 target files are up-todate and WebFiles needs to be build
		If $rebuildall = 0 _
				And isGreaterFileTime($INPUT_DIR, $Filename, $OUTPUT_DIR, "htm") _
				And isGreaterFileTime($Example_DIR, StringTrimRight($Filename, 3) & "au3", $OUTPUT_DIR, "htm") _
				Then ContinueLoop

		;FileChangeDir($OUTPUT_DIR)
		$hOut = FileOpen($OUTPUT_DIR & StringTrimRight($Filename, 3) & "htm", 2)
		;FileChangeDir($INPUT_DIR)

		Convert()
		FileClose($hOut)
	WEnd
	FileClose($hFileList)
EndFunc   ;==>Rebuild

;------------------------------------------------------------------------------
; The main conversion function
;------------------------------------------------------------------------------
Func Convert()

	$RefType = StringStripWS(get(""), 3)

	If $RefType <> "###Function###"  And $RefType <> "###Keyword###"  And $RefType <> "###User Defined Function###"  And $RefType <> "###Structure Name###"  Then
		SplashUpdate($path & " has invalid first line; skipping file.")
		SplashUpdate('x' & $RefType & 'x')
		FileWriteLine(@scriptdir & "\txt2htm_error.log",$path & " has invalid first line; skipping file.")
		FileWriteLine(@scriptdir & "\txt2htm_error.log",'x' & $RefType & 'x')
		Return "Error"
	Else
		SplashUpdate($path)
	EndIf

	$Name = StringReplace(get($RefType), '<br>', '') ;name of the function or keyword

	put('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">')
	put('<html>')
	put('<head>')
	If StringInStr($RefType, "Function") > 0 Then
		put('  <title>Function ' & $Name & '</title>')
	Else
		put('  <title>Keyword ' & $Name & '</title>')
	EndIf
	put('  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">')
	put('  <link href="../css/default.css" rel="stylesheet" type="text/css">')
	put('</head>')
	put('')
	put('<body>')

	; Insert the experimental section if required.
	If get("###Experimental###") Then
		put('<div class="experimental">Warning: This feature is experimental.  It may not work, may contain bugs or may be changed or removed without notice.<br/><br/>DO NOT REPORT BUGS OR REQUEST NEW FEATURES FOR THIS FEATURE.<br/><br/>USE AT YOUR OWN RISK.</div>')
	EndIf

	If StringInStr($RefType, "Function") > 0 Then
		put('<h1>Function Reference</h1>')
	Else
		put('<h1>Keyword Reference</h1>')
	EndIf
	put('<font size="+1">' & $Name & '</font>')
	put('<hr size="2">')
	put('<p>' & get("###Description###") & '</p>')
	put('')

	;	put('<table cellSpacing="5" width="100%" bgColor="#FFFFAA">')
	;	put('<tr>')
	;	put('  <td height="48" class="code">' & get("###Syntax###") & '</td>')
	;	put('</tr>')
	;	put('</table>')
	put('<p class="codeheader">')
	put(get("###Syntax###"))
	put('</p>')

	put('<p>&nbsp;</p>')
	put('')
	put('<p><b>Parameters</b></p>')
	put('' & get("###Parameters###") & '')
	put('' & get("###Fields###") & '')
	put('<p>&nbsp;</p>')
	put('')
	If StringInStr($RefType, "Function") > 0 Then
		put('<p><b>Return Value</b></p>')
		put('' & get("###ReturnValue###") & '')
		put('<p>&nbsp;</p>')
		put('')
	EndIf
	put('<p><b>Remarks</b></p>')
	put('' & get("###Remarks###") & '')
	put('<p>&nbsp;</p>')
	put('')
	put('<p><b>Related</b></p>')
	put('' & get("###Related###") & '')
	put('<p>&nbsp;</p>')
	put('')
	; only add "See also" section if requested
	Local $seealso = get("###See Also###")
	If StringStripWS($seealso,3) <> "" Then
		put('<p><b>See Also</b></p>')
		put('' & $seealso & '')
		put('<p>&nbsp;</p>')
EndIf
	; only add example box when example file is available
	Local $example = get("###Example###")
	If StringStripWS($example,3) <> "" Then
		put('<p><b>Example</b></p>')
		put('<p class="codebox">')
		put('' & $example & '')
		put('<p>&nbsp;</p>')
		put('')
	EndIf
	;	If StringInStr($Name, "Random") Then  ;Random.htm has a special section
	;		put('' & get("###Special###") & '')
	;		put('')
	;	EndIf
	put('</body>')
	put('</html>')

; 	If $ans = 3 Then FileWriteLine($INPUT_DIR & "\totallist.toc", $Name)

EndFunc   ;==>Convert



;------------------------------------------------------------------------------
; Write a new line to the output file
;------------------------------------------------------------------------------
; Write to both the helpfile version and the Web Version
Func put($line)
	FileWriteLine($hOut, $line)
EndFunc   ;==>put

;------------------------------------------------------------------------------
; Retrieve text from $Input to put into html file
;------------------------------------------------------------------------------
Func get($section)

	If $section = "" Then Return $Input[1]  ;very first line

	$i = 0
	$uboundinput = UBound($Input)
	While $i + 1 < $uboundinput
		$i = $i + 1
		If StringInStr($Input[$i], $section) Then ExitLoop
	WEnd
	If $i + 1 < $uboundinput Then $i = $i + 1  ;$i is now index of first line after the section heading

	Select
		Case $section = "###Experimental###"
			; This is an extremely ugly hack to work-around the surrounding
			; crappy code that makes lots of absurd assumptions about the format
			; of the file.  What this does is test if the ###Experimental###
			; section exists.  It returns a boolean to indicate if the section
			; is present.  It determines the section is present by looking
			; at the counter and testing if it is larger than the input size
			; implying that it wasn't found.
			Return $i + 1 < $uboundinput
		Case $section = "###Function###"  Or $section = "###Keyword###"
			Return $Input[$i]
		Case $section = "###Related###"
			Return makeRelatedLinks($i)
		Case $section = "###Example###"
			Return makeExample($i)
		Case $section = "###Special###"  ;Random.htm has a special section
			$tmp = '<p>' & @LF
			For $k = $i To $uboundinput - 1
				$tmp = $tmp & $Input[$k] & '<br>' & @LF
			Next
			Return $tmp & '</p>'
	EndSelect

	$tmp = ""
	While $i < $uboundinput - 1
		If StringInStr($Input[$i], "###") Then ExitLoop
		; makes sure not to go beyond own section
		If StringInStr($Input[$i], "@@ParamTable@@") Then
			$tmp = $tmp & makeParamTable($i)
		ElseIf StringInStr($Input[$i], "@@ControlCommandTable@@") Then
			$tmp = $tmp & makeControlCommandTable($i)
		ElseIf StringInStr($Input[$i], "@@StandardTable@@") Then
			$tmp = $tmp & makeStandardTable($i)
		ElseIf StringInStr($Input[$i], "@@StandardTable1@@") Then
			$tmp = $tmp & makeStandardTable1($i)
		ElseIf StringInStr($Input[$i], "@@ReturnTable@@") Then
			$tmp = $tmp & makeReturnTable($i)
		ElseIf StringInStr($Input[$i], "@@MsdnLink@@") Then
			$tmp = $tmp & makeMsdnLink($i)
		Else
			; will ignore blank lines...
			; but in Remarks section, allow non-consecutive blank lines...
			If StringStripWS($Input[$i], 3) <> "" Or $section = "###Function###"  Then
				;In case of a include <...> Also translate < and >
				If StringInStr($Input[$i], "#include <") Then
					$Input[$i] = StringReplace($Input[$i], "<", "&lt;")
					$Input[$i] = StringReplace($Input[$i], ">", "&gt;")
				EndIf
				$tmp = $tmp & spaceToNBSP($Input[$i]) & '<br>' & @LF
			Else
				If $section = "###Remarks###"  And $i + 1 < $uboundinput - 1 Then
					If StringStripWS($Input[$i + 1], 3) <> "" Then $tmp = $tmp & '<br>' & @LF
				EndIf
			EndIf
		EndIf
		$i = $i + 1
	WEnd

	Return StringStripWS($tmp, 2)  ;remove trailing whitespace

EndFunc   ;==>get



;------------------------------------------------------------------------------
; Makes the names of related functions into links
;------------------------------------------------------------------------------
Func makeRelatedLinks($index)

	;links already containing '<a href' are pasted as is
	;normal function names and AutoItSetOption '(Option)' are "linkified"
	;assumes that multiple links are comma-separated

	;Handle special cases when no links

	$links = StringSplit(StringReplace($Input[$index], ", ", "|"), "|")
	$errFlag = @error
	If $errFlag And StringInStr($Input[$index], "none") Then
		Return "None."
	ElseIf $errFlag And StringInStr($Input[$index], "many") Then
		Return "Many!"  ;special instance in AutoItSetOption
	ElseIf $errFlag Then
		;StringSplit did not create array since only one link (no comma)
		Dim $links[2]
		$links[1] = $Input[$index]
	EndIf

	;Create links

	$tmp = ""
	For $i = 1 To UBound($links) - 1
		If StringStripWS($links[$i], 1) = "" Then ContinueLoop
		If StringInStr($links[$i], "<a href") Then
			$tmp = $tmp & $links[$i] & ", "
		ElseIf StringInStr($links[$i], " (Option)") Then
			$tag = StringReplace($links[$i], " (Option)", "")
			$tmp = $tmp & '<a href="AutoItSetOption.htm#' & $tag & '">' & $links[$i] & '</a>, '
		Else
			$htmName = $links[$i]
			;  gui links to the summary pages
			If StringInStr($links[$i], "...") Then
				$htmName = StringReplace($links[$i], "...", " Management")
			EndIf

			; links cross .chm
			If StringLeft($links[$i], 1) = "."  Then
				$htmName = StringReplace($links[$i], ".", "")
			EndIf

			If StringInstr($htmName, " Management")= 0 Then
			; if the targetfile doesn't exist in the target htm dir then look in the other dirs.
			If Not FileExists($OUTPUT_DIR & $htmName & ".htm") And Not FileExists($INPUT_DIR & $htmName & ".txt") Then
				If Not FileExists($CROSS_DIR & $htmName & ".txt") Then
					$tempdir_In = IniRead("txt2htm.ini", "Input", "functions", "ERR")
					$tempdir_Out = IniRead("txt2htm.ini", "Output", "functions", "ERR")
					If Not FileExists($tempdir_Out & $htmName & ".htm") And Not FileExists($tempdir_In & $htmName & ".txt") Then
						$tempdir_In = IniRead("txt2htm.ini", "Input", "keywords", "ERR")
						$tempdir_Out = IniRead("txt2htm.ini", "Output", "keywords", "ERR")
						If Not FileExists($tempdir_Out & $htmName & ".htm") And Not FileExists($tempdir_In & $htmName & ".txt") Then
							$tempdir_In = IniRead("txt2htm.ini", "Input", "libfunctions", "ERR")
							$tempdir_Out = IniRead("txt2htm.ini", "Output", "libfunctions", "ERR")
							If Not FileExists($tempdir_Out & $htmName & ".htm") And Not FileExists($tempdir_In & $htmName & ".txt") Then
								SplashUpdate('** Error in ' & $Filename & ' => Invalid Function reference to:' & $htmName)
								FileWriteLine(@scriptdir & "\txt2htm_error.log",'** Error in ' & $Filename & ' => Invalid Function reference to:' & $htmName)
								;ContinueLoop
							EndIf
						EndIf
					EndIf
					$htmName = "../../" & $tempdir_Out & $htmName
				Else
					$links[$i] = $htmName
					$htmName = $CROSSLINK & $htmName
				EndIf
			EndIf
			EndIf
			$tmp = $tmp & '<a href="' & $htmName & '.htm">' & $links[$i] & '</a>, '
		EndIf
	Next

	Return StringTrimRight($tmp, 2)  ;remove trailing comma and space

EndFunc   ;==>makeRelatedLinks

;------------------------------------------------------------------------------
; Makes the a link to MSDN
; 11 Oct 2007 @@MsdnLink@@ search_string
;------------------------------------------------------------------------------
Func makeMsdnLink($index)
	Local $name = StringSplit($Input[$index], " ")
	$name = $name[$name[0]]
	; It can be better to launch an explorer web page
	; seems pretty long to stay in .chm to browse pages
	; an expert is needed for doing someting similar to the button "Open this script"
	Return 'Search <a href="http://search.msdn.microsoft.com/search/Default.aspx?brand=msdn&query=' & _
		$name & '">' & _
		$name & '</a> in MSDN  Library' & @CRLF
EndFunc   ;==>makeMsdnLink

;------------------------------------------------------------------------------
; Formats a code example (converts tabs and linefeeds to html)
; 31 Jan version also converts groups of 2 or more spaces to &nbsp;...
; 20 Sep version also converts < to &lt and > to &gt
; 14 Jan 2005 added the Button logic and commented the < > because we use HTML Color files now
; 18 Sep 2005 separation of install dir lib examples from autoit examples
;------------------------------------------------------------------------------
Func makeExample(ByRef $i)

	Local $nbspified
	$tmp = ""
	$uboundinput = UBound($Input)
	While $i < $uboundinput
		If StringInStr($Input[$i], "###Special###") Then ExitLoop
		If StringInStr($Input[$i], "@@IncludeExample@@") Then
			$i = -1
			ExitLoop
		EndIf
		$nbspified = StringReplace($Input[$i], "  ", "&nbsp;&nbsp;")
		;$nbspified = StringReplace($nbspified, "<", "&lt;")
		;$nbspified = StringReplace($nbspified, ">", "&gt;")
		$tmp = $tmp & _
				StringReplace($nbspified, @TAB, '&nbsp;&nbsp;&nbsp; ') & '<br>' & @LF
		$i = $i + 1
	WEnd

	If $i = -1 Then
		Local $example = includeExample($i)
		If $example <> "" Then
			$example = $example & @CRLF & _
					'<script type="text/javascript">' & @CRLF & _
					'if (document.URL.match(/^mk:@MSITStore:/i))' & @CRLF & _
					'document.write(''<br><OBJECT id=hhctrl type="application/x-oleobject" classid="clsid:adb880a6-d8ff-11cf-9377-00aa003b7a11" width=58 height=57><PARAM name="Command" value="ShortCut"><PARAM name="Button" value="Text:Open this Script"><PARAM name="Item1" value=",Examples\\HelpFile\\' & StringReplace($Filename, ".txt", ".au3") & ',"></OBJECT>'');' & @CRLF & _
					'</script>'
			$example = $example & @CRLF
		EndIf
		Return $example
	EndIf

	Return StringTrimRight($tmp, 6)  ;remove very last <br> @LF stuff

EndFunc   ;==>makeExample

;------------------------------------------------------------------------------
; include file as an example
; 14 Jan 2005 added the au3 to htm conversion to Color the examples
; 06 Apr 2005 suppress mixing source AU3 examples and Colored HTM conversions files
; 18 Sep 2005 separation of input lib examples
;------------------------------------------------------------------------------
Func includeExample(ByRef $i)
	$Example_DIR = IniRead("txt2htm.ini", "Input", "Examples", "ERR")
	If $ans = 3 Then $Example_DIR = IniRead("txt2htm.ini", "Input", "LibExamples", "ERR")
	$Example_DIR = @ScriptDir & "\" & $Example_DIR
	$Example_DIRO = IniRead("txt2htm.ini", "Output", "Examples", "ERR")
	DirCreate($Example_DIRO)
	$tFilename = StringReplace($Example_DIR & $Filename, ".txt", ".au3")
	; Added to convert the AU3 file to colored HTM by running ConvAU3ToHtm.exe.
	; This program sends the commands to SciTE DIrector interface to Open the file, save as html and close.
	If FileExists($tFilename) Then
		$oFileName = @ScriptDir & "\" & $Example_DIRO & $Filename & ".htm"
		;RunWait(@ScriptDir & "\ConvAU3ToHtm.exe " & $tFilename, @ScriptDir)
		SendSciTE_Command(0, $SciTE_hwnd, 'open:' & StringReplace($tFilename, "\", "\\") & '')
		SendSciTE_Command(0, $SciTE_hwnd, 'exportashtml:' & StringReplace($oFileName, "\", "\\"))
		SendSciTE_Command(0, $SciTE_hwnd, 'close:')
		If FileExists($oFileName) Then
			$tFilename = $oFileName
			; Read the generated file and strip the header and footer and replace some stuff to make it look better
			Local $TotalFile = FileRead($tFilename, FileGetSize($tFilename))
			Local $OutRec = StringTrimLeft($TotalFile, StringInStr($TotalFile, '<body bgcolor="#') + 24)
			$OutRec = StringLeft($OutRec, StringInStr($OutRec, '</body>') - 1)
			$OutRec = StringReplace($OutRec, "<br />", "")
			FileDelete($tFilename)
			FileWrite($tFilename, $OutRec)
		EndIf
	EndIf
	;
	If Not FileToArray($tFilename, $Input) Then
		SplashUpdate("** Warning in:" & $Filename & " ==> Include Example file was not found; skipping it:" & StringReplace($Filename, ".txt", ".au3"))
		FileWriteLine(@scriptdir & "\txt2htm_error.log","** Warning in:" & $Filename & " ==> Include Example file was not found; skipping it:" & StringReplace($Filename, ".txt", ".au3"))
		;MsgBox(4096, 'debug:', '$path & " was not found; skipping it.' & $tFilename & " was not found; skipping it.") ;### Debug MSGBOX
		;ConsoleWrite('**** Include Example file:' & $tFilename & " was not found; skipping it." & @LF) ;### Debug MSGBOX
		Return ""
	Else
		$i = 1
		Return makeExample($i)
	EndIf

EndFunc   ;==>includeExample

;------------------------------------------------------------------------------
; Set au3.api and au3.keyword.properties to be used by ConvAu3ToHtm
; to generated the right colored examples.
; They are built by "AutoIt Extractor.au3"
;------------------------------------------------------------------------------
Func SetSciTE_API_files()
	Local $AutoItDir = RegRead("HKLM\SOFTWARE\AutoIt v3\AutoIt", "InstallDir")
	If DirCreate($AutoItDir & "\SciTE\Defs\Rest") Then
		FileCopy($AutoItDir & "\SciTE\au3.keywords.properties", $AutoItDir & "\SciTE\Defs\Rest\*.*")
	EndIf

	$AU3PROP = IniRead("txt2htm.ini", "Input", "sciteprop", "ERR")
	FileCopy($AU3PROP & "au3.keywords.properties", $AutoItDir & "\SciTE\*.*", 9)
EndFunc   ;==>SetSciTE_API_files

;------------------------------------------------------------------------------
; Restore au3.api and au3.keyword.properties to be current version
;------------------------------------------------------------------------------
Func RestoreSciTE_API_files()
	Local $AutoItDir = RegRead("HKLM\SOFTWARE\AutoIt v3\AutoIt", "InstallDir")
	FileCopy($AutoItDir & "\SciTE\Defs\Rest\au3.keywords.properties", $AutoItDir & "\SciTE\*.*", 1)
	DirRemove($AutoItDir & "\SciTE\Defs\Rest", 1)
EndFunc   ;==>RestoreSciTE_API_files

;------------------------------------------------------------------------------
; Convert tab-delimited text to an html table
;------------------------------------------------------------------------------
Func makeStandardTable(ByRef $index)
	;Assumes one line per row with tab-separated columns
	$index = $index + 1

	$tbl = @LF  ;will contain final html table code

	While Not StringInStr($Input[$index], "@@End@@")
		;StringSplit each row into an array for easy parsing
		$x = StringSplit($Input[$index], @TAB)
		If @error Then
			$index = $index + 1
			ContinueLoop
		EndIf

		$tbl = $tbl & '  <tr>' & @LF
		For $i = 1 To $x[0]
			$tbl = $tbl & '    <td>' & $x[$i] & '</td>' & @LF
		Next
		$tbl = $tbl & '  </tr>' & @LF

		$index = $index + 1
	WEnd

	Return '<table width="100%" border="1">' & $tbl & '</table>'

EndFunc   ;==>makeStandardTable

;------------------------------------------------------------------------------
; Convert tab-delimited text to an html table
; 28 Feb version change table look to have a separating line after first row only
;------------------------------------------------------------------------------
Func makeStandardTable1(ByRef $index)
	;Assumes one line per row with tab-separated columns
	$index = $index + 1

	$tbl = @LF  ;will contain final html table code
	; to generate a separation line after the first row
	$td = '    <td style="border-bottom-style: solid; border-bottom-width: 1">'
	While Not StringInStr($Input[$index], "@@End@@")
		;StringSplit each row into an array for easy parsing
		$x = StringSplit($Input[$index], @TAB)
		If @error Then
			$index = $index + 1
			ContinueLoop
		EndIf

		$tbl = $tbl & '  <tr>' & @LF
		For $i = 1 To $x[0]
			$tbl = $tbl & $td & $x[$i] & '</td>' & @LF
		Next
		$tbl = $tbl & '  </tr>' & @LF
		;reset separation line for next lines
		$td = '    <td>'
		$index = $index + 1
	WEnd

	Return '<table width="100%" border="1" cellspacing="0" cellpadding="3" bordercolor="#C0C0C0">' & $tbl & '</table>'

EndFunc   ;==>makeStandardTable1

;------------------------------------------------------------------------------
; Convert tab-delimited text to an html table
;    Each indentend line indicates contents of column 2 for that row.
; Column 1 has 10% width and Column 2 has 90% width
;------------------------------------------------------------------------------
Func makeReturnTable(ByRef $index)
	;Assumes one line per row with tab-separated columns
	$index = $index + 1

	$tbl = @LF  ;will contain final html table code
	$r = 1
	While Not StringInStr($Input[$index], "@@End@@")
		;StringSplit each row into an array for easy parsing
		$x = StringSplit($Input[$index], @TAB)
		If @error Then
			$index = $index + 1
			ContinueLoop
		EndIf

		$tbl = $tbl & '  <tr>' & @LF
		For $i = 1 To $x[0]
			If $r = 1 Then
				If $i = 1 Then
					$tbl = $tbl & '    <td width="10%" valign="top">' & $x[$i] & '</td>' & @LF
				Else
					$tbl = $tbl & '    <td width="90%">' & $x[$i] & '</td>' & @LF
				EndIf
			Else
				If $i = 1 Then
					$tbl = $tbl & '    <td valign="top">' & $x[$i] & '</td>' & @LF
				Else
					$tbl = $tbl & '    <td>' & $x[$i] & '</td>' & @LF
				EndIf
			EndIf
		Next
		$tbl = $tbl & '  </tr>' & @LF
		$r = 0
		$index = $index + 1
	WEnd

	Return '<table width="100%">' & $tbl & '</table>'

EndFunc   ;==>makeReturnTable



;------------------------------------------------------------------------------
; Convert special tab delimited text to a two-colum table.
;    Each non-indented line indicates a new row.
;    Each indentend line indicates contents of column 2 for that row.
; Column 1 has 15% width and Column 2 has 85% width
;------------------------------------------------------------------------------
Func makeParamTable(ByRef $index)


	;make a pass to determine how many rows and columns the table has
	$rows = 0
	$i = $index + 1
	While Not StringInStr($Input[$i], "@@End@@")
		$indention = stringCount($Input[$i], @TAB)
		If $indention = 0 Then $rows = $rows + 1
		$i = $i + 1
	WEnd
	$start = $index + 1
	$stop = $i - 1

	$index = $stop + 1  ;note that $index was ByRef'ed

	Dim $matrix[$rows][2]

	$data = ""
	$r = -1
	$c = 0
	For $i = $start To $stop
		$indention = stringCount($Input[$i], @TAB)
		$data = spaceToNBSP(StringTrimLeft($Input[$i], $indention))

		; Ensure [optional] is bold.
		If StringInStr($Input[$i], "[optional]") Then $data = StringReplace($Input[$i], "[optional]", "<b>[optional]</b>")

		If $indention = 0 Then
			$r = $r + 1
			$c = 0
		Else
			$c = 1
		EndIf

		If $matrix[$r][$c] <> "" Then
			$matrix[$r][$c] = $matrix[$r][$c] & '<br>' & $data
		Else
			$matrix[$r][$c] = $data
		EndIf
	Next

	$tbl = '<table border="1" width="100%" cellspacing="0" cellpadding="3" bordercolor="#C0C0C0">' & @LF

	For $r = 0 To $rows - 1

		$tbl = $tbl & '  <tr>' & @LF

		For $c = 0 To 1
			$data = StringReplace($matrix[$r][$c], "<br>", "<br>" & @LF & "       ")
			Select
				Case $r = 0 And $c = 0  ;only put width%-info on first row
					$tbl = $tbl & '    <td width="15%">' & $data & '</td>' & @LF
				Case $r = 0 And $c = 1
					$tbl = $tbl & '    <td width="85%">' & $data & '</td>' & @LF
				Case Else
					$tbl = $tbl & '   <td>' & $data & '</td>' & @LF
			EndSelect
		Next
		$tbl = $tbl & '  </tr>' & @LF
	Next

	Return $tbl & '</table>'

EndFunc   ;==>makeParamTable



;------------------------------------------------------------------------------
; Convert special tab delimited text to a two-colum table.
;    Each non-indented line indicates a new row.
;    Each indentend line indicates contents of column 2 for that row.
; Column 1 has 40% width and Column 2 has 60% width
;------------------------------------------------------------------------------
Func makeControlCommandTable(ByRef $index)


	;make a pass to determine how many rows and columns the table has
	$rows = 0
	$i = $index + 1
	While Not StringInStr($Input[$i], "@@End@@")
		$indention = stringCount($Input[$i], @TAB)
		If $indention = 0 Then $rows = $rows + 1
		$i = $i + 1
	WEnd
	$start = $index + 1
	$stop = $i - 1

	$index = $stop + 1  ;note that $index was ByRef'ed

	Dim $matrix[$rows][2]

	$data = ""
	$r = -1
	$c = 0
	For $i = $start To $stop
		$indention = stringCount($Input[$i], @TAB)
		$data = spaceToNBSP(StringTrimLeft($Input[$i], $indention))
		If $indention = 0 Then
			$r = $r + 1
			$c = 0
		Else
			$c = 1
		EndIf
		If $matrix[$r][$c] <> "" Then
			$matrix[$r][$c] = $matrix[$r][$c] & '<br>' & $data
		Else
			$matrix[$r][$c] = $data
		EndIf
	Next

	$tbl = '<table border="1" width="100%" cellspacing="0" cellpadding="3" bordercolor="#C0C0C0">' & @LF

	For $r = 0 To $rows - 1

		$tbl = $tbl & '  <tr>' & @LF

		For $c = 0 To 1
			$data = StringReplace($matrix[$r][$c], "<br>", "<br>" & @LF & "       ")
			Select
				Case $r = 0 And $c = 0  ;only put width%-info on first row
					$tbl = $tbl & '    <td width="40%">' & $data & '</td>' & @LF
				Case $r = 0 And $c = 1
					$tbl = $tbl & '    <td width="60%">' & $data & '</td>' & @LF
				Case Else
					$tbl = $tbl & '   <td>' & $data & '</td>' & @LF
			EndSelect
		Next
		$tbl = $tbl & '  </tr>' & @LF
	Next

	Return $tbl & '</table>'

EndFunc   ;==>makeControlCommandTable


; --------------------------------------------------------------------
; Count the occcurrences of a single character in a string
; --------------------------------------------------------------------
Func stringCount($str, $char)
	$t = StringSplit($str, $char)
	If @error Then Return 0
	Return $t[0] - 1
EndFunc   ;==>stringCount



; --------------------------------------------------------------------
; Convert all but one leading space to the &nbsp; html code
; --------------------------------------------------------------------
Func spaceToNBSP($str)
	For $i = 1 To StringLen($str)
		If StringMid($str, $i, 1) <> " "  Then ExitLoop
	Next
	If $i - 1 > 0 Then $str = StringReplace($str, " ", "&nbsp;", $i - 1)

	;Also convert each leading tab to 4 spaces ("&nbsp;&nbsp;&nbsp; ")
	For $i = 1 To StringLen($str)
		If StringMid($str, $i, 1) <> @TAB Then ExitLoop
	Next
	If $i - 1 > 0 Then
		$str = StringReplace($str, @TAB, "&nbsp;&nbsp;&nbsp; ", $i - 1)
	EndIf

	Return $str
EndFunc   ;==>spaceToNBSP



;------------------------------------------------------------------------------
; Based on Jon's post http://www.hiddensoft.com/forum/index.php?showtopic=530
;------------------------------------------------------------------------------
Func FileToArray($sFile, ByRef $array)

	Local $hFile
	Local $sText

	$hFile = FileOpen($sFile, 0)
	If $hFile = -1 Then Return 0  ;failure

	$sText = ""
	While 1
		$sText = $sText & FileReadLine($hFile) & @LF
		If @error <> 0 Then ExitLoop
	WEnd

	FileClose($hFile)

	$sText = StringTrimRight($sText, 1)  ;remove final @LF
	$array = StringSplit($sText, @LF)

	Return 1  ;success

EndFunc   ;==>FileToArray



;------------------------------------------------------------------------------
; Run DOS/console commands
;------------------------------------------------------------------------------
Func _RunCmd($command)
	FileWriteLine("brun.bat", $command & " > " & $TEMP_LIST)
	RunWait(@ComSpec & " /c brun.bat", "", @SW_HIDE)
	FileDelete("brun.bat")
	Return
EndFunc   ;==>_RunCmd



;------------------------------------------------------------------------------
; Update the splash screen in a flicker-free way
;------------------------------------------------------------------------------
Func SplashUpdate($message)
	_OutputBuildWrite ($message & @CRLF)
	Return

	$log = $log & @CRLF & $message

	;splash screen can only hold about 20 lines at a time
	If stringCount($splash, @LF) >= 20 Then $splash = ""
	$splash = $splash & $message & @LF
	ControlSetText("txt2htm Conversion", "", "Static1", $splash)
EndFunc   ;==>SplashUpdate


;------------------------------------------------------------------------------
; Check when rebuildall not asked if the input is newer than the output
;------------------------------------------------------------------------------
Func isGreaterFileTime($indir, $path, $outdir, $new)
	Local $t
	If Not $rebuildall Then
		$t = FileGetTime($indir & $path)
		If Not IsArray($t) Then
			; skip generation of this file when its not there
			Return 1
			;ConsoleWrite("Unable to retrieve filedate for:" & $indir & $path & @CRLF)
		Else
			$szInTime = $t[0] & $t[1] & $t[2] & $t[3] & $t[4] & $t[5]
			;		$t = FileGetTime($indir & "..\Examples\" & StringTrimRight($path, 3) & "au3")
			;		If @error Then
			;			$szInExampleTime = $szInTime
			;		Else
			;			$szInExampleTime = $t[0] & $t[1] & $t[2] & $t[3] & $t[4] & $t[5]
			;		EndIf
			$t = FileGetTime($outdir & StringTrimRight($path, 3) & $new)
			If @error Then Return 0
			$szOutTime = $t[0] & $t[1] & $t[2] & $t[3] & $t[4] & $t[5]
			;		If $szInTime < $szOutTime And $szInExampleTime < $szOutTime Then Return 1; to skip generation for this file
			If $szInTime < $szOutTime Then Return 1; to skip generation for this file
		EndIf
	EndIf
	Return 0
EndFunc   ;==>isGreaterFileTime
;
; Send commands to Scite's Director interface
Func SendSciTE_Command($My_Hwnd, $SciTE_hwnd, $sCmd)
	Local $WM_COPYDATA = 74
	Local $CmdStruct = DllStructCreate('Char[' & StringLen($sCmd) + 1 & ']')
	;ConsoleWrite('-->' & $sCmd & @lf )
	DllStructSetData($CmdStruct, 1, $sCmd)
	Local $COPYDATA = DllStructCreate('Ptr;DWord;Ptr')
	DllStructSetData($COPYDATA, 1, 1)
	DllStructSetData($COPYDATA, 2, StringLen($sCmd) + 1)
	DllStructSetData($COPYDATA, 3, DllStructGetPtr($CmdStruct))
	DllCall('User32.dll', 'None', 'SendMessage', 'HWnd', $SciTE_hwnd, _
			'Int', $WM_COPYDATA, 'HWnd', $My_Hwnd, _
			'Ptr', DllStructGetPtr($COPYDATA))
EndFunc   ;==>SendSciTE_Command
