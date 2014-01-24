
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
HMENU hMenubar1, hMenu1, submenu1, hMenubar2, hMenu2, hPopUp1;
HINSTANCE ghInstance;
HWND ghwndEdit, staticimage1;
HWND hEdit , hLabel, button1, button2, checkbox1;
HWND radiobtn1, radiobtn2, radiobtn3;
HWND hProgressBar, treeview1;
HFONT hfont1, hfont2, hfont3;
HBITMAP hBitmap, kurtd3_bitmap;
NONCLIENTMETRICS ncm;
HWND hTrack;
UINT hTrack_id;
HWND hDebugLabel;
HWND hMonthCal, hCombo, groupbox1;
HWND hDlgCurrent = NULL;
HWND hTab, listbox1;
HWND rebar1, toolbar1;
HANDLE hImg;
HMODULE hmod;


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ControlProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PanelProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DestroyChildWindow(HWND, LPARAM);

void CreateDialogBox (HWND);
void RegisterDialogClass (HWND);
void RegisterPanel ();
COLORREF ShowColorDialog (HWND);
void CreateMenubar (HWND);
void OpenDialog (HWND);
void LoadFile_internal (LPCWSTR);
void CreateMyTooltip (HWND);
void CreateTrackBar (HWND);
void UpdateTrackBar();
void AddMenus (HWND);
HWND BuildToolBar (HWND);
HWND CreateRebar (HWND, HWND);
HTREEITEM AddItemToTree(HWND, LPSTR, int);
void CenterWindow(HWND);

WCHAR s_buf[500];



COLORREF gColor = RGB(255, 255, 255);

INT WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEX wc;
	INITCOMMONCONTROLSEX iccex;
	ACCEL accel[2];
	HACCEL hAccel;
	
	//memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra= 0;
	wc.cbWndExtra= 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName= NULL;
	wc.lpszClassName = L"Window";
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;	// default window color
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
	
	
	//ShowWindow(hwnd, nCmdShow);
	//UpdateWindow(hwnd);

	// Create Keyboard Accelerator/Shorcut
	accel[0].fVirt = FCONTROL | FVIRTKEY;
	accel[0].key = 'A';			// must be uppercase
	accel[0].cmd = 9101;		// msg code to send to WM_COMMAND
	accel[1].fVirt = FCONTROL | FVIRTKEY;
	accel[1].key = 'C';			// must be uppercase
	accel[1].cmd = 9102;		// msg code to send to WM_COMMAND
	hAccel = CreateAcceleratorTableW(accel, 2);
	
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		if (!IsDialogMessageW(hDlgCurrent, &msg)) {
			if (!TranslateAcceleratorW(hwnd, hAccel, &msg)) {	/* Handle Keyboard shortcut */
				TranslateMessage(&msg);		/* Translate key codes to chars if present */
				DispatchMessageW(&msg);		/* Send it to WndProc */
			}
		}
	}

	return (int) msg.wParam;
}


BOOL CALLBACK DestroyChildWindow(HWND hwnd, LPARAM lParam) {
	DestroyWindow(hwnd);
	SendMessageW(hwnd, WM_DESTROY, 0, 0);
	return TRUE;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_control, hwnd_dialog;
	POINT point;
	UINT state;
	static HWND hwndPanel;
	RECT rectParent;
	WNDCLASSEX wc = {0};

	switch (msg) {
		case WM_CREATE:
			CenterWindow(hwnd);
			hBitmap = (HBITMAP) LoadImageW(NULL, L"test123.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			AddMenus(hwnd);
			
			
			//ghSb = CreateStatusWindowW(WS_CHILD | WS_VISIBLE, L"Status bar title", hwnd, 5003);	// obsolete
			
			// STATUSCLASSNAME or "msctls_statusbar32"
			ghSb = CreateWindowW(L"msctls_statusbar32", L"new Status bar title", 
				WS_VISIBLE | WS_CHILD | WS_BORDER | SBARS_SIZEGRIP | CCS_BOTTOM,
				0, 0, 0, 0, hwnd, (HMENU) 5003, NULL, NULL );
			ShowWindow(ghSb, SW_HIDE);

			BuildToolBar(hwnd);
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
			SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			hfont2 = CreateFontIndirectW(&ncm.lfMessageFont);
			
			ghwndEdit = CreateWindowW(L"EDIT", L"abcd",
				WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_CLIPCHILDREN,
				50, 50, 260, 180, hwnd, NULL, NULL, NULL);
			
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
					wc.hInstance = GetModuleHandleW(NULL);
					wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc.lpszClassName = L"DialogClass";
					RegisterClassExW(&wc);
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
					//RegisterDialogClass(hwnd);
					memset(&wc, 0, sizeof(wc));
					wc.cbSize = sizeof(WNDCLASSEX);
					wc.lpfnWndProc = (WNDPROC) ControlProc;
					wc.hInstance = ghInstance;
					wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
					wc.lpszClassName = L"ControlClass";
					RegisterClassExW(&wc);
					
					hwnd_control = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT, L"ControlClass", 
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
					state = GetMenuState(hMenu1, IDM_VIEW_STB, MF_BYCOMMAND);
					if (state == SW_SHOWNA) {
						ShowWindow(ghSb, SW_HIDE);
						CheckMenuItem(hMenu1, IDM_VIEW_STB, MF_UNCHECKED);
					} else {
						ShowWindow(ghSb, SW_SHOWNA);
						CheckMenuItem(hMenu1, IDM_VIEW_STB, MF_CHECKED);
					}
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
	MENUITEMINFO mii;

	submenu1 = CreateMenu();
	hMenubar1 = CreateMenu();
	hMenu1 = CreateMenu();

	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_NEW, L"&New\tCtrl+N");
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_OPEN, L"&Open\tCtrl+C+O");

	AppendMenuW(hMenu1, MF_STRING, IDM_VIEW_STB, L"&Statusbar");
	CheckMenuItem(hMenu1, IDM_VIEW_STB, MF_UNCHECKED);	// MF_CHECKED, MF_UNCHECKED

	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_DIALOG, L"&Dialog, ToolBar, && Rebar");
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_COLOR, L"&Color");

	AppendMenuW(submenu1, MF_STRING, 5553, L"&from AppendMenuW()\tCtrl+M");
	
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.fType = MFT_STRING | MFT_RIGHTJUSTIFY | MFT_RIGHTORDER;
	//mii.fState = ;
	//mii.wID = 5553;
	mii.hSubMenu = submenu1;
	//mii.hbmpChecked = ;
	//mii.hbmpUnchecked = ;
	//mii.dwItemData = ;
	mii.dwTypeData = L"from InsertMenuItemW()\tCtrl+Shit+I";
	//mii.cch = ;
	//mii.hbmpItem = ;

	InsertMenuItemW(submenu1, 0, FALSE, &mii);	// TRUE to set this menu to 0 position index (first)



	AppendMenuW(hMenu1, MF_POPUP, (UINT_PTR) submenu1, L"&Submenu1");

	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_CONTROL, L"&Control");

	AppendMenuW(hMenu1, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu1, MF_STRING, IDM_FILE_QUIT, L"&Quit");

	
	AppendMenuW(hMenubar1, MF_POPUP, (UINT_PTR) hMenu1, L"&File");

	
	SetMenuItemBitmaps(hMenu1, IDM_FILE_OPEN, MF_BITMAP | MF_BYCOMMAND, hBitmap, hBitmap);


	SetMenu(hwnd, hMenubar1);
}


LRESULT CALLBACK ControlProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_parent;
	UINT checked;
	int ctrlID, requestID, position;
	LPNMHDR lpNmHdr;
	SYSTEMTIME time;
	int i = 0;
	BOOL ret;
	TCITEMW tabItem1, tabItem2;
	PAINTSTRUCT ps;
	RECT r;
	HDC hdc;
	WCHAR os_list[5][32] = {L"MSDOS", L"Windows 98 SE", L"Windows ME", L"Windows XP", L"Windows 7"};
	WCHAR os_other[6][32] = {L"UNIX", L"Linux", L"BSD", L"Plan 9", L"Mac OS X", L"OS/2 WARP"};

	//IsDialogMessageW(hwnd, (LPMSG) &msg);
	switch (msg) {
		case WM_CREATE:
			// window class
			// macro: WC_STATIC, WC_BUTTON, WC_EDIT, WC_COMBOBOX, WC_SCROLLBAR, WC_LISTBOX
			// value: "STATIC" "BUTTON" "EDIT" "COMBOBOX" "SCROLLBAR" "LISTBOX"

			// default Height: Edit 21, Button 25, Static 13, CheckBox 17 
			hLabel = CreateWindowW(L"STATIC", L"This is Label", 
				WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP, 
				20, 80, 150, 13, hwnd, (HMENU) 500, NULL, NULL);

			button1 = CreateWindowW(L"BUTTON", L"&Button", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
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

			hDebugLabel = CreateWindowW(L"STATIC", L"Debug Label !!!", WS_CHILD | WS_VISIBLE, 
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

			SendMessageW(radiobtn2, BM_SETCHECK, BST_CHECKED, TRUE);		// set default value to radiobtn2

			// ComboBox: CBS_DROPDOWN or CBS_DROPDOWNLIST, msg CB_SETCURSEL, CB_GETCURSEL
			hCombo = CreateWindowW(TEXT("COMBOBOX"), NULL,
				WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
				410, 20, 120, 110, hwnd, NULL, NULL, NULL);

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
			hTab = CreateWindowW(L"SysTabControl32", NULL, WS_CHILD | WS_VISIBLE,
				400, 70, 150, 150, hwnd, (HMENU) 2444, NULL, NULL);

			tabItem1.mask = TCIF_TEXT;
			tabItem1.pszText = L"Tab 1";

			tabItem2.mask = TCIF_TEXT;
			tabItem2.pszText = L"Tab 2";

			SendMessageW(hTab, TCM_INSERTITEM, 0, (LPARAM) &tabItem1);
			SendMessageW(hTab, TCM_INSERTITEM, 1, (LPARAM) &tabItem2);


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
			SendMessageW(listbox1, WM_SETFONT, (WPARAM) hfont1, TRUE);

			// WC_TREEVIEW or "SysTreeView32"
			treeview1 = CreateWindowExW(WS_EX_CLIENTEDGE, L"SysTreeView32", NULL, 
				WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT, 
				400, 230, 150, 100, hwnd, (HMENU) 7265, NULL, NULL);

			AddItemToTree(treeview1, L"treeview1 test", 0);
			AddItemToTree(treeview1, L"child", 1);

			// STATIC Image
			staticimage1 = CreateWindowW(L"STATIC", L"This is Label", 
				WS_CHILD | WS_VISIBLE | SS_BITMAP,
				560, 50, 100, 100, hwnd, (HMENU) 9524, NULL, NULL);
			// LoadImage(): 0 -> actual resource size,  LR_DEFAULTSIZE -> fit to parent
			kurtd3_bitmap = LoadImageW(NULL, L"kurtd3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			SendMessageW(staticimage1, STM_SETIMAGE, IMAGE_BITMAP, kurtd3_bitmap);



			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 600:
					GetWindowTextW(hEdit, s_buf, 50);
					SetWindowTextW(hLabel, s_buf);
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
				case 3555:
					i = SendMessageW(listbox1, LB_GETCURSEL, 0, 0);

					SendMessageW(listbox1, LB_GETTEXT, i, (LPARAM) &s_buf);	// fetch from listbox1 Control (much better)
					//wcscpy(s_buf, os_other[i]);	// fetch from local sBuffer
					
					SetWindowTextW(hDebugLabel, s_buf);
					break;
			}


			// GroupBox-RadioButton msg
			if (HIWORD(wParam) == BN_CLICKED) {
				switch (LOWORD(wParam)) {
					case 6001:
						wcscpy(s_buf, L"blue");
						SetWindowTextW(hDebugLabel, s_buf);
						break;
					case 6002:
						wcscpy(s_buf, L"yellow");
						SetWindowTextW(hDebugLabel, s_buf);
						break;
					case 6003:
						wcscpy(s_buf, L"orange");
						SetWindowTextW(hDebugLabel, s_buf);
						break;
				}
			}

			// ComboBox msg
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				i = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
				SendMessageW(hCombo, CB_GETLBTEXT, i, (LPARAM) s_buf);
				
				SetWindowTextW(hDebugLabel, s_buf);			// fetch from hCombo (much better)
				//SetWindowTextW(hDebugLabel, os_list[i]);	// fetch from local sBuffer
				
				SetFocus(hwnd);
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
				swprintf(s_buf, 400, L"%d-%d-%d", time.wYear, time.wMonth, time.wDay);
				SetWindowTextW(hDebugLabel, s_buf);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);

			GetClientRect(hwnd, &r);

			for (i=0; i<1000; i++) {
				int x, y;
				x = (rand() % r.right - r.left);
				y = (rand() % r.bottom - r.top);
				SetPixel(hdc, x, y, RGB(255, 0, 0));
			}

			EndPaint(hwnd, &ps);
			break;
		case WM_CLOSE:
			hwnd_parent = GetWindow(hwnd, GW_OWNER);
			ret = EnableWindow(hwnd_parent, TRUE);
			if (ret == FALSE) MessageBoxW(NULL, L"GetWindow() FAIL", L"DEBUG", MB_OK);
			DestroyWindow(hwnd);
			
			ShowWindow(hwnd_parent, SW_RESTORE);
			ShowWindow(hwnd_parent, SW_SHOW);
			EnumChildWindows(hwnd, DestroyChildWindow, lParam);

			DeleteObject(kurtd3_bitmap);
			break; 
		case WM_DESTROY:
			DeleteObject(hImg);
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
			hLabel = CreateWindowW(L"STATIC", L"This is Label", 
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
	WCHAR text_buf[5000] = {0};		/* BUG: Buffer overflow ! */
	
	hFile = CreateFileW((LPCWSTR) file, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	dwSize = GetFileSize(hFile, NULL);
	lpsBuffer = (LPBYTE) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, dwSize + 1);
	ReadFile(hFile, lpsBuffer, dwSize, &dw, NULL);
	CloseHandle(hFile);

	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)lpsBuffer, dwSize, text_buf, dwSize);
	SetWindowTextW(ghwndEdit, (LPWSTR) text_buf);		// BUG: buffer overflow, replace lpsBuffer with L"abcd text"

	HeapFree(GetProcessHeap(), 0, lpsBuffer);
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
	SendMessageW(hTrack, TBM_SETPOS, FALSE, 50);
	SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel);
	SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel);
}

void UpdateTrackBar (HWND hTrackBar) {
	LRESULT pos = SendMessageW(hTrackBar, TBM_GETPOS, 0, 0);
	wsprintfW(s_buf, L"%ld", pos);

	SetWindowTextW(hDebugLabel, s_buf);
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
	DWORD dwBtnSize;

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
	rbBand.lpText = "target";
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
	tvi.lParam = (LPARAM)nLevel;
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

void CenterWindow(HWND hwnd) {
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

// MessageBoxW(NULL, L"First Program", L"DEBUG", MB_OK);

