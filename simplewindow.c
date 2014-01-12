
/* force MSVC to use WideChar function, must be declared before #include <windows.h> */
#define UNICODE


#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>



#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Comctl32.lib")


#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#define IDM_VIEW_STB 4
#define IDM_FILE_DIALOG 5
#define IDM_FILE_COLOR 6
#define IDM_FILE_CONTROL 7


#define ID_BLUE 6001
#define ID_YELLOW 6002
#define ID_ORANGE 6003

WCHAR buf[500];

HFONT defaultFont;
HWND ghSb;
HMENU hMenubar;
HMENU hMenu;
HINSTANCE ghInstance;
HWND ghwndEdit;
HWND hEdit , hLabel, button1, button2, checkbox1;
HFONT hFont, hFont_default;
HBITMAP hBitmap;
NONCLIENTMETRICS ncm;
HWND hTrack;
UINT hTrack_id;
HWND hDebugLabel;
HWND hMonthCal;
COLORREF g_color;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PanelProc (HWND, UINT, WPARAM, LPARAM);

void CreateDialogBox (HWND);
void RegisterDialogClass (HWND);
void RegisterPanel ();
COLORREF ShowColorDialog (HWND);
void CreateMenubar (HWND);
void OpenDialog (HWND);
void LoadFile_internal (LPCWSTR);
void CreateMyTooltip (HWND);
void CreateTrackBar(HWND);
void UpdateTrackBar();
void AddMenus (HWND);


HMODULE hmod;



COLORREF gColor = RGB(255, 255, 255);

int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEXW wc;
	INITCOMMONCONTROLSEX iccex;

	//memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra= 0;
	wc.cbWndExtra= 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName= NULL;
	wc.lpszClassName = L"Window";
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);			// 9: light blue
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassExW(&wc)) {
		MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	iccex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//InitCommonControls();		// obsolete
	InitCommonControlsEx(&iccex);

	hwnd = CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES , wc.lpszClassName, L"Title", 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	//ShowWindow(hwnd, nCmdShow);
	//UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg);		/* Translate key codes to chars if present */
		DispatchMessageW(&msg);		/* Send it to WndProc */
	}

	return (int) msg.wParam;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	POINT point;
	UINT state;
	static HWND hwndPanel;
	HWND hwnd_tmp;
	RECT rectParent;

	switch (msg) {
		case WM_CREATE:
			AddMenus(hwnd);
			
			
			//ghSb = CreateStatusWindowW(WS_CHILD | WS_VISIBLE, L"Status bar title", hwnd, 5003);	// obsolete
			
			// STATUSCLASSNAME or "msctls_statusbar32"
			ghSb = CreateWindowW(L"msctls_statusbar32", L"new Status bar title", 
				WS_VISIBLE | WS_CHILD | WS_BORDER | SBARS_SIZEGRIP | CCS_BOTTOM,
				0, 0, 0, 0, hwnd, (HMENU) 5003, NULL, NULL );
			ShowWindow(ghSb, SW_HIDE);

			RegisterDialogClass(hwnd);
			CreateMyTooltip(hwnd);
			
			// Delphi: Tahoma 13, .NET: Microsoft Sans Serif 14, System Default: Segoe UI
			
			// get custom font
			hFont = CreateFontW(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Comic Sans MS");

			// get system default font
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
			hFont_default = CreateFontIndirect(&ncm.lfMessageFont);
			
			ghwndEdit = CreateWindowW(L"EDIT", L"abcd",
				WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_CLIPCHILDREN,
				0, 0, 260, 180, hwnd, NULL, NULL, NULL);
				
			//SendMessageW(ghwndEdit, WM_SETFONT, (WPARAM) hFont, TRUE);

			/* // RICH EDIT CONTROL
			LoadLibrary(TEXT("Msftedit.dll"));
			ghwndEdit = CreateWindowW(MSFTEDIT_CLASS, NULL,
				WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE |WS_HSCROLL | WS_VSCROLL |WS_BORDER | WS_TABSTOP ,
				0, 0, 260, 180, hwnd, NULL, NULL, NULL);*/

			/*// Scintilla
			hmod = LoadLibraryW(L"SciLexer.DLL");
			if (hmod != NULL) {
				ghwndEdit = CreateWindowW(L"Scintilla", L"abcd",
					WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_CLIPCHILDREN,
					0, 0, 260, 180, hwnd, NULL, NULL, NULL);
			} else {
				MessageBoxW(hwnd, L"The Scintilla DLL could not be loaded.",
					L"Error loading Scintilla", MB_OK | MB_ICONERROR);
			}*/
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
				case IDM_FILE_CONTROL:
					hwnd_tmp = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"DialogClass", 
						L"Dialog Box", WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 
						100, 100, 800, 550, hwnd, NULL, GetModuleHandle(NULL), NULL);
					
					// default Height: Edit 21, Button 25, Static 13 
					
					hLabel = CreateWindowW(L"STATIC", L"This is Label", 
						WS_CHILD | WS_VISIBLE | SS_LEFT, 
						20, 90, 300, 13, hwnd_tmp, (HMENU) 500, NULL, NULL);
					button1 = CreateWindowW(L"BUTTON", L"Button", WS_VISIBLE | WS_CHILD, 
						20, 50, 80, 25, hwnd_tmp, (HMENU) 600, NULL, NULL);
					button2 = CreateWindowW(L"BUTTON", L"&Quit", WS_VISIBLE | WS_CHILD, 
						120, 50, 80, 25, hwnd_tmp, (HMENU) 650, NULL, NULL);
					checkbox1 = CreateWindowW(L"BUTTON", L"This is Checkbox", 
						WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 
						20, 10, 185, 14, hwnd_tmp, (HMENU) 700, NULL, NULL);

					hDebugLabel = CreateWindowW(L"STATIC", L"0", WS_CHILD | WS_VISIBLE, 
						170, 20, 90, 30, hwnd_tmp, (HMENU) 3004, NULL, NULL);

					// EDIT ctrl: max 32,767 bytes
					hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"This is edit", 
						WS_VISIBLE | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL | ES_NOHIDESEL,
						20, 110, 185, 21, hwnd_tmp, (HMENU) 800, NULL, NULL);	// WS_BORDER

					CheckDlgButton(hwnd_tmp, 1, BST_CHECKED);

					// better font
					SendMessageW(hEdit, WM_SETFONT, (WPARAM) hFont, TRUE);
					SendMessageW(hLabel, WM_SETFONT, (WPARAM) hFont, TRUE);
					//SendMessageW(button1, WM_SETFONT, (WPARAM) hFont, TRUE);
					SendMessageW(button2, WM_SETFONT, (WPARAM) hFont, TRUE);
					SendMessageW(checkbox1, WM_SETFONT, (WPARAM) hFont, TRUE);
					
					SendMessageW(hEdit, WM_SETFONT, (WPARAM) hFont_default, MAKELPARAM(TRUE, 0));
					SendMessageW(hLabel, WM_SETFONT, (WPARAM) hFont_default, MAKELPARAM(TRUE, 0));
					
					//ShowWindow(hwnd_tmp, SW_SHOW);	// already WS_VISIBLE

					CreateTrackBar(hwnd_tmp);
					// SysMonthCal32: 1. Standard 178, 156 + WS_BORDER; 2. Luna 230, 170;
					hMonthCal = CreateWindowW(L"SysMonthCal32", L"",          
						WS_VISIBLE | WS_CHILD,  
						20, 200, 225, 160, hwnd_tmp, (HMENU) 4001, NULL, NULL);

					// GroupBox
					CreateWindowW(TEXT("button"), TEXT("Choose Color"), 
							WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
							260, 20, 120, 150, hwnd_tmp, (HMENU) NULL, NULL, NULL);
					CreateWindowW(TEXT("button"), TEXT("Blue"),
							WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							270, 40, 100, 30, hwnd_tmp, (HMENU) ID_BLUE, NULL, NULL);
					CreateWindowW(TEXT("button"), TEXT("Yellow"),
							WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							270, 65, 100, 30, hwnd_tmp, (HMENU) ID_YELLOW, NULL, NULL);
					CreateWindowW(TEXT("button"), TEXT("Orange"),
							WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							270, 90, 100, 30, hwnd_tmp, (HMENU) ID_ORANGE, NULL, NULL);
					
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
				case 300:
					MessageBoxW(NULL, L"First Program", L"First", MB_OK);
					break;
			}	// switch(LOWORD(wParam))

			break;
		
		case WM_SIZE:
			break;
		case WM_SIZING:
			GetClientRect(hwnd, &rectParent);
			//MoveWindow(ghwndEdit, rectParent.top, rectParent.left, rectParent.right, rectParent.bottom, 
			//	FALSE);
			SetWindowPos(ghwndEdit, 0, rectParent.top, rectParent.left, rectParent.right, rectParent.bottom, 0);
			break;
		case WM_EXITSIZEMOVE:
			//GetClientRect(hwnd, &rectParent);
			//MoveWindow(ghwndEdit, rectParent.top, rectParent.left, rectParent.right, rectParent.bottom, TRUE); 
			break;
		case WM_SETFOCUS:
			//SetFocus(GetDlgItem(hwnd, 0));
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
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			DeleteObject(hFont);
			DeleteObject(hFont_default);
			DeleteObject(hBitmap);
			FreeLibrary(hmod);
			PostQuitMessage(0);
			break;
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}


void AddMenus (HWND hwnd) {
	hMenubar = CreateMenu();
	hMenu = CreateMenu();

	AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");

	AppendMenuW(hMenu, MF_STRING, IDM_VIEW_STB, L"&Statusbar");
	CheckMenuItem(hMenu, IDM_VIEW_STB, MF_UNCHECKED);	// MF_CHECKED, MF_UNCHECKED

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


LRESULT CALLBACK DialogProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	UINT checked;
	int ctrlID;
	int requestID;
	int position;
	LPNMHDR lpNmHdr;
	SYSTEMTIME time;

	switch (msg) {
		case WM_CREATE:
			/*CreateWindow(TEXT("button"), TEXT("Ok"),
			WS_VISIBLE | WS_CHILD ,
			50, 50, 80, 25,
			hwnd, (HMENU) 2, NULL, NULL);*/
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 600:
					GetWindowTextW(hEdit, buf, 50);
					SetWindowTextW(hLabel, buf);
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

			if (HIWORD(wParam) == BN_CLICKED) {
			   switch (LOWORD(wParam)) {
				case ID_BLUE:
					wcscpy(buf, L"blue");
					SetWindowTextW(hDebugLabel, buf);
					break;
				case ID_YELLOW:
					wcscpy(buf, L"yellow");
					SetWindowTextW(hDebugLabel, buf);
					break;
				case ID_ORANGE:
					wcscpy(buf, L"orange");
					SetWindowTextW(hDebugLabel, buf);
					break;
			}
			   
			}
			break;
		case WM_SETFONT:
			//MessageBoxW(NULL, L"Font set", L"First", MB_OK);
			break;
		case WM_HSCROLL:
			ctrlID = GetDlgCtrlID((HWND) lParam);
			requestID = LOWORD(wParam);
			position = SendMessageW((HWND) lParam, TBM_GETPOS, 0, 0);
			switch (ctrlID) {
				case 3001:
					UpdateTrackBar((HWND) lParam);
					break;
			}
			
			break;
		case WM_NOTIFY:
			lpNmHdr = (LPNMHDR) lParam;

			if (lpNmHdr->code==MCN_SELECT) {
				//ZeroMemory(&time, sizeof(SYSTEMTIME));
				SendMessageW(hMonthCal, MCM_GETCURSEL, 0, (LPARAM) &time);
  
				swprintf(buf, 400, L"%d-%d-%d", time.wYear, time.wMonth, time.wDay);
				SetWindowTextW(hDebugLabel, buf);

			}
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break; 
		case WM_DESTROY:
			break;
	}
	
	
	return (DefWindowProc(hwnd, msg, wParam, lParam));
	
}

void RegisterDialogClass (HWND hwnd) {
	WNDCLASSEX wc = {0};
	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc= (WNDPROC) DialogProc;
	wc.hInstance= ghInstance;
	wc.hbrBackground= GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName= L"DialogClass";
	RegisterClassEx(&wc);
}

void CreateDialogBox (HWND hwnd) {
	CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,TEXT("DialogClass"), TEXT("Dialog Box"), 
	WS_VISIBLE | WS_SYSMENU | WS_CAPTION , 100, 100, 200, 150, 
	NULL, NULL, ghInstance,NULL);
}

COLORREF ShowColorDialog (HWND hwnd) {
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
	rwc.lpfnWndProc = PanelProc;
	RegisterClass(&rwc);
}

LRESULT CALLBACK PanelProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps; 
	RECT rect;

	switch (msg) {
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


void CreateMenubar (HWND hwnd) {
	HMENU hMenubar;
	HMENU hMenu;

	hMenubar = CreateMenu();
	hMenu = CreateMenu();
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, TEXT("&File"));
	AppendMenu(hMenu, MF_STRING, IDM_FILE_NEW, TEXT("&Open"));
	SetMenu(hwnd, hMenubar);
}

void OpenDialog (HWND hwnd) {
	OPENFILENAMEW ofn;
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
	
	if (GetOpenFileNameW(&ofn))
		LoadFile_internal(ofn.lpstrFile);
}

void LoadFile_internal (LPCWSTR file) {
	HANDLE hFile;
	DWORD dwSize;
	DWORD dw;
	LPBYTE lpBuffer;
	WCHAR ws_buf[50000] = {0};		/* BUG: buffer overflow ! */

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
	//MessageBoxW(NULL, ws_buf, L"First", MB_OK);

	HeapFree(GetProcessHeap(), 0, lpBuffer);
}


void CreateMyTooltip (HWND hwnd) {
	HWND hwndTT;
	TOOLINFO ti;
	RECT rect;
	WCHAR tooltip[30] = L"A main window";

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

	SendMessageW(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
}


void CreateTrackBar (HWND hwnd) {
	//INITCOMMONCONTROLSEX icex;
	
	HWND hLeftLabel = CreateWindowW(L"STATIC", L"0", 
	WS_CHILD | WS_VISIBLE, 0, 0, 10, 30, hwnd, (HMENU)1, NULL, NULL);

	HWND hRightLabel = CreateWindowW(L"STATIC", L"100", 
	WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU)2, NULL, NULL);

	

	//InitCommonControlsEx(NULL);
	//icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	//icex.dwICC= ICC_LISTVIEW_CLASSES;
	//InitCommonControlsEx(&icex);

	hTrack = CreateWindowW(L"msctls_trackbar32", L"Trackbar Control",
	WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ,
	20, 150, 170, 30, hwnd, (HMENU) 3001, NULL, NULL);

	SendMessageW(hTrack, TBM_SETRANGE,TRUE, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0,10);
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPOS, FALSE, 50);
	SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel);
	SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel);
}

void UpdateTrackBar (HWND hTrackBar) {
	LRESULT pos = SendMessageW(hTrackBar, TBM_GETPOS, 0, 0);
	wsprintfW(buf, L"%ld", pos);

	SetWindowTextW(hDebugLabel, buf);
}



// MessageBoxW(NULL, L"First Program", L"First", MB_OK);
