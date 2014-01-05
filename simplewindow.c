
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#define IDM_VIEW_STB 4
#define IDM_FILE_DIALOG 5
#define IDM_FILE_COLOR 6


HWND ghSb;

HMENU hMenubar;
HMENU hMenu;
HINSTANCE ghInstance;
HWND ghwndEdit;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PanelProc(HWND, UINT, WPARAM, LPARAM);

void CreateDialogBox(HWND);
void RegisterDialogClass(HWND);
void RegisterPanel(void);
COLORREF ShowColorDialog(HWND);
void CreateMenubar(HWND);
void OpenDialog(HWND);
void LoadFile(LPSTR);

void AddMenus(HWND);


COLORREF gColor = RGB(255, 255, 255);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    PWSTR pCmdLine, int nCmdShow)
{
  MSG  msg;    
  HWND hwnd;
  WNDCLASSW wc;
	
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.lpszClassName = L"Window";
  wc.hInstance     = hInstance;
  wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
  wc.lpszMenuName  = NULL;
  wc.lpfnWndProc   = WndProc;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  
  RegisterClassW(&wc);
  hwnd = CreateWindowW( wc.lpszClassName, L"Window",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                100, 100, 350, 250, NULL, NULL, hInstance, NULL);  

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);


  while( GetMessage(&msg, NULL, 0, 0)) {
    DispatchMessage(&msg);
  }

  return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam)
{
	  POINT point;
	  UINT state;
	  static HWND hwndPanel;
  switch(msg)  
  {
	  
   case WM_CREATE:
          AddMenus(hwnd);

		  InitCommonControls();

		  ghSb = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "xxx", hwnd, 1);

		  RegisterDialogClass(hwnd);


		  ghwndEdit = CreateWindowEx(WS_EX_RIGHTSCROLLBAR, TEXT("edit"), NULL,    
                        WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE,
                        0, 0, 260, 180,        
                        hwnd, (HMENU) 1, NULL, NULL);
		  

          break;

   case WM_COMMAND:
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
  
  AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

  hBitmap = (HBITMAP) LoadImage( NULL, "test123.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  SetMenuItemBitmaps(hMenu, IDM_FILE_OPEN, MF_BITMAP | MF_BYCOMMAND, hBitmap, hBitmap);

  
  AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");

  

  SetMenu(hwnd, hMenubar);
}


LRESULT CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

  switch(msg)  
  {
    case WM_CREATE:
	CreateWindow(TEXT("button"), TEXT("Ok"),    
	      WS_VISIBLE | WS_CHILD ,
	      50, 50, 80, 25,        
	      hwnd, (HMENU) 1, NULL, NULL);  
	break;

    case WM_COMMAND:
	DestroyWindow(hwnd);
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
  wc.lpszClassName    = TEXT("DialogClass");
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


void RegisterPanel(void) {

  WNDCLASS rwc = {0};
  rwc.lpszClassName = TEXT( "Panel" );
  rwc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  rwc.lpfnWndProc   = PanelProc;
  RegisterClass(&rwc);
}

LRESULT CALLBACK PanelProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
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

void LoadFile(LPSTR file)
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


// MessageBoxW(NULL, L"First Program", L"First", MB_OK);
