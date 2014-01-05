
/* force MSVC to use WideChar function, must be declared before #include <windows.h> */
#define UNICODE		

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#define IDM_VIEW_STB 4
#define IDM_FILE_DIALOG 5
#define IDM_FILE_COLOR 6
#define IDM_FILE_CONTROL 7


HWND ghSb;

HMENU hMenubar;
HMENU hMenu;
HINSTANCE ghInstance;
HWND ghwndEdit;
HWND hEdit , hLabel;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PanelProc (HWND, UINT, WPARAM, LPARAM);

void CreateDialogBox (HWND);
void RegisterDialogClass (HWND);
void RegisterPanel ();
COLORREF ShowColorDialog (HWND);
void CreateMenubar (HWND);
void OpenDialog (HWND);
void LoadFile (LPCWSTR);
void CreateMyTooltip (HWND);

void AddMenus (HWND);


COLORREF gColor = RGB(255, 255, 255);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    PWSTR pCmdLine, int nCmdShow)
{
  MSG  msg;    
  HWND hwnd;
  WNDCLASSEXW wc;
  
  //memset(&wc, 0, sizeof(wc));
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"Window";
  wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);			// 9: light blue
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
  
  
  if (!RegisterClassExW(&wc) ) {
	  MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
	  return 0;
  }

  hwnd = CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES,
				wc.lpszClassName, L"Title",
                WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                100, 100, 550, 550, 
				NULL, NULL, hInstance, NULL);  
  
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);


  while (GetMessage(&msg, NULL, 0, 0) > 0) {    /* If no error is received... */
	TranslateMessage(&msg);		/* Translate key codes to chars if present */
    DispatchMessage(&msg);		/* Send it to WndProc */
  }

  return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam)
{
	  POINT point;
	  UINT state;
	  static HWND hwndPanel;
	  HWND hwnd_tmp;
  switch(msg)  
  {
	  
   case WM_CREATE:
          AddMenus(hwnd);

		  InitCommonControls();

		  ghSb = CreateStatusWindowW(WS_CHILD | WS_VISIBLE, L"xxx", hwnd, 1);

		  RegisterDialogClass(hwnd);
		  
		  CreateMyTooltip(hwnd);

		  ghwndEdit = CreateWindowExW(WS_EX_RIGHTSCROLLBAR, L"edit", NULL,    
                        WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE,
                        0, 0, 260, 180,        
                        hwnd, (HMENU) 1, NULL, NULL);
		  
		  

          break;

   case WM_COMMAND:
	   
      if (LOWORD(wParam) == 2) {
        Beep(40, 50);
      }
          switch(LOWORD(wParam)) {
              case IDM_FILE_NEW:
				  Beep(50, 100);
				  break;
			  case IDM_FILE_DIALOG:
				  CreateDialogBox(hwnd);
				  break;
			  case IDM_FILE_COLOR:
				  gColor = ShowColorDialog(hwnd);
				  InvalidateRect(hwndPanel, NULL, TRUE);
				  break;
			  case IDM_FILE_CONTROL:
				  hwnd_tmp = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  L"DialogClass", L"Dialog Box", 
								WS_VISIBLE | WS_SYSMENU | WS_CAPTION , 100, 100, 400, 550, 
								NULL, NULL, GetModuleHandle(NULL),  NULL);
				  hLabel = CreateWindowW(L"STATIC", L"STATIC label", WS_CHILD | WS_VISIBLE | SS_LEFT, 
									 20, 90, 300, 230, hwnd_tmp, (HMENU) 1, NULL, NULL);
				  CreateWindowW(L"BUTTON", L"Beep", WS_VISIBLE | WS_CHILD ,
									20, 50, 80, 25, hwnd_tmp, (HMENU) 600, NULL, NULL);
				  CreateWindowW(L"BUTTON", L"Quit Button", WS_VISIBLE | WS_CHILD ,
									120, 50, 80, 25, hwnd_tmp, (HMENU) 650, NULL, NULL);

				  CreateWindowW(L"BUTTON", L"Show Title", WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
									20, 20, 185, 35, hwnd_tmp, (HMENU) 700, NULL, NULL);

				  // EDIT ctrl: max 32,767 bytes
				  hEdit = CreateWindowW(L"EDIT", L"Show Title", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL ,
									20, 110, 185, 20, hwnd_tmp, (HMENU) 800, NULL, NULL);

				  CheckDlgButton(hwnd_tmp, 1, BST_CHECKED);
				  CreateMyTooltip(hwnd_tmp);		// NOT WORKING ?
				  ShowWindow(hwnd_tmp, SW_SHOW);
				  break;
			  case 200:
				  
				  break;
              case IDM_FILE_OPEN:
                  OpenDialog(hwnd);
                  break;
              case IDM_FILE_QUIT:
                  SendMessage(hwnd, WM_CLOSE, 0, 0);
                  break;
			  case IDM_VIEW_STB:
                  state = GetMenuState(hMenu, IDM_VIEW_STB, MF_BYCOMMAND);
                  if (state == SW_SHOWNA) {
                    ShowWindow(ghSb, SW_HIDE);
                    CheckMenuItem(hMenu, IDM_VIEW_STB, MF_UNCHECKED);
                  } else {
                    ShowWindow(ghSb, SW_SHOWNA);
                    CheckMenuItem(hMenu, IDM_VIEW_STB, MF_CHECKED);
                  }
                  
                  break;
           }
           break;
	 case WM_RBUTTONUP:
        point.x = LOWORD(lParam);
        point.y = HIWORD(lParam);
        hMenu = CreatePopupMenu();
        ClientToScreen(hwnd, &point);

        AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
        AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");
                    
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
        break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;      
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}


void AddMenus(HWND hwnd) 
{
	HBITMAP hBitmap;
  hMenubar = CreateMenu();
  hMenu = CreateMenu();
  
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");

  AppendMenuW(hMenu, MF_STRING, IDM_VIEW_STB, L"&Statusbar");
  CheckMenuItem(hMenu, IDM_VIEW_STB, MF_CHECKED);

  AppendMenuW(hMenu, MF_STRING, IDM_FILE_DIALOG, L"&Dialog");
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_COLOR, L"&Color");
  
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_CONTROL, L"&Control");

  AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");
  
  hBitmap = (HBITMAP) LoadImageW( NULL, L"test123.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  SetMenuItemBitmaps(hMenu, IDM_FILE_OPEN, MF_BITMAP | MF_BYCOMMAND, hBitmap, hBitmap);

  
  AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");

  

  SetMenu(hwnd, hMenubar);
}


LRESULT CALLBACK DialogProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT checked;
	WCHAR buf_text[500];
  switch(msg)  
  {
    case WM_CREATE:
	/*CreateWindow(TEXT("button"), TEXT("Ok"),    
	      WS_VISIBLE | WS_CHILD ,
	      50, 50, 80, 25,        
	      hwnd, (HMENU) 2, NULL, NULL);*/
	break;

    case WM_COMMAND:
		

		switch (LOWORD(wParam))
		{
			case 600:
				GetWindowTextW(hEdit, buf_text, 50);
				SetWindowTextW(hLabel, buf_text);
				Beep(40, 50);
				break;
			case 650:
				DestroyWindow(hwnd);
				break;
			case 700:
				checked = IsDlgButtonChecked(hwnd, 700);
				if (checked) {
					CheckDlgButton(hwnd, 700, BST_UNCHECKED);
					SetWindowTextW(hwnd, L"unchecked");
				} else {
					CheckDlgButton(hwnd, 700, BST_CHECKED);
					SetWindowTextW(hwnd, L"checked");
				}
				break;
			case 800:
				SetWindowTextW(hwnd, L"asd");
				break;
		}
		
	break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;       

  }
  return (DefWindowProc(hwnd, msg, wParam, lParam));

}

void RegisterDialogClass(HWND hwnd) 
{
  WNDCLASSEX wc = {0};
  wc.cbSize           = sizeof(WNDCLASSEX);
  wc.lpfnWndProc      = (WNDPROC) DialogProc;
  wc.hInstance        = ghInstance;
  wc.hbrBackground    = GetSysColorBrush(COLOR_3DFACE);
  wc.lpszClassName    = L"DialogClass";
  RegisterClassEx(&wc);

}

void CreateDialogBox(HWND hwnd)
{
  CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  TEXT("DialogClass"), TEXT("Dialog Box"), 
        WS_VISIBLE | WS_SYSMENU | WS_CAPTION , 100, 100, 200, 150, 
        NULL, NULL, ghInstance,  NULL);
}

COLORREF ShowColorDialog(HWND hwnd) {

  CHOOSECOLOR cc;                 
  static COLORREF crCustClr[16];     

  ZeroMemory(&cc, sizeof(cc));
  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwnd;
  cc.lpCustColors = (LPDWORD) crCustClr;
  cc.rgbResult = RGB(0, 255, 0);
  cc.Flags = CC_FULLOPEN | CC_RGBINIT;
  ChooseColor(&cc);

  return cc.rgbResult;
}


void RegisterPanel () {

  WNDCLASS rwc = {0};
  rwc.lpszClassName = TEXT( "Panel" );
  rwc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  rwc.lpfnWndProc   = PanelProc;
  RegisterClass(&rwc);
}

LRESULT CALLBACK PanelProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HDC hdc;
  PAINTSTRUCT ps; 
  RECT rect;

  switch(msg)  
  {
    case WM_PAINT:
    {
        GetClientRect(hwnd, &rect);
        hdc = BeginPaint(hwnd, &ps);
        SetBkColor(hdc, gColor);
        ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, TEXT(""), 0, NULL);
        EndPaint(hwnd, &ps);
        break;
    }
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}


void CreateMenubar(HWND hwnd) {
  HMENU hMenubar;
  HMENU hMenu;

  hMenubar = CreateMenu();
  hMenu = CreateMenu();
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, TEXT("&File"));
  AppendMenu(hMenu, MF_STRING, IDM_FILE_NEW, TEXT("&Open"));
  SetMenu(hwnd, hMenubar);
}

void OpenDialog(HWND hwnd) 
{
  OPENFILENAME ofn;
  TCHAR szFile[MAX_PATH];

 
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFile = szFile;
  ofn.lpstrFile[0] = '\0';
  ofn.hwndOwner = hwnd;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = TEXT("All files(*.*)\0*.*\0");
  ofn.nFilterIndex = 1;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrFileTitle = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  

  if(GetOpenFileName(&ofn))
      LoadFile(ofn.lpstrFile);

  
}

void LoadFile(LPCWSTR file)
{
  HANDLE hFile;
  DWORD dwSize;
  DWORD dw;
  LPBYTE lpBuffer;
  WCHAR ws_buf[500] = {0};

  /* string consisting of several Asian characters */
	wchar_t wcsString[] = L"\u9580\u961c\u9640\u963f\u963b\u9644";
  
  hFile = CreateFileW((LPCWSTR) file, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  dwSize = GetFileSize(hFile, NULL);
  lpBuffer = (LPBYTE) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, dwSize + 1);
  ReadFile(hFile, lpBuffer, dwSize, &dw, NULL);
  CloseHandle(hFile);
  //lpBuffer[dwSize] = 0;

  

  MultiByteToWideChar(CP_UTF8, 0, (LPCCH) lpBuffer, dwSize, ws_buf, dwSize);
  SetWindowTextW(ghwndEdit, (LPWSTR) ws_buf);		// BUG HERE, replace lpBuffer with L"abcd text"
  MessageBoxW(NULL, ws_buf, L"First", MB_OK);
  

  HeapFree(GetProcessHeap(), 0, lpBuffer);
}


void CreateMyTooltip (HWND hwnd)
{

    INITCOMMONCONTROLSEX iccex; 
    HWND hwndTT;                

    TOOLINFO ti;
    WCHAR tooltip[30] = L"A main window";
    RECT rect;                 
  
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
        0, 0, 0, 0, hwnd, NULL, NULL, NULL );

    SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
   
    GetClientRect (hwnd, &rect);

    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hwnd;
    ti.hinst = NULL;
    ti.uId = 0;
    ti.lpszText = tooltip;
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;

    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
}



// MessageBoxW(NULL, L"First Program", L"First", MB_OK);
