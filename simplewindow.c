/*
	Name    : Win32 API samples (without resource file)
	Author  : Daniel Sirait <dsirait@outlook.com>
	License : Public Domain
*/

/* processorArchitecture value can be 'x86', 'amd64', or '*' */

#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

/* force MSVC to use ANSI/WideChar function, must be before #include <windows.h> */
#define UNICODE
//#undef UNICODE

/* turn off deprecation & warnings */
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE_GLOBALS
#define _CRT_OBSOLETE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS


#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
//#include <exdisp.h>


#define IDM_FILE_NEW 11
#define IDM_FILE_OPEN 12
#define IDM_FILE_QUIT 13
#define IDM_VIEW_STB 14
#define IDM_FILE_DIALOG 15
#define IDM_FILE_COLOR 16
#define IDM_FILE_CONTROL 17
#define IDM_FILE_ANTIALIASING 18
#define IDM_HELP_ABOUT 21
#define IDM_FILE_CHART_HISTOGRAM 5554
#define IDM_FILE_CHART_BAR 5555
#define IDM_FILE_CHART_LINE 5556

HMENU hMenubar1, hMenu1, hMenu2, submenu1, hMenubar2, hMenu2, hPopUp1, chartmenu;
HINSTANCE ghInstance;
HWND ghwndEdit, staticimage1;
HWND hEdit , hLabel, button1, button2, checkbox1, tabButton1, hDialogLabel;
HWND radiobtn1, radiobtn2, radiobtn3, hProgressBar, treeview1, hDebugLabel;
HFONT hfont1, hfont2, hfont3, hfont_custom, hfont_hyperlink, hfont_default;
HBITMAP hBitmap, kurtd3_bitmap;
NONCLIENTMETRICS ncm;
UINT hTrack_id;
HWND ghSb, hTrack, hMonthCal, hCombo, groupbox1;
HWND hDlgCurrent = NULL;
HWND hTab, listbox1, rebar1, toolbar1, hLink1;
HWND hlink_label;
HANDLE hImg;
HMODULE hmod;
HBRUSH hbrush_syscolor;

INT WINAPI wWinMain (HINSTANCE, HINSTANCE, PWSTR, INT);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ControlProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK aaProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK histogramProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PanelProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DestroyChildWindow (HWND, LPARAM);
VOID CALLBACK SetChildWindowFont (HWND, LPARAM);

void CreateDialogBox (HWND);
void RegisterPanel ();
COLORREF ShowColorDialog (HWND);
void OpenDialog (HWND);
void LoadFile_internal (LPCWSTR);
void CreateMyTooltip (HWND);
void CreateTrackBar (HWND);
void UpdateTrackBar ();
void AddMenus (HWND);
HWND BuildToolBar (HWND);
HWND CreateRebar (HWND, HWND);
HTREEITEM AddItemToTree (HWND, LPCWSTR, int);
void CenterWindow (HWND);


WCHAR s_buf[500];
COLORREF gColor = RGB(255, 255, 255);


INT WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, INT nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEX wc = {0};		// initialize with NULL / 0
	INITCOMMONCONTROLSEX iccex;
	ACCEL accel[2];
	HACCEL hAccel;
	LONG style;

	ZeroMemory(&wc, sizeof(wc));			// initialize with NULL (other alternative)
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra= 0;
	wc.cbWndExtra= 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName= NULL;
	wc.lpszClassName = L"Window";
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;	// default window , COLOR_WINDOW
	//wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));	// make something different
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);	// Note: LoadCursor() superseded by LoadImage()
	wc.hIcon = (HICON) LoadImageW(NULL, L"razor.ico", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wc.hIconSm = (HICON) LoadImageW(NULL, L"razor.ico", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);

	iccex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	//InitCommonControls();			// obsolete
	InitCommonControlsEx(&iccex);

	if (!RegisterClassExW(&wc)) {
		MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	
	// create main window
	hwnd = CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT, 
		wc.lpszClassName, L"Title", 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_TABSTOP, 
		0, 0, 800, 600, (HWND) NULL, (HMENU) NULL, hInstance, NULL);
	
	// override Style
	/*style = GetWindowLong(hwnd, GWL_STYLE);
	style = style & ~(WS_MINIMIZEBOX | WS_SYSMENU);
	SetWindowLongPtrA(hwnd, GWL_STYLE, style);*/
	
	ShowWindow(hwnd, nCmdShow);
	//UpdateWindow(hwnd);

	// Create Keyboard Accelerator/Shorcut, Ctrl+A, Ctrl+Alt+C
	accel[0].fVirt = FCONTROL | FVIRTKEY;
	accel[0].key = 'A';			// must be uppercase
	accel[0].cmd = 9101;		// msg code to send to WM_COMMAND
	accel[1].fVirt = FCONTROL | FALT | FVIRTKEY;
	accel[1].key = 'C';			// must be uppercase
	accel[1].cmd = 9102;		// msg code to send to WM_COMMAND

	hAccel = CreateAcceleratorTableW(accel, 2);
	//hAccel = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDC_SHORTCUT));

	while (GetMessageW(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		if (!IsDialogMessageW(hDlgCurrent, &msg)) {				/* Disable keyboard shorcut when other window active  */
			if (!TranslateAcceleratorW(hwnd, hAccel, &msg)) {	/* Handle Keyboard shortcut */
				TranslateMessage(&msg);		/* Translate key codes to chars if present */
				DispatchMessageW(&msg);		/* Send it to WndProc */
			}
		}
	}
	
	return (int) msg.wParam;
}


BOOL CALLBACK DestroyChildWindow (HWND hwnd, LPARAM lParam) {
	DestroyWindow(hwnd);
	//SendMessageW(hwnd, WM_DESTROY, 0, 0);
	return TRUE;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_control, hwnd_dialog, hwnd_aa;
	POINT point;
	UINT state;
	static HWND hwndPanel;
	RECT rectParent;
	WNDCLASSEX wc1 = {0}, wc2 = {0}, wc3 = {0};		// initialize with 0 / 

	switch (msg) {
		case WM_CREATE:
			CenterWindow(hwnd);
			hBitmap = (HBITMAP) LoadImageW(NULL, L"test123.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			AddMenus(hwnd);
			
			hbrush_syscolor = CreateSolidBrush(GetSysColor(COLOR_MENU));

			//ghSb = CreateStatusWindowW(WS_CHILD | WS_VISIBLE, L"Status bar title", hwnd, 5003);	// obsolete function
			// STATUSCLASSNAME or "msctls_statusbar32"
			ghSb = CreateWindowW(L"msctls_statusbar32", L"new Status bar title", 
				WS_VISIBLE | WS_CHILD | WS_BORDER | SBARS_SIZEGRIP | CCS_BOTTOM, 
				0, 0, 0, 0, hwnd, (HMENU) 5003, NULL, NULL);

			ShowWindow(ghSb, SW_SHOW);

			BuildToolBar(hwnd);
			CreateMyTooltip(hwnd);
			
			// Win XP: Tahoma 13, .NET: Microsoft Sans Serif 14, Win 7: Segoe UI 15
			// get custom font
			hfont1 = CreateFontW(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Tahoma");

			hfont3 = CreateFontW(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Comic Sans MS");

			hfont_custom = CreateFontW(13, 0, 0, 0, FW_BOLD, TRUE, TRUE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Tahoma");

			hfont_hyperlink = CreateFontW(13, 0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Tahoma");

			// get default font from system
			// System font. By default, the system uses the system font to draw menus, dialog box controls, and text. 
			// Windows 95/98/ME/NT: The system font is MS Sans Serif. 
			// Windows 2000/XP: The system font is Tahoma
			// Windows 7/8/10:  The system font is Segoe UI

			hfont_default = GetStockObject(SYSTEM_FONT);

			// get system default font
			ncm.cbSize = sizeof(NONCLIENTMETRICSW);
			SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

			hfont2 = CreateFontIndirectW(&ncm.lfMessageFont);
			
			/*ghwndEdit = CreateWindowW(L"EDIT", L"abcd",
				WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_CLIPCHILDREN,
				50, 50, 260, 180, hwnd, NULL, NULL, NULL);*/

			//SendMessageW(ghwndEdit, WM_SETFONT, (WPARAM) hfont1, TRUE);

			 // RICH EDIT CONTROL
			LoadLibraryW(L"Msftedit.dll");
			ghwndEdit = CreateWindowW(MSFTEDIT_CLASS, NULL,
				WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE |WS_HSCROLL | WS_VSCROLL |WS_BORDER | WS_TABSTOP ,
				50, 50, 260, 180, hwnd, NULL, NULL, NULL);

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
					MessageBoxA(NULL, "Clicked !", "IDM_FILE_NEW", MB_OK);
					break;
				case IDM_HELP_ABOUT:
					ShellExecuteW(NULL, L"open", L"http://bitbucket.org/SiraitX", NULL, NULL, SW_SHOWNORMAL);
					break;
				case IDM_FILE_DIALOG:
					ZeroMemory(&wc3, sizeof(wc3));
					wc3.cbSize = sizeof(WNDCLASSEX);
					wc3.lpfnWndProc = (WNDPROC) DialogProc;
					wc3.hInstance = GetModuleHandleW(NULL);
					wc3.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc3.lpszClassName = L"DialogClass";
					RegisterClassExW(&wc3);
					hwnd_dialog = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, 
						L"DialogClass", L"Dialog, ToolBar, & Rebar", WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 
						100, 100, 400, 400, 
						hwnd, (HMENU) NULL, GetModuleHandleW(NULL),NULL);
					break;
				case IDM_FILE_COLOR:
					gColor = ShowColorDialog(hwnd);
					InvalidateRect(hwndPanel, NULL, TRUE);
					break;
				case IDM_FILE_CONTROL:
					ZeroMemory(&wc1, sizeof(wc1));
					wc1.cbSize = sizeof(WNDCLASSEX);
					wc1.lpfnWndProc = (WNDPROC) ControlProc;
					wc1.hInstance = ghInstance;
					wc1.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc1.lpszClassName = L"ControlClass";
					RegisterClassExW(&wc1);
					
					hwnd_control = CreateWindowExW(WS_EX_DLGMODALFRAME, L"ControlClass", 
						L"Dialog Box", WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 
						100, 100, 800, 550, hwnd, (HMENU) NULL, GetModuleHandle(NULL), NULL);
					
					EnableWindow(hwnd, FALSE);	// make hwnd_tmp modal window

					SendMessageW(hwnd_control, WM_SETFONT, (WPARAM) hfont3, TRUE);

					break;
				case IDM_FILE_ANTIALIASING:
					ZeroMemory(&wc2, sizeof(wc2));
					wc2.cbSize = sizeof(WNDCLASSEX);
					wc2.lpfnWndProc = (WNDPROC) aaProc;
					wc2.hInstance = ghInstance;
					wc2.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc2.lpszClassName = L"AntiAliasingClass";
					RegisterClassExW(&wc2);
					
					hwnd_aa = CreateWindowExW(WS_EX_DLGMODALFRAME, L"AntiAliasingClass", 
						L"Anti Aliasing Example", WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 
						100, 100, 600, 400, hwnd, (HMENU) NULL, GetModuleHandle(NULL), NULL);

					EnableWindow(hwnd, FALSE);	// make hwnd_tmp modal window
					break;
				case IDM_FILE_OPEN:
					OpenDialog(hwnd);
					break;
				case IDM_FILE_QUIT:
					SendMessageW(hwnd, WM_CLOSE, 0, 0);
					break;
				case IDM_VIEW_STB:
					state = GetMenuState(hMenu1, IDM_VIEW_STB, MF_BYCOMMAND);
					if (state == SW_SHOWNA) {
						ShowWindow(ghSb, SW_HIDE);
						CheckMenuItem(hMenu1, IDM_VIEW_STB, MF_UNCHECKED);
					} else {
						ShowWindow(ghSb, SW_SHOW);
						CheckMenuItem(hMenu1, IDM_VIEW_STB, MF_CHECKED);
					}
					break;
				case IDM_FILE_CHART_HISTOGRAM:
					ZeroMemory(&wc2, sizeof(wc2));
					wc2.cbSize = sizeof(WNDCLASSEX);
					wc2.lpfnWndProc = (WNDPROC) histogramProc;
					wc2.hInstance = ghInstance;
					wc2.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc2.lpszClassName = L"HistogramWindowClass";
					RegisterClassExW(&wc2);
					
					hwnd_aa = CreateWindowExW(WS_EX_DLGMODALFRAME, L"HistogramWindowClass", 
						L"Histogram Chart", WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 
						100, 100, 600, 400, hwnd, (HMENU) NULL, GetModuleHandle(NULL), NULL);

					EnableWindow(hwnd, FALSE);	// make hwnd_tmp modal window

					break;
				case 300:
					MessageBoxW(NULL, L"First Program", L"DEBUG", MB_OK);
					break;
				case 9101:
					MessageBoxW(NULL, L"Ctrl+A pressed", L"DEBUG", MB_OK);
					break;
				case 9102:
					MessageBoxW(NULL, L"Ctrl+C pressed", L"DEBUG", MB_OK);
					break;
			}	// switch(LOWORD(wParam))

			break;
		case WM_ACTIVATE:
			break;
		case WM_KEYDOWN:

			break;
		case WM_SIZE:
			break;
		case WM_SIZING:
			GetClientRect(hwnd, &rectParent);
			MoveWindow(ghwndEdit, rectParent.top, rectParent.left, rectParent.right, rectParent.bottom, FALSE);
			// MoveWindow and SetWindowPos are similar but SetWindowsPos provides Z-ordering of the window
			//SetWindowPos(ghwndEdit, 0, rectParent.top, rectParent.left, rectParent.right, rectParent.bottom, 0);
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_SETFOCUS:
			break;
		case WM_RBUTTONUP:
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			hPopUp1 = CreatePopupMenu();
			ClientToScreen(hwnd, &point);

			AppendMenuW(hPopUp1, MF_STRING, IDM_FILE_NEW, L"&New");
			AppendMenuW(hPopUp1, MF_STRING, IDM_FILE_OPEN, L"&Open");
			AppendMenuW(hPopUp1, MF_SEPARATOR, 0, NULL);
			AppendMenuW(hPopUp1, MF_STRING, IDM_FILE_QUIT, L"&Quit");

			TrackPopupMenu(hPopUp1, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
			DestroyMenu(hPopUp1);
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyWindow(hwnd);
			//DeleteObject(hbrush_syscolor);
			break;
		case WM_DESTROY:
			DeleteObject(hfont1);
			DeleteObject(hfont2);
			DeleteObject(hfont3);
			DeleteObject(hfont_custom);
			DeleteObject(hfont_default);
			DeleteObject(hfont_hyperlink);

			DeleteObject(hBitmap);

			FreeLibrary(hmod);

			PostQuitMessage(0);
			break;
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}


void AddMenus (HWND hwnd) {
	submenu1 = CreateMenu();
	chartmenu = CreateMenu();
	hMenubar1 = CreateMenu();
	hMenu1 = CreateMenu();
	hMenu2 = CreateMenu();

	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_NEW, L"&New\tCtrl+N");
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_OPEN, L"&Open\tCtrl+C+O");

	AppendMenuW(hMenu1, MF_STRING, IDM_VIEW_STB, L"&Statusbar");
	CheckMenuItem(hMenu1, IDM_VIEW_STB, MF_CHECKED);	// MF_CHECKED, MF_UNCHECKED

	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_DIALOG, L"&Dialog, ToolBar, && Rebar");
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_COLOR, L"&Color");

	AppendMenuW(submenu1, MF_STRING, 5553, L"&from AppendMenuW()\tCtrl+M");
	AppendMenuW(chartmenu, MF_STRING, 5554, L"&Histogram Chart");
	AppendMenuW(chartmenu, MF_STRING, 5555, L"&Bar Chart");
	AppendMenuW(chartmenu, MF_STRING, 5556, L"&Line Chart");
	
	AppendMenuW(hMenu1, MF_POPUP, (UINT_PTR) submenu1, L"&Submenu1");

	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_CONTROL, L"Win32 Standard &Control");
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_ANTIALIASING, L"&Anti Aliasing");
	AppendMenuW(hMenu1, MF_POPUP, (UINT_PTR) chartmenu, L"&Charts");

	AppendMenuW(hMenu1, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_QUIT, L"&Quit");

	
	AppendMenuW(hMenubar1, MF_POPUP, (UINT_PTR) hMenu1, L"&File");

	// Help Menu
	AppendMenuW(hMenu2, MF_STRING, IDM_HELP_ABOUT, L"&About");
	AppendMenuW(hMenubar1, MF_POPUP, (UINT_PTR) hMenu2, L"&Help");

	
	SetMenuItemBitmaps(hMenu1, IDM_FILE_OPEN, MF_BITMAP | MF_BYCOMMAND, hBitmap, hBitmap);


	SetMenu(hwnd, hMenubar1);
}


void PremultiplyBitmapAlpha (HDC hDC, HBITMAP hBmp) {
	BITMAP bm = { 0 };
	GetObject(hBmp, sizeof(bm), &bm);
	BITMAPINFO *bmi = (BITMAPINFO*) _alloca(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	ZeroMemory(bmi, sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BOOL bRes = GetDIBits(hDC, hBmp, 0, bm.bmHeight, NULL, bmi, DIB_RGB_COLORS);
	if( !bRes || bmi->bmiHeader.biBitCount != 32 ) return;
	LPBYTE pBitData = (LPBYTE) LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
	if( pBitData == NULL ) return;
	LPBYTE pData = pBitData;
	GetDIBits(hDC, hBmp, 0, bm.bmHeight, pData, bmi, DIB_RGB_COLORS);
	for (int y = 0; y < bm.bmHeight; y++) {
		for( int x = 0; x < bm.bmWidth; x++ ) {
			pData[0] = (BYTE)((DWORD)pData[0] * pData[3] / 255);
			pData[1] = (BYTE)((DWORD)pData[1] * pData[3] / 255);
			pData[2] = (BYTE)((DWORD)pData[2] * pData[3] / 255);
			pData += 4;
		}
	}
	SetDIBits(hDC, hBmp, 0, bm.bmHeight, pBitData, bmi, DIB_RGB_COLORS);
	LocalFree(pBitData);
}

LRESULT CALLBACK ControlProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Declaring variable inside WndProc() & DialogProc() is a bad practice because the function
	// will get called very often and slow down performance
	HWND hwnd_parent;
	UINT checked;
	int ctrlID, requestID, position, tab_index;
	SYSTEMTIME time;
	int i = 0;
	BOOL ret;
	TCITEMW tabItem1, tabItem2, tabItem3, tabItem4, tabItem5;
	WCHAR os_list[5][32] = {L"MSDOS", L"Windows 98 SE", L"Windows ME", L"Windows XP", L"Windows 7"};
	WCHAR os_other[6][32] = {L"UNIX", L"Linux", L"BSD", L"Plan 9", L"Mac OS X", L"OS/2 WARP"};
	
	hwnd_parent = GetWindow(hwnd, GW_OWNER);
	

	//IsDialogMessageW(hwnd, (LPMSG) &msg);
	switch (msg) {
		case WM_INITDIALOG:
			break;
		case WM_CREATE:
			// window class
			// macro: WC_STATIC, WC_BUTTON, WC_EDIT, WC_COMBOBOX, WC_SCROLLBAR, WC_LISTBOX
			// value: "STATIC" "BUTTON" "EDIT" "COMBOBOX" "SCROLLBAR" "LISTBOX"
			
			// default Height: Edit 21, Button 25, Static 13, CheckBox 17
			hLabel = CreateWindowW(L"STATIC", L"This is Label", WS_CHILD | WS_VISIBLE | SS_LEFT, 
				20, 80, 150, 13, hwnd, (HMENU) 500, NULL, NULL);

			// WS_TABSTOP: The window is a control that can receive the keyboard focus when the user presses the TAB key. 
			button1 = CreateWindowW(L"BUTTON", L"Push &Button", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
				20, 50, 90, 25, hwnd, (HMENU) 600, NULL, NULL);
			hImg = LoadImageW(NULL, L"test123.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			SendMessageW(button1, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hImg);
			
			button2 = CreateWindowW(L"BUTTON", L"&Quit", WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_DISABLED,
				120, 50, 80, 25, hwnd, (HMENU) 650, NULL, NULL);

			checkbox1 = CreateWindowW(L"BUTTON", L"This is &Checkbox", 
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX | WS_TABSTOP, 
				20, 10, 110, 17, hwnd, (HMENU) 700, NULL, NULL);
			// BST_CHECKED / BST_INDETERMINATE / BST_UNCHECKED
			//CheckDlgButton(hwnd, 700, BST_CHECKED);	// same below
			SendMessageW(checkbox1, BM_SETCHECK, BST_CHECKED, 0);

			hDebugLabel = CreateWindowW(L"STATIC", L"Debug Label !!!", WS_VISIBLE | WS_CHILD | SS_LEFT, 
				550, 20, 200, 13, hwnd, (HMENU) 3111, NULL, NULL);

			// EDIT ctrl: max 32,767 bytes
			hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"This is edit", 
				WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL | ES_NOHIDESEL,
				20, 110, 185, 21, hwnd, (HMENU) 800, NULL, NULL);	// WS_BORDER

			//hResize = CreateWindowW(L"STATIC", L"Debug Label !!!", WS_CHILD | WS_VISIBLE, 
			//	550, 20, 200, 13, hwnd, (HMENU) 3111, NULL, NULL);

			// better font
			SendMessageW(hEdit, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageW(hLabel, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageW(button2, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageW(checkbox1, WM_SETFONT, (WPARAM) hfont3, TRUE);

			SendMessageW(button1, WM_SETFONT, (WPARAM) hfont2, TRUE);	
			SendMessageW(hEdit, WM_SETFONT, (WPARAM) hfont2, MAKELPARAM(TRUE, 0));
			SendMessageW(hDebugLabel, WM_SETFONT, (WPARAM) hfont1, TRUE);
			
			//ShowWindow(hwnd_tmp, SW_SHOW);	// already WS_VISIBLE

			CreateTrackBar(hwnd);

			// Calendar
			// MONTHCAL_CLASS/SysMonthCal32: 1. Standard 178, 156 + WS_BORDER; 2. Luna 230, 170;
			hMonthCal = CreateWindowW(L"SysMonthCal32", NULL,
				WS_VISIBLE | WS_CHILD | WS_TABSTOP,
				20, 200, 225, 160, hwnd, (HMENU) 4001, NULL, NULL);

			// GroupBox
			groupbox1 = CreateWindowW(L"BUTTON", L"Choose Color",
				WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
				260, 20, 120, 150, hwnd, (HMENU) NULL, NULL, NULL);
			radiobtn1 = CreateWindowW(L"BUTTON", L"Blue",
				WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,	// WS_GROUP: start exclusive radiobtn
				270, 40, 100, 30, hwnd, (HMENU) 6001, NULL, NULL);
			radiobtn2 = CreateWindowW(L"BUTTON", L"Yellow",
				WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,	// WS_GROUP: start another exclusive, until radiobtn3
				270, 65, 100, 30, hwnd, (HMENU) 6002, NULL, NULL);
			radiobtn3 = CreateWindowW(L"BUTTON", L"Orange",
				WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
				270, 90, 100, 30, hwnd, (HMENU) 6003, NULL, NULL);

			SendMessageW(radiobtn2, BM_SETCHECK, BST_CHECKED, TRUE);		// set default value to radiobtn2

			// ComboBox: CBS_DROPDOWN or CBS_DROPDOWNLIST, msg CB_SETCURSEL, CB_GETCURSEL
			// [F4] or [Alt + Up/Down] arrow key to display the list using keyboard (use TAB STOP until focus)
			hCombo = CreateWindowW(L"COMBOBOX", NULL,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
				410, 20, 120, 110, hwnd, (HMENU) 7500, NULL, NULL);

			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"MSDOS");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows 98 SE");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows ME");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows XP");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows 7");
			
			SendMessageW(hCombo, CB_SETCURSEL, 4, 0);	// set default index for ComboBox
			
			// set font
			SendMessageW(hCombo, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(groupbox1, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(radiobtn1, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(radiobtn2, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(radiobtn3, WM_SETFONT, (WPARAM) hfont2, TRUE);

			// PROGRESS_CLASS or L"msctls_progress32"
			hProgressBar = CreateWindowW(L"msctls_progress32", NULL,
				WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
				20, 380, 190, 25, hwnd, (HMENU) 7301, NULL, NULL);

			SendMessageW(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
			SendMessageW(hProgressBar, PBM_SETSTEP, 1, 0);
			SendMessageW(hProgressBar, PBM_SETPOS, 100, 0);

			
			// WC_TABCONTROL or "SysTabControl32"
			hTab = CreateWindowW(L"SysTabControl32", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_MULTILINE,
				400, 70, 150, 150, hwnd, (HMENU) 2444, NULL, NULL);

			tabItem1.mask = TCIF_TEXT;
			tabItem1.pszText = L"Tab 1";

			tabItem2.mask = TCIF_TEXT;
			tabItem2.pszText = L"Tab 2";

			tabItem3.mask = TCIF_TEXT;
			tabItem3.pszText = L"Tab 3";

			tabItem4.mask = TCIF_TEXT;
			tabItem4.pszText = L"Tab 4";

			tabItem5.mask = TCIF_TEXT;
			tabItem5.pszText = L"Tab 5";

			SendMessageW(hTab, TCM_INSERTITEM, 0, (LPARAM) &tabItem1);
			SendMessageW(hTab, TCM_INSERTITEM, 1, (LPARAM) &tabItem2);
			SendMessageW(hTab, TCM_INSERTITEM, 2, (LPARAM) &tabItem3);
			SendMessageW(hTab, TCM_INSERTITEM, 3, (LPARAM) &tabItem4);
			SendMessageW(hTab, TCM_INSERTITEM, 4, (LPARAM) &tabItem5);

			SendMessageW(hTab, TCM_SETCURSEL, 0, 0);	// Selects a tab in a tab control
			 
			tabButton1 = CreateWindowW(L"BUTTON", L"Tab Button", WS_CHILD | WS_VISIBLE, 
				430, 140, 90, 30, hwnd, (HMENU) 3444, NULL, NULL);

			BringWindowToTop(tabButton1);

			// CW_LISTBOX or "LISTBOX"
			listbox1 = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", NULL, 
				WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS,
				260, 200, 120, 120, hwnd, (HMENU) 3555, NULL, NULL);

			SendMessageW(listbox1, LB_ADDSTRING, 0, (LPARAM) L"UNIX");
			SendMessageW(listbox1, LB_ADDSTRING, 0, (LPARAM) L"Linux");
			SendMessageW(listbox1, LB_ADDSTRING, 0, (LPARAM) L"BSD");
			SendMessageW(listbox1, LB_ADDSTRING, 0, (LPARAM) L"Plan 9");
			SendMessageW(listbox1, LB_ADDSTRING, 0, (LPARAM) L"Mac OS X");
			SendMessageW(listbox1, LB_ADDSTRING, 0, (LPARAM) L"OS/2 WARP");

			// set font
			SendMessageW(hTab, WM_SETFONT, (WPARAM) hfont1, TRUE);
			//SendMessageW(tabButton1, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageW(listbox1, WM_SETFONT, (WPARAM) hfont1, TRUE);

			// WC_TREEVIEW or "SysTreeView32"
			treeview1 = CreateWindowExW(WS_EX_CLIENTEDGE, L"SysTreeView32", NULL, 
				WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT, 
				400, 230, 150, 100, hwnd, (HMENU) 7265, NULL, NULL);

			AddItemToTree(treeview1, L"treeview1 test", 0);
			AddItemToTree(treeview1, L"child", 1);

			// STATIC Image
			staticimage1 = CreateWindowW(L"STATIC", L"staticimage1", 
				WS_CHILD | WS_VISIBLE | SS_BITMAP,
				560, 50, 0, 0, hwnd, (HMENU) 9524, NULL, NULL);


			

			// LoadImage(): 0 -> actual resource size,  LR_DEFAULTSIZE -> fit to parent
			kurtd3_bitmap = LoadImageW(NULL, L"kurtd3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			/*HDC hdc = GetDC(kurtd3_bitmap);
			HDC hdcMem = CreateCompatibleDC(hdc);
			BLENDFUNCTION fn;

			ZeroMemory(&fn, sizeof(fn));
			fn.BlendOp = AC_SRC_OVER;
			fn.BlendFlags = 0;
			fn.SourceConstantAlpha = 127;
			fn.AlphaFormat = AC_SRC_ALPHA;

			PremultiplyBitmapAlpha(hdc, kurtd3_bitmap);

			AlphaBlend(hdc, 0, 0, 636, 380, hdcMem, 0, 0, 636, 380, fn);*/



			
			SendMessageW(staticimage1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) kurtd3_bitmap);
			

			// SysLink (hyperlink)
			hLink1 = CreateWindowW(L"SysLink" , L"SysLink: <a id=\"click1_id\" href=\"http://click1.com/\">Click 1</a> and <a id=\"click2_id\" href=\"http://click2.com/\">Click 2</a>", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
				270, 350, 180, 15, hwnd, (HMENU) 7600, NULL, NULL);

			SendMessageW(hLink1, WM_SETFONT, (WPARAM) hfont2, TRUE);

			// HyperLink using Static Control (Label)
			hlink_label = CreateWindowW(L"STATIC", L"Static Control HyperLink (click Here)", 
				WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP | SS_NOTIFY, 
				400, 400, 150, 13, hwnd, (HMENU) 1200, NULL, NULL);

			SendMessageW(hlink_label, WM_SETFONT, (WPARAM) hfont1, TRUE);

			

			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 600:
					SendMessageW(hEdit, WM_GETTEXT, 100, (LPARAM) s_buf);
					SendMessageW(hLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					Beep(40, 50);

					// change progress bar 
					i = 0;
					SendMessageW(hProgressBar, PBM_SETPOS, 0, 0);
					SetTimer(hwnd, 8866, 0, NULL);
					break;
				case 650:
					SendMessageW(hwnd, WM_CLOSE, 0, 0);
					BringWindowToTop(hwnd_parent);		// show parent window
					break;
				case 700:
					checked = SendMessageW(checkbox1, BM_GETCHECK, 0, 0);
					if (checked) {
						SendMessageW(checkbox1, BM_SETCHECK, BST_UNCHECKED, 0);
						SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM) L"unchecked");
					} else {
						SendMessageW(checkbox1, BM_SETCHECK, BST_CHECKED, 0);
						SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM) L"checked");
					}
					break;
				case 800:
					SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM) L"asd");
					break;
				case 3444:
					MessageBoxW(NULL, L"tabButton1 clicked", L"DEBUG", MB_OK);
					break;
				case 3555:
					// ListBox msg
					if (HIWORD(wParam) == LBN_SELCHANGE) {
						i = SendMessageW(listbox1, LB_GETCURSEL, 0, 0);
						SendMessageW(listbox1, LB_GETTEXT, i, (LPARAM) &s_buf);		// fetch from listbox1 Control (much better)
						//wcscpy(s_buf, os_other[i]);	// fetch from local sBuffer
						SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					}
					break;
				case 7500:
					// ComboBox msg
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						i = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
						SendMessageW(hCombo, CB_GETLBTEXT, i, (LPARAM) s_buf);

						//SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);			// fetch from hCombo (much better)
						//SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) os_list[i]);	// fetch from local sBuffer

						SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					}
					break;
				case 1200:
					if (HIWORD(wParam) == STN_CLICKED)
						ShellExecuteW(NULL, L"open", L"http://bitbucket.org/SiraitX", NULL, NULL, SW_SHOWNORMAL);
					break;
				case 6001:
					if (HIWORD(wParam) == BN_CLICKED) {
						wcscpy(s_buf, L"blue clicked");
						SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					}
                    break;
				case 6002:
					if (HIWORD(wParam) == BN_CLICKED) {
						wcscpy(s_buf, L"yellow clicked");
						SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					}
					break;
				case 6003:
					if (HIWORD(wParam) == BN_CLICKED) {
						wcscpy(s_buf, L"orange clicked");
						SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					}
					break;
			}


			
			

			break;

			
		
		case WM_CTLCOLORSTATIC:
			if ((HWND)lParam == GetDlgItem(hwnd, 500)) {
				// we're about to draw the static
				// set the text colour in (HDC)lParam
				SetTextColor((HDC) wParam, RGB(255,0,255));		// change text color
				SelectObject((HDC) wParam, hfont_custom);		// change font handle
			} else if ((HWND)lParam == GetDlgItem(hwnd, 1200)) {
				SetTextColor((HDC) wParam, RGB(0,0,255));		// change text color
				SelectObject((HDC) wParam, hfont_hyperlink);		// change font handle
			}
			SetBkMode((HDC) wParam, TRANSPARENT);			// change text background color
			return hbrush_syscolor;		// set background color using HBRUSH
			break;
		case WM_CTLCOLORBTN:
			{
			if ((HWND)lParam == GetDlgItem(hwnd, 600))
				SetTextColor((HDC) wParam, RGB(255,0,0));
			}
			break;
		case WM_DRAWITEM:
			{
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
			if (pDIS->hwndItem == GetDlgItem(hwnd, 600)) {
				SetTextColor(pDIS->hDC, RGB(100, 0, 100));
			}
			}
			break;
		case WM_ACTIVATE:
			if (0 == wParam)		// becoming inactive
				hDlgCurrent = NULL;
			else					// becoming active
				hDlgCurrent = hwnd;
			break;
		case WM_SETFOCUS:
			break;
		case WM_SETFONT:
			// set all child control to use specific font
			EnumChildWindows(hwnd, SetChildWindowFont, lParam);
			break;
		case WM_TIMER:
			SendMessageW(hProgressBar, PBM_STEPIT, 0, 0);
			i++;
			if (i >= 100) 
				KillTimer(hwnd, 8866);
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
			switch ( ((LPNMHDR)lParam)->code ) {
				case NM_CUSTOMDRAW:

					// WM_CTLCOLORBTN won't work to set Button text color
					// Buttons with the BS_PUSHBUTTON, BS_DEFPUSHBUTTON, or BS_PUSHLIKE styles do not use the returned brush. 
					{
						LPNMHDR nmhdr = (LPNMHDR) lParam;

						if (nmhdr->idFrom == 600 && nmhdr->code == NM_CUSTOMDRAW)
						{
							LPNMCUSTOMDRAW custDraw = (LPNMCUSTOMDRAW)(nmhdr);

							if (custDraw->dwDrawStage == CDDS_PREPAINT)
							{
								const int textLength = GetWindowTextLength(custDraw->hdr.hwndFrom);

								if (textLength > 0)
								{
									CHAR buttonText[100];
									SIZE dimensions = {0};
									int xPos, yPos;

									GetWindowTextA(custDraw->hdr.hwndFrom, buttonText, textLength+1);
									GetTextExtentPoint32A(custDraw->hdc, buttonText, textLength, &dimensions);
                        
									xPos = (custDraw->rc.right - dimensions.cx) / 2;
									yPos = (custDraw->rc.bottom - dimensions.cy) / 2;

									SetBkMode(custDraw->hdc, TRANSPARENT);
									SetTextColor(custDraw->hdc, RGB(24, 27, 255));
									TextOutA(custDraw->hdc, xPos, yPos, buttonText, textLength);
                        
									return CDRF_SKIPDEFAULT;
								}
							}

							return CDRF_DODEFAULT;
						}
					}

					break;
				case MCN_SELECT:
					ZeroMemory(&time, sizeof(SYSTEMTIME));
					SendMessageW(hMonthCal, MCM_GETCURSEL, 0, (LPARAM) &time);
					// BUG wsprintf (win api): max buffer = 1024
					// BUG swprintf parameter
					wsprintf(s_buf, L"%d-%d-%d", time.wYear, time.wMonth, time.wDay);
					
					SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
					break;
				case TCN_SELCHANGE:
					tab_index = SendMessageW(hTab, TCM_GETCURSEL, 0, 0);
					if (tab_index == 0) {
						ShowWindow(tabButton1, SW_SHOW);
						BringWindowToTop(tabButton1);
					} else ShowWindow(tabButton1, SW_HIDE);
					break;
				case NM_CLICK:		// msg from SysLink control, Fall through to the next case.
				case NM_RETURN:		// handle mouse click & keyboard accessibility
					switch (((LPNMHDR)lParam)->idFrom) {
						case 7600:
						{
							PNMLINK pNMLink = (PNMLINK) lParam;
							LITEM item = pNMLink->item;

							// using html id
							if (!lstrcmpiW(pNMLink->item.szID, L"click1_id")) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							} else if (!lstrcmpiW(pNMLink->item.szID, L"click2_id")) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							}

							// another way using number
							/*if (item.iLink == 0) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							} else if (item.iLink == 1) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							}*/
							break;
						}
					}
					break;
			}

			break;
		case WM_PAINT:
			break;
		case WM_CLOSE:
			ret = EnableWindow(hwnd_parent, TRUE);
			if (ret == FALSE) MessageBoxW(NULL, L"GetWindow() FAIL", L"DEBUG", MB_OK);

			BringWindowToTop(hwnd_parent);		// show parent window

			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyWindow(hwnd);

			DeleteObject(kurtd3_bitmap);
			DeleteObject(hImg);
			break;
		case WM_DESTROY:
			break;
		default: 
			return DefWindowProc(hwnd, msg, wParam, lParam); 
	}
	
	
	return DefWindowProcW(hwnd, msg, wParam, lParam);
	
}


VOID CALLBACK SetChildWindowFont (HWND hwnd, LPARAM lParam) {
	SendMessageW(hwnd, WM_SETFONT, (WPARAM) hfont3, TRUE);
}


//double cubicInterpolate (double p[4], double x) {
//	return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
//}
//
//double bicubicInterpolate (double p[4][4], double x, double y) {
//	double arr[4];
//	arr[0] = cubicInterpolate(p[0], y);
//	arr[1] = cubicInterpolate(p[1], y);
//	arr[2] = cubicInterpolate(p[2], y);
//	arr[3] = cubicInterpolate(p[3], y);
//	return cubicInterpolate(arr, x);
//}




BYTE BicubicInterpolation (BYTE p[4][4]) {
	FLOAT x1, x2, x3;
	UINT32 x;
	x1 = (p[1][1] + p[1][2] + p[2][1] + p[2][2]) / 4;
	x2 = (p[0][0] + p[0][3] + p[3][0] + p[3][3]) / 4;
	x3 = (p[0][1] + p[0][2] + p[1][0] + p[1][3] + p[2][0] + p[2][3] + p[3][1] + p[3][2]) / 8;
	x = (x1 + (x2 * (4.f/6.f)) + x3 * (2.f/6.f));
	return x;
}

BYTE ChangeEndianness (BYTE value) {
    BYTE result = 0;
    result |= (value & 0x000000FF) << 24;
    result |= (value & 0x0000FF00) << 8;
    result |= (value & 0x00FF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;
    return result;
}

void paintRect (HDC hdc) {
	RECT dim = {0,0,200,200};
        HDC tempHdc = CreateCompatibleDC(hdc);
        BLENDFUNCTION blend;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = 127;	// half transparent
		blend.AlphaFormat = AC_SRC_ALPHA;	// 0, AC_SRC_ALPHA

        HBITMAP hbitmap;       // bitmap handle 
        BITMAPINFO bmi;        // bitmap header 
        // zero the memory for the bitmap info 
        ZeroMemory(&bmi, sizeof(BITMAPINFO));

        // setup bitmap info  
        // set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas. 
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = dim.right-dim.left;
        bmi.bmiHeader.biHeight = -(dim.bottom-dim.top);
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = (dim.right-dim.left) * (dim.bottom-dim.top) * 4;

        // create our DIB section and select the bitmap into the dc 
        hbitmap = LoadImageW(NULL, L"123.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);;//CreateDIBSection(tempHdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0x0);
        //SelectObject(tempHdc, hbitmap);
		//PremultiplyBitmapAlpha (tempHdc, hbitmap);
		//BitBlt(hdc, 0, 0, 200, 200, tempHdc, 0, 0, SRCCOPY);


        /*SetDCPenColor(tempHdc, RGB(0,0,255));
        SetDCBrushColor(tempHdc, RGB(0,0,255));
        FillRect(tempHdc, &dim, CreateSolidBrush(RGB(0,0,255)));
		*/
		//PremultiplyBitmapAlpha (tempHdc, hbitmap);
       AlphaBlend(hdc, dim.left, dim.top, dim.right, dim.bottom, tempHdc, dim.left, dim.top, dim.right, dim.bottom, blend); 


}

/* Anti Aliasing procedure */
LRESULT CALLBACK aaProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_parent;
	BOOL ret;
	HDC hdc, hdc_tmp, hdc_aa;
	PAINTSTRUCT ps, ps2;
	//RECT r;
	int i, j, k;
	HPEN pen_solid1, pen_solid2, holdPen1, holdPen2;
	RECT rc;
	int ssaa_scale;
	HBITMAP bmp;
	HBITMAP bmp_tmp;
	BITMAPINFO bi_aa = {0}, bi_tmp = {0};
	LPCOLORREF pbits, pbits_tmp;
	HBRUSH brush;
	COLORREF cr;
	COLORREF cr1, cr2, cr3, cr4;
	BYTE red, green, blue;
	int count;
	RGBQUAD *b,*c;

	switch (msg) {
		case WM_CREATE:
			CreateWindowW(L"STATIC", L"Anti Aliasing 2x \n(Bilinear interpolation)", 
				WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP, 
				50, 250, 250, 37, hwnd, (HMENU) 4714, NULL, NULL);
			CreateWindowW(L"STATIC", L"Original (No AA)", 
				WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP, 
				380, 250, 250, 17, hwnd, (HMENU) 4714, NULL, NULL);

			//InvalidateRect(hwnd, NULL, FALSE);

			break;
		case WM_PAINT:
			//GetClientRect(hwnd, &r);

			hdc = BeginPaint(hwnd, &ps);
			
			// draw line & super sample anti aliasing
			ssaa_scale = 2;
			ZeroMemory(&bi_tmp, sizeof(BITMAPINFOHEADER));
			bi_tmp.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi_tmp.bmiHeader.biWidth = 200*ssaa_scale;
			bi_tmp.bmiHeader.biHeight = -200*ssaa_scale;	// (-): top-bottom, (+): bottom-top
			bi_tmp.bmiHeader.biPlanes = 1;
			bi_tmp.bmiHeader.biBitCount = 32;
			bi_tmp.bmiHeader.biClrUsed = 0;
			bi_tmp.bmiHeader.biClrImportant = 0;
			bi_tmp.bmiHeader.biCompression = BI_RGB;


			hdc_tmp = CreateCompatibleDC(NULL);
			pen_solid1 = CreatePen(PS_SOLID, 1*ssaa_scale, RGB(0, 0, 0));
			SelectObject(hdc_tmp, pen_solid1);
			
			
			//bmp_tmp = CreateCompatibleBitmap(hdc, 200*ssaa_scale, 200*ssaa_scale);
			bmp_tmp = CreateDIBSection(NULL, &bi_tmp, DIB_RGB_COLORS, &pbits_tmp, NULL, 0);
			SelectObject(hdc_tmp, bmp_tmp);

			rc.left = 0;
			rc.top = 0;
			rc.right = 200*ssaa_scale;
			rc.bottom = 200*ssaa_scale;


			brush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));	// get dialog window background color
			FillRect(hdc_tmp, &rc, brush);			
			SelectObject(hdc_tmp, brush);
			//

			MoveToEx(hdc_tmp, 0, 0, NULL);
			LineTo(hdc_tmp, 50*ssaa_scale, 200*ssaa_scale);
			Ellipse(hdc_tmp, 30*ssaa_scale, 30*ssaa_scale, 120*ssaa_scale, 90*ssaa_scale);

			//MoveToEx(hdc_tmp, 50, 50, NULL);
			//LineTo(hdc_tmp, 50, 50);

			// StretchBlt with HALFTONE is same as Bilinear interpolation
			//setstretchbltmode(hdc, halftone);
			//stretchblt(hdc, 50, 0, 200, 200, hdc_tmp, 0, 0, 200*ssaa_scale, 200*ssaa_scale, srccopy);
			
			ZeroMemory(&bi_aa, sizeof(BITMAPINFOHEADER));
			bi_aa.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi_aa.bmiHeader.biWidth = 200;
			bi_aa.bmiHeader.biHeight = -200;		// (-): top-bottom, (+): bottom-top
			bi_aa.bmiHeader.biPlanes = 1;
			bi_aa.bmiHeader.biBitCount = 32;
			bi_aa.bmiHeader.biClrUsed = 0;
			bi_aa.bmiHeader.biClrImportant = 0;
			bi_aa.bmiHeader.biCompression = BI_RGB;

			bmp = CreateDIBSection(NULL, &bi_aa, DIB_RGB_COLORS, &pbits, NULL, 0);
			hdc_aa = CreateCompatibleDC(NULL);
			SelectObject(hdc_aa, bmp);

			b = pbits_tmp;
			c = pbits;
			// 24 bit: BGRBGR.... + 0 padding to 1-byte per 3-byte BGR	(very confusing)
			// 32-bit: BGRA	(much easier to use than 24-bit)

			// Bilinear interpolation (Fast)
			for (i=0; i<200; i++) {
				for (j=0; j<200; j++) {
					c[i*200+j].rgbBlue = ( b[i*400*2+j*2].rgbBlue + b[i*400*2+(j*2+1)].rgbBlue + b[(i*400*2+400)+j*2].rgbBlue + b[(i*400*2+400)+(j*2+1)].rgbBlue ) / 4;
					c[i*200+j].rgbGreen = ( b[i*400*2+j*2].rgbGreen + b[i*400*2+(j*2+1)].rgbGreen + b[(i*400*2+400)+j*2].rgbGreen + b[(i*400*2+400)+(j*2+1)].rgbGreen ) / 4;
					c[i*200+j].rgbRed = ( b[i*400*2+j*2].rgbRed + b[i*400*2+(j*2+1)].rgbRed + b[(i*400*2+400)+j*2].rgbRed + b[(i*400*2+400)+(j*2+1)].rgbRed ) / 4;
					//c[i*200+j].rgbReserved = 0;		// not used (reserved for alpha)
				}
			}
			
			

			// Bilinear interpolation (Slow)
			//for (i=0; i<200; i++) {
			//	for (j=0; j<200; j++) {
			//		cr1 = GetPixel(hdc_tmp, i*ssaa_scale, j*ssaa_scale);
			//		cr2 = GetPixel(hdc_tmp, i*ssaa_scale, 1+j*ssaa_scale);
			//		cr3 = GetPixel(hdc_tmp, 1+i*ssaa_scale, j*ssaa_scale);
			//		cr4 = GetPixel(hdc_tmp, 1+i*ssaa_scale, 1+j*ssaa_scale);

			//		red = (GetRValue(cr1) + GetRValue(cr2) + GetRValue(cr3) + GetRValue(cr4)) / 4;
			//		green = (GetGValue(cr1) + GetGValue(cr2) + GetGValue(cr3) + GetGValue(cr4)) / 4;
			//		blue = (GetBValue(cr1) + GetBValue(cr2) + GetBValue(cr3) + GetBValue(cr4)) / 4;
			//		
			//		// BUG: SetPixel() is too slow, better use BitBlt pointer 'pbits'
			//		SetPixelV(hdc_aa, i, j, RGB(red, green, blue));

			//		//pbits[i*200+j] = RGB(red, green, blue);		// OK. 24-bit interpolation
			//		//pbits[i*200+j] = RGB(0,255,0);
			//		
			//	}
			//}

			BitBlt(hdc, 0, 0, 200, 200, hdc_aa, 0, 0, SRCCOPY);
			//BitBlt(hdc, 0, 0, 400, 400, hdc_tmp, 0, 0, SRCCOPY);

			SelectObject(hdc, brush);
			// draw line & elipse without AA (original)
			pen_solid2 = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			holdPen2 = SelectObject(hdc, pen_solid2);
			MoveToEx(hdc, 400, 0, NULL);
			LineTo(hdc, 50+400, 200);
			Ellipse(hdc, 30+400, 30, 120+400, 90);
			
			paintRect(hdc);

			EndPaint(hwnd, &ps);
			
			// free heap
			DeleteObject(brush);
			DeleteObject(bmp);
			DeleteObject(bmp_tmp);
			DeleteObject(pen_solid1);
			DeleteObject(pen_solid2);
			DeleteObject(holdPen2);
			DeleteDC(hdc_tmp);
			DeleteDC(hdc_aa);
			DeleteDC(hdc);

			break;
		case WM_CLOSE:
			
			hwnd_parent = GetWindow(hwnd, GW_OWNER);
			ret = EnableWindow(hwnd_parent, TRUE);
			BringWindowToTop(hwnd_parent);		// show parent window

			if (ret == FALSE) MessageBoxW(NULL, L"GetWindow() FAIL", L"DEBUG", MB_OK);
			
			ShowWindow(hwnd_parent, SW_RESTORE);
			ShowWindow(hwnd_parent, SW_SHOW);

			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			
			break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}



LRESULT CALLBACK histogramProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_parent;
	BOOL ret;
	HDC hdc, hdc_tmp, hdc_aa;
	PAINTSTRUCT ps, ps2;
	//RECT r;
	int i, j;
	HPEN pen_solid1, pen_solid2, holdPen1, holdPen2;
	RECT rc;
	int ssaa_scale;
	HBITMAP bmp;
	HBITMAP bmp_tmp;
	BITMAPINFO bi_aa = {0}, bi_tmp = {0};
	LPCOLORREF pbits, pbits_tmp;
	COLORREF cr;
	COLORREF cr1, cr2, cr3, cr4;
	BYTE red, green, blue;
	int count;
	HPEN hPen_solid, hPen_noborder;
	HBRUSH hBrush_green;

	switch (msg) {
		case WM_CREATE:

			//InvalidateRect(hwnd, NULL, FALSE);

			break;
		case WM_PAINT:
			//GetClientRect(hwnd, &r);

			hdc = BeginPaint(hwnd, &ps);
			//ARRAYSIZE();
			SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
			SelectObject(hdc, hfont1);

			TextOutW(hdc, 50, 20, L"test", 4);


			hPen_solid = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			SelectObject(hdc, hPen_solid);

			MoveToEx(hdc, 100, 200, NULL);
			LineTo(hdc, 250, 200);

			MoveToEx(hdc, 100, 200, NULL);
			LineTo(hdc, 100, 50);


			// If a PS_NULL pen is used, the dimensions of the rectangle are 1 pixel less in height and 1 pixel less in width.
			hPen_noborder = CreatePen(PS_NULL, 1, RGB(0, 0, 0));	// no border
			hBrush_green = CreateSolidBrush(RGB(9, 189, 21));

			SelectObject(hdc, hPen_noborder);
			SelectObject(hdc, hBrush_green);

			//Rectangle(hdc, 100+1, 50+1, 250+1, 200+1);
			Rectangle(hdc, 120+1, 50+1, 150+1, 200+1);		
			


			DeleteDC(hdc);

			break;
		case WM_CLOSE:
			
			hwnd_parent = GetWindow(hwnd, GW_OWNER);
			ret = EnableWindow(hwnd_parent, TRUE);
			if (ret == FALSE) MessageBoxW(NULL, L"GetWindow() FAIL", L"DEBUG", MB_OK);
			BringWindowToTop(hwnd_parent);		// show parent window

			ShowWindow(hwnd_parent, SW_RESTORE);
			ShowWindow(hwnd_parent, SW_SHOW);

			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			
			break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK DialogProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int ctrlID;
	int requestID;
	int position;


	switch (msg) {
		case WM_CREATE:
			// default Height: Edit 21, Button 25, Static 13
			hDialogLabel = CreateWindowW(L"STATIC", L"This is Dialog", 
				WS_CHILD | WS_VISIBLE | SS_LEFT, 
				20, 120, 150, 13, hwnd, (HMENU) 500, NULL, NULL);
			
			toolbar1 = BuildToolBar(hwnd);
			
			// add menu to Dialog
			hMenubar2 = CreateMenu();
			hMenu2 = CreateMenu();
			AppendMenuW(hMenu2, MF_STRING, 1074, L"&Open");
			AppendMenuW(hMenu2, MF_STRING, 1075, L"&Save As");
			AppendMenuW(hMenubar2, MF_POPUP, (UINT_PTR) hMenu2, L"&File");
			SetMenu(hwnd, hMenubar2);

			rebar1 = CreateRebar(hwnd, toolbar1);

			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				
			}

			if (HIWORD(wParam) == BN_CLICKED) {
				switch (LOWORD(wParam)) {
					
				}
			}

			if (HIWORD(wParam) == CBN_SELCHANGE) {

			}

			break;
		case WM_SETFONT:
			//MessageBoxW(NULL, L"Font set", L"DEBUG", MB_OK);
			break;
		case WM_HSCROLL:
			ctrlID = GetDlgCtrlID((HWND) lParam);
			position = SendMessageW((HWND) lParam, TBM_GETPOS, 0, 0);
			requestID = LOWORD(wParam);
			switch (ctrlID) {

			}
			
			break;
		case WM_NOTIFY:
			break;
		case WM_CLOSE:
			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyWindow(hwnd);
			break; 
		case WM_DESTROY:
			break;
	}
	
	
	return DefWindowProcW(hwnd, msg, wParam, lParam);
	
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
	ChooseColorW(&cc);

	return cc.rgbResult;
}


void RegisterPanel () {
	WNDCLASS rwc = {0};
	rwc.lpszClassName = L"Panel";
	rwc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	rwc.lpfnWndProc = PanelProc;
	RegisterClassW(&rwc);
}

LRESULT CALLBACK PanelProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps; 
	RECT rect;

	switch (msg) {
		case WM_PAINT:
			GetClientRect(hwnd, &rect);
			hdc = BeginPaint(hwnd, &ps);
			SetBkColor(hdc, gColor);
			ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &rect, L"", 0, NULL);
			EndPaint(hwnd, &ps);
			break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
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
	LPBYTE lpsBuffer;
	WCHAR text_buf[5000] = {0};		/* BUG: buffer overflow ! */
	//PWCHAR text_buf;					/* bug fix ! Global variable */
	
	hFile = CreateFileW((LPCWSTR) file, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	dwSize = GetFileSize(hFile, NULL);
	lpsBuffer = (LPBYTE) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, dwSize);
	ReadFile(hFile, lpsBuffer, dwSize, &dw, NULL);
	CloseHandle(hFile);

	//text_buf = (LPBYTE) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, dwSize);
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH) lpsBuffer, dwSize, text_buf, dwSize);

	SendMessageW(ghwndEdit, WM_SETTEXT, 0, (LPARAM) text_buf);		// BUG: buffer overflow, replace lpsBuffer with L"abcd text"

	HeapFree(GetProcessHeap(), 0, lpsBuffer);
	//HeapFree(GetProcessHeap(), 0, text_buf);
}


void CreateMyTooltip (HWND hwnd) {
	HWND hwndTT;
	TOOLINFO ti;
	RECT rect;
	WCHAR tooltip[30] = L"A main window";

	hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
		0, 0, 0, 0, hwnd, NULL, NULL, NULL );

	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	 
	GetClientRect(hwnd, &rect);

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
	HWND hLeftLabel, hRightLabel;
	
	hLeftLabel = CreateWindowW(L"STATIC", L"0", 
	WS_CHILD | WS_VISIBLE, 0, 0, 10, 30, hwnd, (HMENU)1, NULL, NULL);

	hRightLabel = CreateWindowW(L"STATIC", L"100", 
	WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU)2, NULL, NULL);

	hTrack = CreateWindowW(L"msctls_trackbar32", L"Trackbar Control",
	WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ,
	20, 150, 170, 30, hwnd, (HMENU) 3001, NULL, NULL);

	SendMessageW(hTrack, TBM_SETRANGE,TRUE, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0,10);
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPOS, TRUE, 50);
	SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel);
	SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel);
}

void UpdateTrackBar (HWND hTrackBar) {
	LRESULT pos = SendMessageW(hTrackBar, TBM_GETPOS, 0, 0);
	wsprintf(s_buf, L"%ld", pos);			// Win32 API
	//swprintf(s_buf, L"%ld", pos);			// ANSI C
	SendMessageW(hDebugLabel, WM_SETTEXT, 0, (LPARAM) s_buf);
}


HWND BuildToolBar (HWND hwnd) {
	HWND hToolBar;
	TBBUTTON tbb[4];
	TBADDBITMAP tbab;
	WCHAR wcsString[] = L"\u9580\u961c\u9640\u963f\u963b\u9644";

	// TOOLBARCLASSNAME or "ToolBarWindow32"
	hToolBar = CreateWindowW(L"ToolBarWindow32", NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS |
		TBSTYLE_FLAT | CCS_TOP | BTNS_AUTOSIZE | TBSTYLE_TRANSPARENT, 
		20, 0, 0, 0, hwnd, (HMENU) 7711, GetModuleHandle(NULL), NULL);
	SendMessageW(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
	SendMessageW(hToolBar, TB_SETEXTENDEDSTYLE, 0, (LPARAM) TBSTYLE_EX_HIDECLIPPEDBUTTONS);

	tbab.hInst = NULL;	//HINST_COMMCTRL;
	tbab.nID = (UINT_PTR) hBitmap;	//IDB_STD_SMALL_COLOR;

	SendMessageW(hToolBar, TB_ADDBITMAP, 0, (LPARAM) &tbab);

	ZeroMemory(tbb, sizeof(tbb));
	
	tbb[0].iBitmap = 0;		// bitmap index
	tbb[0].iString = L"New";
	tbb[0].fsState = TBSTATE_ENABLED;
	tbb[0].fsStyle = TBSTYLE_BUTTON;
	tbb[0].idCommand = IDM_FILE_NEW;

	tbb[1].iBitmap = 0;		// bitmap index
	tbb[1].iString = L"Open";
	tbb[1].fsState = TBSTATE_ENABLED;
	tbb[1].fsStyle = TBSTYLE_BUTTON;
	tbb[1].idCommand = IDM_FILE_OPEN;

	tbb[2].iBitmap = 0;		// bitmap index
	tbb[2].iString = L"Separator";
	tbb[2].fsState = TBSTATE_ENABLED;
	tbb[2].fsStyle = TBSTYLE_SEP;		// Separator
	tbb[2].idCommand = IDM_FILE_QUIT;

	tbb[3].iBitmap = 0;		// bitmap index
	tbb[3].iString = wcsString;
	tbb[3].fsState = TBSTATE_ENABLED;
	tbb[3].fsStyle = TBSTYLE_BUTTON;
	tbb[3].idCommand = IDM_FILE_QUIT;

	SendMessageW(hToolBar, TB_SETBUTTONSIZE, (WPARAM)0, (LPARAM)MAKELONG(24, 22));
	SendMessageW(hToolBar, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM) &tbb);
	SendMessageW(hToolBar, TB_AUTOSIZE, 0, 0);

	//SendMessageW(hToolBar, TB_SETMAXTEXTROWS, 0, 0);	// ShowToolTip

	return hToolBar;
}


HWND CreateRebar (HWND hwnd_parent, HWND hwnd_target) {
	REBARINFO rbi;
	REBARBANDINFO rbBand;
	RECT rc;
	HWND hwndRB;
	//DWORD dwBtnSize;

	// REBARCLASSNAME or "ReBarWindow32"
	hwndRB = CreateWindowExW(WS_EX_TOOLWINDOW, L"ReBarWindow32", NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_VARHEIGHT | CCS_NODIVIDER | RBS_AUTOSIZE | RBS_REGISTERDROP | RBS_VERTICALGRIPPER,
		0, 0, 0, 0, hwnd_parent, NULL, GetModuleHandleW(NULL), NULL);

	if (!hwndRB)
		return NULL;
	// Initialize and send the REBARINFO structure.
	rbi.cbSize = sizeof(REBARINFO);  // Required when using this
	// structure.
	rbi.fMask = 0;
	rbi.himl = (HIMAGELIST) NULL;
	if (!SendMessageW(hwndRB, RB_SETBARINFO, 0, (LPARAM) &rbi))
		return NULL;
	// Initialize structure members that both bands will share.
	rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
	rbBand.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE;
		/*RBBIM_COLORS | RBBIM_TEXT | 
		RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE |
		RBBIM_SIZE;*/
	rbBand.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP;
	//rbBand.hbmBack = NULL;	// bitmap background, set to NULL to get gradient background
	// Create the combo box control to be added.

	// Set values unique to the band with the combo box.
	GetWindowRect(hwnd_target, &rc);
	rbBand.lpText = L"target";
	rbBand.hwndChild = hwnd_target;
	rbBand.cxMinChild = rc.right - rc.left;
	rbBand.cyMinChild = rc.bottom - rc.top;
	rbBand.cx = 200;

	// Add the band that has the combo box.
	SendMessageW(hwndRB, RB_INSERTBAND, (WPARAM)-1, (LPARAM) &rbBand);

	return (hwndRB);
}


HTREEITEM AddItemToTree (HWND hwndTV, LPCWSTR lpszItem, int nLevel) {
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hPrev = (HTREEITEM) TVI_FIRST;
	static HTREEITEM hPrevRootItem = NULL;
	static HTREEITEM hPrevLev2Item = NULL;
	HTREEITEM hti;

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Set the text of the item.
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Assume the item is not a parent item, so give it a 
	// document image. 
	tvi.iImage = NULL;
	tvi.iSelectedImage = NULL;

	// Save the heading level in the item's application-defined 
	// data area. 
	tvi.lParam = (LPARAM) nLevel;
	tvins.item = tvi;
	tvins.hInsertAfter = hPrev;

	// Set the parent item based on the specified level. 
	if (nLevel == 1)
		tvins.hParent = TVI_ROOT;
	else if (nLevel == 2)
		tvins.hParent = hPrevRootItem;
	else
		tvins.hParent = hPrevLev2Item;

	// Add the item to the tree-view control. 
	hPrev = SendMessageW(hwndTV, TVM_INSERTITEM, 0, &tvins);

	// Save the handle to the item. 
	if (nLevel == 1)
		hPrevRootItem = hPrev;
	else if (nLevel == 2)
		hPrevLev2Item = hPrev;

	// The new item is a child item. Give the parent item a 
	// closed folder bitmap to indicate it now has child items. 
	if (nLevel > 1) {
		hti = TreeView_GetParent(hwndTV, hPrev);
		tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvi.hItem = hti;
		tvi.iImage = NULL;
		tvi.iSelectedImage = NULL;
		SendMessageW(hwndTV, TVM_SETITEM, 0, &tvi);
	}

	return hPrev;
}

void CenterWindow (HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	/*SetWindowPos(hwnd, 0,
		(1366 - ),
		500,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);*/
	SetWindowPos(hwnd, 0,
		(GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);
}







/*
CommandLineToArgvW()

*/

// MessageBoxW(NULL, L"First Program", L"DEBUG", MB_OK);

