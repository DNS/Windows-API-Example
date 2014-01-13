
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


HFONT defaultFont;
HWND ghSb;
HMENU hMenubar;
HMENU hMenu;
HINSTANCE ghInstance;
HWND ghwndEdit;
HWND hEdit , hLabel, button1, button2, checkbox1;
HWND radiobtn1, radiobtn2, radiobtn3;
HWND hProgressBar;
HFONT hfont1, hfont2, hfont3;
HBITMAP hBitmap;
NONCLIENTMETRICS ncm;
HWND hTrack;
UINT hTrack_id;
HWND hDebugLabel;
HWND hMonthCal, hCombo, groupbox1;
HWND hDlgCurrent = NULL;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ControlProc (HWND, UINT, WPARAM, LPARAM);
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


WCHAR buf[500];



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

	hwnd = CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT, wc.lpszClassName, L"Title", 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_TABSTOP, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	
	
	//ShowWindow(hwnd, nCmdShow);
	//UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		if (!IsDialogMessageW(hDlgCurrent, &msg)) {
			TranslateMessage(&msg);		/* Translate key codes to chars if present */
			DispatchMessageW(&msg);		/* Send it to WndProc */
		}
	}

	return (int) msg.wParam;
}


BOOL CALLBACK EnumFunc (HWND hwnd, LPARAM lParam) {
	DestroyWindow(hwnd);
	//SendMessageW(hwnd_tmp, WM_DESTROY, 0, 0);
	//MessageBoxW(NULL, L"First Program", L"First", MB_OK);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_tmp;
	POINT point;
	UINT state;
	static HWND hwndPanel;
	RECT rectParent;
	WNDCLASSEX wc = {0};

	switch (msg) {
		case WM_CREATE:
			AddMenus(hwnd);
			
			
			//ghSb = CreateStatusWindowW(WS_CHILD | WS_VISIBLE, L"Status bar title", hwnd, 5003);	// obsolete
			
			// STATUSCLASSNAME or "msctls_statusbar32"
			ghSb = CreateWindowW(L"msctls_statusbar32", L"new Status bar title", 
				WS_VISIBLE | WS_CHILD | WS_BORDER | SBARS_SIZEGRIP | CCS_BOTTOM,
				0, 0, 0, 0, hwnd, (HMENU) 5003, NULL, NULL );
			ShowWindow(ghSb, SW_HIDE);

			
			CreateMyTooltip(hwnd);
			
			// Delphi: Tahoma 13, .NET: Microsoft Sans Serif 14, System Default: Segoe UI 15
			// get custom font
			hfont1 = CreateFontW(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Tahoma");

			hfont3 = CreateFontW(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
				DEFAULT_PITCH | FF_DONTCARE, L"Comic Sans MS");

			// get system default font (default: Segoe UI 15)
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			hfont2 = CreateFontIndirect(&ncm.lfMessageFont);
			
			ghwndEdit = CreateWindowW(L"EDIT", L"abcd",
				WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_CLIPCHILDREN,
				0, 0, 260, 180, hwnd, NULL, NULL, NULL);
				
			//SendMessageW(ghwndEdit, WM_SETFONT, (WPARAM) hfont1, TRUE);

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
					memset(&wc, 0, sizeof(wc));
					wc.cbSize = sizeof(WNDCLASSEX);
					wc.lpfnWndProc = (WNDPROC) DialogProc;
					wc.hInstance = GetModuleHandle(NULL);
					wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc.lpszClassName = L"DialogClass";
					RegisterClassExW(&wc);
					CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, TEXT("DialogClass"), 
						TEXT("Dialog Box"), WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 200, 150, 
						hwnd, (HMENU) NULL, GetModuleHandle(NULL),NULL);
					break;
				case IDM_FILE_COLOR:
					gColor = ShowColorDialog(hwnd);
					InvalidateRect(hwndPanel, NULL, TRUE);
					break;
				case IDM_FILE_CONTROL:
					//RegisterDialogClass(hwnd);
					memset(&wc, 0, sizeof(wc));
					wc.cbSize = sizeof(WNDCLASSEX);
					wc.lpfnWndProc = (WNDPROC) ControlProc;
					wc.hInstance = ghInstance;
					wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc.lpszClassName = L"ControlClass";
					RegisterClassExW(&wc);
					
					hwnd_tmp = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT, L"ControlClass", 
						L"Dialog Box", WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 
						100, 100, 800, 550, hwnd, (HMENU) NULL, GetModuleHandle(NULL), NULL);

					EnableWindow(hwnd, FALSE);	// make hwnd_tmp modal window


					break;
				case IDM_FILE_OPEN:
					OpenDialog(hwnd);
					break;
				case IDM_FILE_QUIT:
					SendMessageW(hwnd, WM_CLOSE, 0, 0);
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
			//EnumWindows(EnumFunc, lParam);
			DestroyWindow(hwnd);
			break;
		case WM_SYSCOMMAND:
			break;
		case WM_DESTROY:
			DeleteObject(hfont1);
			DeleteObject(hfont2);
			DeleteObject(hfont3);
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


LRESULT CALLBACK ControlProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_parent;
	UINT checked;
	int ctrlID, requestID, position;
	LPNMHDR lpNmHdr;
	SYSTEMTIME time;
	int sel, i = 0;
	BOOL ret;
	WCHAR os_list[5][32] = {L"Windows 98 SE", L"Windows ME", L"Windows XP", L"Windows 7", L"Windows Phone 8"};

	//IsDialogMessageW(hwnd, (LPMSG) &msg);
	switch (msg) {
		case WM_CREATE:
			// macro: WC_STATIC, WC_BUTTON, WC_EDIT, WC_COMBOBOX, WC_SCROLLBAR, WC_LISTBOX
			// string: without WC

			// default Height: Edit 21, Button 25, Static 13, CheckBox 17 
			hLabel = CreateWindowW(L"STATIC", L"This is Label", 
				WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP, 
				20, 80, 150, 13, hwnd, (HMENU) 500, NULL, NULL);
			button1 = CreateWindowW(L"BUTTON", L"&Button", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
				20, 50, 80, 25, hwnd, (HMENU) 600, NULL, NULL);
			button2 = CreateWindowW(L"BUTTON", L"&Quit", WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_DISABLED,
				120, 50, 80, 25, hwnd, (HMENU) 650, NULL, NULL);
			
			checkbox1 = CreateWindowW(L"BUTTON", L"This is &Checkbox", 
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX | WS_TABSTOP, 
				20, 10, 110, 17, hwnd, (HMENU) 700, NULL, NULL);
			// BST_CHECKED / BST_INDETERMINATE / BST_UNCHECKED
			//CheckDlgButton(hwnd, 700, BST_CHECKED);	// same below
			SendMessageW(checkbox1, BM_SETCHECK, BST_CHECKED, 0);

			hDebugLabel = CreateWindowW(L"STATIC", L"Debug Label", WS_CHILD | WS_VISIBLE, 
				550, 20, 200, 13, hwnd, (HMENU) 3111, NULL, NULL);

			// EDIT ctrl: max 32,767 bytes
			hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"This is edit", 
				WS_VISIBLE | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL | ES_NOHIDESEL,
				20, 110, 185, 21, hwnd, (HMENU) 800, NULL, NULL);	// WS_BORDER

			

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
				WS_VISIBLE | WS_CHILD,
				20, 200, 225, 160, hwnd, (HMENU) 4001, NULL, NULL);

			// GroupBox
			groupbox1 = CreateWindowW(TEXT("BUTTON"), TEXT("Choose Color"),
				WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
				260, 20, 120, 150, hwnd, (HMENU) NULL, NULL, NULL);
			radiobtn1 = CreateWindowW(TEXT("BUTTON"), TEXT("Blue"),
				WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,	// WS_GROUP: start exclusive radiobtn
				270, 40, 100, 30, hwnd, (HMENU) 6001, NULL, NULL);
			radiobtn2 = CreateWindowW(TEXT("BUTTON"), TEXT("Yellow"),
				WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,	// WS_GROUP: start another exclusive, until radiobtn3
				270, 65, 100, 30, hwnd, (HMENU) 6002, NULL, NULL);
			radiobtn3 = CreateWindowW(TEXT("BUTTON"), TEXT("Orange"),
				WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
				270, 90, 100, 30, hwnd, (HMENU) 6003, NULL, NULL);

			SendMessage(radiobtn2, BM_SETCHECK, BST_CHECKED, TRUE);		// set default value to radiobtn2

			// ComboBox: CBS_DROPDOWN or CBS_DROPDOWNLIST
			hCombo = CreateWindowW(TEXT("COMBOBOX"), NULL,
				WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
				410, 20, 120, 110, hwnd, NULL, NULL, NULL);
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows 98 SE");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows ME");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows XP");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows 7");
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM) L"Windows Phone 8");

			SendMessageW(hCombo, CB_SETCURSEL, 3, 0);	// set default index for ComboBox
			
			// set font
			SendMessageW(hCombo, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(groupbox1, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(radiobtn1, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(radiobtn2, WM_SETFONT, (WPARAM) hfont2, TRUE);
			SendMessageW(radiobtn3, WM_SETFONT, (WPARAM) hfont2, TRUE);

			// PROGRESS_CLASS or L"msctls_progress32"
			hProgressBar = CreateWindowW(L"msctls_progress32", NULL,
				WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
				270, 200, 190, 25, hwnd, (HMENU) 7301, NULL, NULL);

			SendMessageW(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
			SendMessageW(hProgressBar, PBM_SETSTEP, 1, 0);
			SendMessageW(hProgressBar, PBM_SETPOS, 100, 0);
			
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 600:
					GetWindowTextW(hEdit, buf, 50);
					SetWindowTextW(hLabel, buf);
					Beep(40, 50);

					// change progress bar 
					i = 0;
					SendMessageW(hProgressBar, PBM_SETPOS, 0, 0);
					SetTimer(hwnd, 8866, 0, NULL);
					break;
				case 650:
					SendMessageW(hwnd, WM_CLOSE, 0, 0);
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
					case 6001:
						wcscpy(buf, L"blue");
						SetWindowTextW(hDebugLabel, buf);
						break;
					case 6002:
						wcscpy(buf, L"yellow");
						SetWindowTextW(hDebugLabel, buf);
						break;
					case 6003:
						wcscpy(buf, L"orange");
						SetWindowTextW(hDebugLabel, buf);
						break;
				}
			}

			if (HIWORD(wParam) == CBN_SELCHANGE) {
				sel = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
				SendMessageW(hCombo, CB_GETLBTEXT, sel, (LPARAM) buf);
				
				SetWindowTextW(hDebugLabel, buf);			// fetch from ComboBox Control (much better)
				//SetWindowTextW(hDebugLabel, os_list[sel]);	// from local buffer
				
				SetFocus(hwnd);
			}

			break;
		case WM_ACTIVATE:
			if (0 == wParam)             // becoming inactive
               hDlgCurrent = NULL;
            else                         // becoming active
               hDlgCurrent = hwnd;
			break;
		case WM_SETFOCUS:
			break;
		case WM_SETFONT:
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
			lpNmHdr = (LPNMHDR) lParam;
			if (lpNmHdr->code == MCN_SELECT) {
				ZeroMemory(&time, sizeof(SYSTEMTIME));
				SendMessageW(hMonthCal, MCM_GETCURSEL, 0, (LPARAM) &time);
				swprintf(buf, 400, L"%d-%d-%d", time.wYear, time.wMonth, time.wDay);
				SetWindowTextW(hDebugLabel, buf);
			}
			break;
		case WM_CLOSE:
			hwnd_parent = GetWindow(hwnd, GW_OWNER);
			ret = EnableWindow(hwnd_parent, TRUE);
			
			if (ret == FALSE) MessageBoxW(NULL, L"GetWindow() FAIL", L"First", MB_OK);
			DestroyWindow(hwnd);
			break; 
		case WM_DESTROY:
			break;
	}
	
	
	return DefWindowProcW(hwnd, msg, wParam, lParam);
	
}


LRESULT CALLBACK DialogProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	UINT checked;
	int ctrlID;
	int requestID;
	int position;
	LPNMHDR lpNmHdr;
	SYSTEMTIME time;
	int sel;

	switch (msg) {
		case WM_CREATE:
			// default Height: Edit 21, Button 25, Static 13
			hLabel = CreateWindowW(L"STATIC", L"This is Label", 
				WS_CHILD | WS_VISIBLE | SS_LEFT, 
				20, 20, 150, 13, hwnd, (HMENU) 500, NULL, NULL);
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
			//MessageBoxW(NULL, L"Font set", L"First", MB_OK);
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
	WCHAR ws_buf[5000] = {0};		/* BUG: buffer overflow ! */

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
