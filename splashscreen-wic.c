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


#define IDI_SPLASHIMAGE 3


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
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;	// default window color
	//wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));	// make something different
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
	hwnd = CreateWindowExW(NULL, 
		wc.lpszClassName, L"Title", 
		WS_VISIBLE, 
		0, 0, 636, 380, (HWND) NULL, (HMENU) NULL, hInstance, NULL);
	
	CenterWindow(hwnd);

	style = GetWindowLong(hwnd, GWL_STYLE);
	style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	
	//SetWindowLong(hwnd, GWL_STYLE, style);	// obsolete
	SetWindowLongPtr(hwnd, GWL_STYLE, style);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	

	while (GetMessageW(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		TranslateMessage(&msg);		/* Translate key codes to chars if present */
		DispatchMessageW(&msg);		/* Send it to WndProc */
	}

	return (int) msg.wParam;
}





LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_control, hwnd_dialog, hwnd_aa;
	POINT point;
	UINT state;
	static HWND hwndPanel;
	RECT rectParent;

	switch (msg) {
		case WM_CREATE:

			staticimage1 = CreateWindowW(L"STATIC", L"This is Label", 
				WS_CHILD | WS_VISIBLE | SS_BITMAP, 
				0, 0, 100, 100, hwnd, (HMENU) 9524, NULL, NULL);

			//hBitmap1 = LoadImageW(NULL, L"test123.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			hBitmap1 = LoadSplashImage();
			SendMessageW(staticimage1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBitmap1);


			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				
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
	ipImageStream = CreateStreamOnResource(MAKEINTRESOURCE(IDI_SPLASHIMAGE), L"PNG");
	
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