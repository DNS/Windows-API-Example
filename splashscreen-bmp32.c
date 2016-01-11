
/*

UpdateLayeredWindow() expect the BMP file need to be 32-bit BGRA 
(with Premultiplied Alpha & Down-Top (or Bottom-Up) row order).

Adobe Photoshop doesn't support BMP with premultiplied alpha.
Use Pixelformer to convert PNG with alpha to BMP with premultiplied alpha.

The transparent "TEST 123" text will be click-trough to the background program.

babymetal-july-babymetal-2014-promo-636-transparent.bmp

*/



#undef UNICODE

#include <windows.h>
//#include <commctrl.h>


//#pragma comment(lib, "Comctl32.lib")


HBITMAP hBitmap1;

LRESULT CALLBACK SplashProc (HWND, UINT, WPARAM, LPARAM);
void CenterWindow (HWND hwnd);


INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEX wc = {0};
	//INITCOMMONCONTROLSEX iccex;
	LONG style;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc = SplashProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Window";
	wc.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(255,255,255));	// make something different
	wc.hCursor = LoadCursorA(NULL, IDC_ARROW);	// Note: LoadCursor() superseded by LoadImage()
	wc.hIcon = NULL;
	wc.hIconSm = NULL;

	//iccex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	//iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	////InitCommonControls();				// obsolete
	//InitCommonControlsEx(&iccex);

	if (!RegisterClassExA(&wc)) {
		MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	
	// create main window
	hwnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST,
		wc.lpszClassName, "Title", 
		WS_VISIBLE |  WS_POPUP, 
		0, 0, 636, 380, (HWND) NULL, (HMENU) NULL, hInstance, NULL);
	
	CenterWindow(hwnd);


	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessageA(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		TranslateMessage(&msg);		/* Translate key codes to chars if present */
		DispatchMessageA(&msg);		/* Send it to WndProc */
	}

	return (int) msg.wParam;
}



LRESULT CALLBACK SplashProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	SIZE sizeSplash = {636, 380};	// size of the image
	HDC hdcScreen, hdcMem;
	HBITMAP hbmpOld;
	BLENDFUNCTION blend = {0};
	POINT ptZero = {0, 0};
	POINT ptOrigin = {0, 0};

	switch (msg) {
		case WM_CREATE:
			
			hBitmap1 = LoadImageA(NULL, "babymetal-july-babymetal-2014-promo-636-transparent.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			
			
			blend.BlendOp = AC_SRC_OVER;
			blend.BlendFlags = 0;
			blend.SourceConstantAlpha = 255;
			blend.AlphaFormat = AC_SRC_ALPHA;	// 0, AC_SRC_ALPHA

			hdcScreen = GetDC(NULL);
			hdcMem = CreateCompatibleDC(hdcScreen);
			hbmpOld = SelectObject(hdcMem, hBitmap1);


			UpdateLayeredWindow(hwnd, hdcScreen, &ptOrigin, &sizeSplash, hdcMem, &ptZero, 0, &blend, ULW_ALPHA);

			ReleaseDC(hwnd, hdcScreen);
			DeleteDC(hdcMem);
			
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				
			}	// switch(LOWORD(wParam))

			break;
		case WM_PAINT:
			break;
		case WM_TIMER:

			break;
		case WM_CLOSE:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		return 0;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}


void CenterWindow (HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	SetWindowPos(hwnd, 0,
		(GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

