/*
	Name        : splashscreen-wic.c
	Author      : Daniel Sirait <dns@cpan.org>
	License     : Public Domain
	Description : Load splash screen (JPEG/PNG/GIF) using Win32 API & 
				  WIC (Windows Imaging Component) via COM interface.
*/


#define UNICODE

#include <windows.h>
#include <commctrl.h>
#include <wincodec.h>
#include <wincodecsdk.h>
#include <objidl.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "windowscodecs.lib")


#define BABYMETAL_SPLASHIMAGE 4


HBITMAP hBitmap1;
HWND staticimage1;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

void CenterWindow (HWND hwnd);

IStream *CreateStreamOnResource (LPCTSTR lpName, LPCTSTR lpType);
IWICBitmapSource *LoadBitmapFromStream (IStream *ipImageStream);
HBITMAP CreateHBITMAP (IWICBitmapSource *ipBitmap);
HBITMAP LoadSplashImage ();


INT WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEX wc;
	INITCOMMONCONTROLSEX iccex;
	ACCEL accel[2];
	HACCEL hAccel;
	LONG style;

	
	//memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra= 0;
	wc.cbWndExtra= 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName= NULL;
	wc.lpszClassName = L"Window";
	//wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);	// default window color
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));	// make something different
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);	// Note: LoadCursor() superseded by LoadImage()
	wc.hIcon = (HICON) LoadImageW(NULL, L"razor.ico", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wc.hIconSm = (HICON) LoadImageW(NULL, L"razor.ico", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);

	iccex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	//InitCommonControls();				// obsolete
	InitCommonControlsEx(&iccex);

	if (!RegisterClassExW(&wc)) {
		MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	
	// create main window
	hwnd = CreateWindowExW(WS_EX_LAYERED, 
		wc.lpszClassName, L"Title", 
		WS_VISIBLE, 
		0, 0, 636, 380, (HWND) NULL, (HMENU) NULL, hInstance, NULL);
	
	CenterWindow(hwnd);

	style = GetWindowLong(hwnd, GWL_STYLE);
	//style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	
	//SetWindowLong(hwnd, GWL_STYLE, style);	// obsolete
	//SetWindowLongPtr(hwnd, GWL_STYLE, style);

	//SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	

	while (GetMessageW(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		TranslateMessage(&msg);		/* Translate key codes to chars if present */
		DispatchMessageW(&msg);		/* Send it to WndProc */
	}

	return (int) msg.wParam;
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
			/*pData[0] = (BYTE)((DWORD)pData[0] * pData[3] / 255);
			pData[1] = (BYTE)((DWORD)pData[1] * pData[3] / 255);
			pData[2] = (BYTE)((DWORD)pData[2] * pData[3] / 255);*/
			pData[0] = (BYTE) 127;
			pData[1] = (BYTE) 127;
			pData[2] = (BYTE) 127;
			pData += 4;
		}
	}
	SetDIBits(hDC, hBmp, 0, bm.bmHeight, pBitData, bmi, DIB_RGB_COLORS);
	LocalFree(pBitData);
}



LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_control, hwnd_dialog, hwnd_aa;
	POINT point;
	UINT state;
	static HWND hwndPanel;
	RECT rectParent;
	POINT ptOrigin = {0, 0};
	SIZE sizeSplash = {636, 380};	// size of the image
	BLENDFUNCTION blend = {0};
	POINT ptZero = {0, 0};
	//HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	//MONITORINFO monitorinfo = {0};
	HDC hdcScreen;
	HDC hdcMem;
	HBITMAP hbmpOld;
	HWND button1;
	
	switch (msg) {
		case WM_CREATE:
			
			//hBitmap1 = LoadSplashImage();		// Load PNG file
			hBitmap1 = LoadImageW(NULL, L"babymetal-july-babymetal-2014-promo-636-transparent.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			
			// Enhanched splash screen with alpha support
			blend.BlendOp = AC_SRC_OVER;
			blend.BlendFlags = 0;
			blend.SourceConstantAlpha = 255;
			blend.AlphaFormat = AC_SRC_ALPHA;	// 0, AC_SRC_ALPHA

			//monitorinfo.cbSize = sizeof(monitorinfo);
			//GetMonitorInfo(hmonPrimary, &monitorinfo);

			hdcScreen = GetDC(NULL);
			hdcMem = CreateCompatibleDC(hdcScreen);
			hbmpOld = SelectObject(hdcMem, hBitmap1);

			//SetLayout(hdcScreen, LAYOUT_BITMAPORIENTATIONPRESERVED | LAYOUT_RTL);
			//PremultiplyBitmapAlpha(hdcScreen, hBitmap1);

			// TODO: how to get BMP height & width size?
			UpdateLayeredWindow(hwnd, hdcScreen, &ptOrigin, &sizeSplash, hdcMem, &ptZero, 
				RGB(0, 0, 0), &blend, ULW_ALPHA);

			ReleaseDC(hwnd, hdcScreen);
			DeleteDC(hdcMem);

			button1 = CreateWindowW(L"BUTTON", L"Push Button", WS_VISIBLE | WS_CHILD,
				20, 50, 90, 25, hwnd, (HMENU) 600, NULL, NULL);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				
			}	// switch(LOWORD(wParam))

			break;
		case WM_PAINT:

			break;
		case WM_CLOSE:
			//EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			//DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			//DeleteObject(hfont1);
			//FreeLibrary(hmod);

			PostQuitMessage(0);
			break;
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}



void CenterWindow (HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	SetWindowPos(hwnd, 0,
		(GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


// Creates a stream object initialized with the data from an executable resource.
IStream *CreateStreamOnResource (LPCTSTR lpName, LPCTSTR lpType) {
	IStream *ipStream = NULL;
	DWORD dwResourceSize;
	HGLOBAL hglbImage;
	LPVOID pvSourceResourceData;
	HGLOBAL hgblResourceData;
	HRSRC hrsrc;
	LPVOID pvResourceData;

	hrsrc = FindResource(NULL, lpName, lpType);
	
	if (hrsrc == NULL)
		goto Return;

	 // load the resource
	dwResourceSize = SizeofResource(NULL, hrsrc);
	hglbImage = LoadResource(NULL, hrsrc);
	if (hglbImage == NULL)
		goto Return;

	// lock the resource, getting a pointer to its data
	pvSourceResourceData = LockResource(hglbImage);
	if (pvSourceResourceData == NULL)
		goto Return;

	// allocate memory to hold the resource data
	hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if (hgblResourceData == NULL)
		goto Return;

	// get a pointer to the allocated memory
	pvResourceData = GlobalLock(hgblResourceData);
	if (pvResourceData == NULL)
		goto FreeData;

	 // copy the data from the resource to the new memory block
	CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
	GlobalUnlock(hgblResourceData);

	// create a stream on the HGLOBAL containing the data
	if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
		goto Return;

FreeData:
	// couldn't create stream; free the memory
	GlobalFree(hgblResourceData);
 
Return:
	// no need to unlock or free the resource
	return ipStream;
}



// Loads a PNG image from the specified stream (using Windows Imaging Component).
IWICBitmapSource *LoadBitmapFromStream (IStream *ipImageStream) {
	UINT nFrameCount = 0;
	IWICBitmapSource *ipBitmap = NULL;
	IWICBitmapDecoder *ipDecoder = NULL;
	IWICBitmapFrameDecode *ipFrame = NULL;
	
	if (FAILED(CoCreateInstance(&CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, &IID_IWICBitmapDecoder, &ipDecoder)))
		goto Return;

	 // load the PNG
	if (FAILED(ipDecoder->lpVtbl->Initialize(ipDecoder, ipImageStream, WICDecodeMetadataCacheOnLoad)))
		goto ReleaseDecoder;

	 // check for the presence of the first frame in the bitmap
	
	if (FAILED(ipDecoder->lpVtbl->GetFrameCount(ipDecoder, &nFrameCount)) || nFrameCount != 1)
		goto ReleaseDecoder;

	 // load the first frame (i.e., the image)
	
	if (FAILED(ipDecoder->lpVtbl->GetFrame(ipDecoder, 0, &ipFrame)))
		goto ReleaseDecoder;

	 // convert the image to 32bpp BGRA format with pre-multiplied alpha
	//   (it may not be stored in that format natively in the PNG resource,
	//   but we need this format to create the DIB to use on-screen)
	WICConvertBitmapSource(&GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
	ipFrame->lpVtbl->Release(ipFrame);

ReleaseDecoder:
	ipDecoder->lpVtbl->Release(ipDecoder);

Return:
	return ipBitmap;
}


// Creates a 32-bit DIB from the specified WIC bitmap.
HBITMAP CreateHBITMAP (IWICBitmapSource *ipBitmap) {
	// initialize return value
	HBITMAP hbmp = NULL;
	void * pvImageBits = NULL;
	UINT cbStride;
	UINT cbImage;
	HDC hdcScreen;
	BITMAPINFO bminfo;

	// get image attributes and check for valid image
	UINT width = 0;
	UINT height = 0;

	if (FAILED(ipBitmap->lpVtbl->GetSize(ipBitmap, &width, &height)) || width == 0 || height == 0)
		goto Return;
	
	// prepare structure giving bitmap information (negative height indicates a top-down DIB)
	
	ZeroMemory(&bminfo, sizeof(bminfo));
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biWidth = width;
	bminfo.bmiHeader.biHeight = -((LONG) height);
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;
 
	// create a DIB section that can hold the image
	hdcScreen = GetDC(NULL);
	hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
	ReleaseDC(NULL, hdcScreen);
	if (hbmp == NULL)
		goto Return;
	
	// extract the image into the HBITMAP
	cbStride = width * 4;
	cbImage = cbStride * height;
	if (FAILED(ipBitmap->lpVtbl->CopyPixels(ipBitmap, NULL, cbStride, cbImage, pvImageBits))) {
		DeleteObject(hbmp);
		hbmp = NULL;
	}
	
Return:
	return hbmp;
}

HBITMAP LoadSplashImage () {
	HBITMAP hbmpSplash = NULL;
	IWICBitmapSource *ipBitmap;
	IStream *ipImageStream;		// load the PNG image data into a stream
	
	CoInitialize(NULL);
	ipImageStream = CreateStreamOnResource(MAKEINTRESOURCE(BABYMETAL_SPLASHIMAGE), L"PNG");
	
	if (ipImageStream == NULL)
		goto Return;
	
	// load the bitmap with WIC
	ipBitmap = LoadBitmapFromStream(ipImageStream);
	if (ipBitmap == NULL)
		goto ReleaseStream;

	// create a HBITMAP containing the image
	hbmpSplash = CreateHBITMAP(ipBitmap);
	ipBitmap->lpVtbl->Release(ipBitmap);

ReleaseStream:
	ipImageStream->lpVtbl->Release(ipImageStream);

Return:
	return hbmpSplash;
}






/*
WIC DECODER: 
	CLSID_WICJpegDecoder
	CLSID_WICPngDecoder
	CLSID_WICGifDecoder
	CLSID_WICTiffDecoder
	CLSID_WICBmpDecoder
	CLSID_WICIcoDecoder

WIC ENCODER:
	CLSID_WICJpegEncoder
	CLSID_WICPngEncoder
	CLSID_WICGifEncoder
	CLSID_WICTiffEncoder
	CLSID_WICBmpEncoder
	CLSID_WICIcoEncoder

*/