LRESULT CALLBACK LowLevelMouseProc(
  __in  int nCode,
  __in  WPARAM wParam,
  __in  LPARAM lParam
);
class CShowPixelsDlg;
void RegisterHook(CShowPixelsDlg *pParent);
void UnRegisterHook();

