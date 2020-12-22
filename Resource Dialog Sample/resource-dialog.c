/*

Modal Dialog: DialogBox() -> Only focus on it's own window. Can't focus on parent window. 
	eg: Notepad.exe [Menu: Help -> About]. "About" child window is modal dialog.

Modeless Dialog: CreateDialog() -> Can change focus on parent window. 
	eg: Notepad.exe [Ctrl+F]. "Find" child window is modeless dialog.



*/
#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Comctl32.lib")
//#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Winmm.lib")



/* force MSVC to use ANSI/WideChar function, must be before #include <windows.h> */
//#define UNICODE
#undef UNICODE

#include <windows.h>
#include <commctrl.h>
//#include <ole2.h>
#include <Mmsystem.h>

#include "resource.h"


HWND button1;
HFONT hfont1;
HMENU menu1;

INT WINAPI WinMain (HINSTANCE, HINSTANCE, PSTR, INT);
LRESULT CALLBACK WindowProc (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);

VOID RegisterCustomControl ();
LRESULT CALLBACK CustomControlProc (HWND, UINT, WPARAM, LPARAM);


INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEXA wc = {0};		// initialize with NULL / 0
	INITCOMMONCONTROLSEX iccex;
	
	mciSendStringA("open \"left 4 dead witch crying.mp3\" alias MySound", NULL, 0, NULL);
	mciSendStringA("play MySound", NULL, 0, NULL);

	ZeroMemory(&wc, sizeof(wc));	// initialize with NULL (other alternative)
	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;// MAKEINTRESOURCE(IDR_MENU1);	// load menu automatically from resource
	wc.lpszClassName = "Window";
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;	// default window color
	wc.hCursor = LoadCursorA(NULL, IDC_ARROW);	// Note: LoadCursor() superseded by LoadImage()
	wc.hIcon = (HICON) LoadImageA(hInstance, MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
	wc.hIconSm = (HICON) LoadImageA(hInstance, MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
	
	iccex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);

	InitCommonControlsEx(&iccex);

	RegisterCustomControl();

	if (!RegisterClassExA(&wc)) {
		MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	// create main window
	hwnd = CreateWindowExA(WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT,
		wc.lpszClassName, "Title",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_TABSTOP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		300, 250, (HWND) NULL, (HMENU) NULL, hInstance, NULL);

	while (GetMessageA(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		TranslateMessage(&msg);		/* Translate key codes to chars if present */
		DispatchMessageA(&msg);		/* Send it to WindowProc */
	}


	UnregisterClassA("CustomControl1", NULL);	// not necessary if WNDCLASSEXA use automatic variable
	
	return msg.wParam;
}


BOOL CALLBACK DestroyChildWindow (HWND hwnd, LPARAM lParam) {
	DestroyWindow(hwnd);
	//SendMessageA(hwnd, WM_DESTROY, 0, 0);
	return TRUE;
}


LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			hfont1 = CreateFontW(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Tahoma");

			button1 = CreateWindowA("BUTTON", "Show Dialog from resource", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
				60, 80, 150, 25, hwnd, (HMENU) 600, NULL, NULL);

			SendMessageA(button1, WM_SETFONT, (WPARAM) hfont1, TRUE);

			// load menu manually from resource
			menu1 = LoadMenuA(NULL, MAKEINTRESOURCEA(IDR_MENU1));
			SetMenu(hwnd, menu1);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 600:
					DialogBoxA(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
					
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
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_SETFOCUS:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyMenu(menu1);
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
			return 0;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}



INT_PTR CALLBACK DialogProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND listview1;
	HWND radiobtn4;
	LVCOLUMN lvc;
	LVITEM lvi;
	int i, j;
	typedef struct _SHINOBI {
		char nama[30];
		char clan[60];
	} SHINOBI;
	SHINOBI shinobi[] = {
		{"Obito", "Uchiha"},
		{"Madara", "Uchiha"},
		{"Neji", "Hyuga"},
		{"Naruto", "Uzumaki"},
		{"Sasuke", "Uchiha"},
		{"Shino", "Aburame"},
		{"Hinata", "Hyuga"},
		{"Shikamaru", "Nara"},
		{"Choji", "Akimichi"},
		{"Ino", "Yamanaka"},
		{"Kimimaro", "Kaguya"},
		{"Nagato", "Uzumaki"},
		{"Kiba", "Inuzuka"},
		{"Hiruzen", "Sarutobi"}
	};
	
	switch (msg) {
		case WM_INITDIALOG:

			// fetch radio button, & set checked state
			radiobtn4 = GetDlgItem(hwnd, IDC_RADIO4);
			SendMessageW(radiobtn4, BM_SETCHECK, BST_CHECKED, TRUE);

			// fetch listview
			listview1 = GetDlgItem(hwnd, IDC_LIST1);

			// create listview column
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.fmt = LVCFMT_LEFT;
			lvc.cx = 70;	// width of column in pixels

			lvc.iSubItem = 0;
			lvc.pszText = "Nama";
			SendMessageA(listview1, LVM_INSERTCOLUMN, 0, &lvc);

			lvc.iSubItem = 1;
			lvc.pszText = "Clan";
			SendMessageA(listview1, LVM_INSERTCOLUMN, 1, &lvc);


			// insert listview data
			lvi.mask = LVIF_TEXT;

			for (i = 0; i < 14; i++) {
				lvi.iItem = i;
				lvi.iSubItem = 0;
				lvi.pszText = shinobi[i].nama;
				SendMessageA(listview1, LVM_INSERTITEM, 0, &lvi);

				lvi.iItem = i;
				lvi.iSubItem = 1;
				lvi.pszText = shinobi[i].clan;
				SendMessageA(listview1, LVM_SETITEM, 0, &lvi);
			}
			 
			//SendMessageA(listview1, LVS_EDITLABELS, 0, 0);
			SendMessageA(listview1, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
			//SendMessageA(listview1, LVM_GETNEXTITEM, 0, );
	

			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_FILE_EXIT:
					EndDialog(hwnd, 0);
					break;
				case IDOK:
					MessageBoxA(NULL, "clicked", "DEBUG", MB_OK);
					break;
				case IDQUIT:
					EndDialog(hwnd, 0);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
		default:
			return FALSE;	// MUST BE SET TO FALSE, dialog box procedure never calls the DefWindowProc function
	}

	
	//return DefDlgProcW(hwnd, msg, wParam, lParam);
	return FALSE;	// MUST BE SET TO FALSE, dialog box procedure never calls the DefWindowProc function
}


VOID RegisterCustomControl () {
	WNDCLASSEXA wc = {0};		// initialize with NULL / 0

	ZeroMemory(&wc, sizeof(wc));	// initialize with NULL (other alternative)
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = CustomControlProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandleA(NULL);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "CustomControl1";
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;	// default window color
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL;
	wc.hIconSm = NULL;

	if (!RegisterClassExA(&wc)) {
		MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

}

LRESULT CALLBACK CustomControlProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			//MessageBoxA(NULL, "CustomControlProc created", "DEBUG", MB_OK);
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rect;

			GetClientRect(hwnd, &rect);

			hdc = BeginPaint(hwnd, &ps);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			DrawTextA(hdc, "Custom Control\nTest 123", -1, &rect, DT_LEFT);
			EndPaint(hwnd, &ps);
		}
			break;
	}



}

