
#define UNICODE

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 

#include <Windows.h>
#include <Commctrl.h>

#define IDC_EXIT_BUTTON 101
#define IDC_PUSHLIKE_BUTTON 102

HPEN pen;
HGDIOBJ old_pen, old_brush;

HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, LPNMCUSTOMDRAW item)
    {
        HBRUSH Brush = NULL;
		HBRUSH pattern;
        HDC hdcmem = CreateCompatibleDC(item->hdc);
		int i;
        HBITMAP hbitmap = CreateCompatibleBitmap(item->hdc, item->rc.right-item->rc.left, item->rc.bottom-item->rc.top);
		int r1 = GetRValue(top), r2 = GetRValue(bottom), g1 = GetGValue(top), g2 = GetGValue(bottom), b1 = GetBValue(top), b2 = GetBValue(bottom);

        SelectObject(hdcmem, hbitmap);
		        
        for(i = 0; i < item->rc.bottom-item->rc.top; i++)
        { 
            RECT temp;
            int r, g, b;
            r = r1 + (i * (r2-r1) / item->rc.bottom-item->rc.top);
            g = g1 + (i * (g2-g1) / item->rc.bottom-item->rc.top);
            b = b1 + (i * (b2-b1) / item->rc.bottom-item->rc.top);
            Brush = CreateSolidBrush(RGB(r, g, b));
            temp.left = 0;
            temp.top = i;
            temp.right = item->rc.right-item->rc.left;
            temp.bottom = i + 1; 
            FillRect(hdcmem, &temp, Brush);
            DeleteObject(Brush);
        }
        pattern = CreatePatternBrush(hbitmap);

        DeleteDC(hdcmem);
        DeleteObject(Brush);
        DeleteObject(hbitmap);

        return pattern;
    }

LRESULT CALLBACK MainWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH defaultbrush = NULL;
    static HBRUSH hotbrush = NULL;
    static HBRUSH selectbrush = NULL;
    static HBRUSH push_uncheckedbrush = NULL;
    static HBRUSH push_checkedbrush = NULL;
    static HBRUSH push_hotbrush1 = NULL;
    static HBRUSH push_hotbrush2 = NULL;
	HWND Exit_Button, Pushlike_Button;
    switch (msg)
    {
        case WM_CREATE:
            {
                Exit_Button = CreateWindowEx(NULL, L"BUTTON", L"EXIT", 
                                                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
                                                        50, 50, 100, 100, hwnd, (HMENU)IDC_EXIT_BUTTON, NULL, NULL);
                if(Exit_Button == NULL)
                    {
                        MessageBox(NULL, L"Button Creation Failed!", L"Error!", MB_ICONEXCLAMATION);
                        exit(EXIT_FAILURE);
                    }

                Pushlike_Button = CreateWindowEx(NULL, L"BUTTON", L"PUSH ME!", 
                                                        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_PUSHLIKE, 
                                                        200, 50, 100, 100, hwnd, (HMENU)IDC_PUSHLIKE_BUTTON, NULL, NULL);
                if(Pushlike_Button == NULL)
                    {
                        MessageBox(NULL, L"Button Creation Failed!", L"Error!", MB_ICONEXCLAMATION);
                        exit(EXIT_FAILURE);
                    }
            }
        break;
        case WM_COMMAND:
            {
                switch(LOWORD(wParam))
                    {
                        case IDC_EXIT_BUTTON:
                            {
                                SendMessage(hwnd, WM_CLOSE, 0, 0);
                            }
                        break;
                    }
            }
        break;
        case WM_NOTIFY:
        {
            LPNMHDR some_item = (LPNMHDR)lParam;

            if (some_item->idFrom == IDC_EXIT_BUTTON && some_item->code == NM_CUSTOMDRAW)
            {
                LPNMCUSTOMDRAW item = (LPNMCUSTOMDRAW)some_item;

                if (item->uItemState & CDIS_SELECTED)
                {
                    //Select our color when the button is selected
                    if (selectbrush == NULL)
                        selectbrush = CreateGradientBrush(RGB(180, 0, 0), RGB(255, 180, 0), item);

                    //Create pen for button border
                    pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));

                    //Select our brush into hDC
                    old_pen = SelectObject(item->hdc, pen);
                    old_brush = SelectObject(item->hdc, selectbrush);

                    //If you want rounded button, then use this, otherwise use FillRect().
                    RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 5, 5);

                    //Clean up
                    SelectObject(item->hdc, old_pen);
                    SelectObject(item->hdc, old_brush);
                    DeleteObject(pen);

                    //Now, I don't want to do anything else myself (draw text) so I use this value for return:
                    return CDRF_DODEFAULT;
                    //Let's say I wanted to draw text and stuff, then I would have to do it before return with
                    //DrawText() or other function and return CDRF_SKIPDEFAULT
                }
                else
                {
                    if (item->uItemState & CDIS_HOT) //Our mouse is over the button
                    {
                        //Select our color when the mouse hovers our button
                        if (hotbrush == NULL)
                            hotbrush = CreateGradientBrush(RGB(255, 230, 0), RGB(245, 0, 0), item);

                        pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));

                        old_pen = SelectObject(item->hdc, pen);
                        old_brush = SelectObject(item->hdc, hotbrush);

                        RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 5, 5);

                        SelectObject(item->hdc, old_pen);
                        SelectObject(item->hdc, old_brush);
                        DeleteObject(pen);

                        return CDRF_DODEFAULT;
                    }

                    //Select our color when our button is doing nothing
                    if (defaultbrush == NULL)
                        defaultbrush = CreateGradientBrush(RGB(255, 180, 0), RGB(180, 0, 0), item);

                    pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));

                    old_pen = SelectObject(item->hdc, pen);
                    old_brush = SelectObject(item->hdc, defaultbrush);

                    RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 5, 5);

                    SelectObject(item->hdc, old_pen);
                    SelectObject(item->hdc, old_brush);
                    DeleteObject(pen);

                    return CDRF_DODEFAULT;
                }
            }
            else if (some_item->idFrom == IDC_PUSHLIKE_BUTTON && some_item->code == NM_CUSTOMDRAW)
            {
                LPNMCUSTOMDRAW item = (LPNMCUSTOMDRAW)some_item;

                if (IsDlgButtonChecked(hwnd, some_item->idFrom))
                {
                    if (item->uItemState & CDIS_HOT)
                    {

                        if (push_hotbrush1 == NULL)
                            push_hotbrush1 = CreateGradientBrush(RGB(0, 0, 245), RGB(0, 230, 255), item);

                        pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));

                        old_pen = SelectObject(item->hdc, pen);
                        old_brush = SelectObject(item->hdc, push_hotbrush1);

                        RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 10, 10);

                        SelectObject(item->hdc, old_pen);
                        SelectObject(item->hdc, old_brush);
                        DeleteObject(pen);

                        return CDRF_DODEFAULT;
                    }


                    if (push_checkedbrush == NULL)
                        push_checkedbrush = CreateGradientBrush(RGB(0, 0, 180), RGB(0, 222, 200), item);


                    pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));


                    old_pen = SelectObject(item->hdc, pen);
                    old_brush = SelectObject(item->hdc, push_checkedbrush);


                    RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 10, 10);


                    SelectObject(item->hdc, old_pen);
                    SelectObject(item->hdc, old_brush);
                    DeleteObject(pen);


                    return CDRF_DODEFAULT;
                }
                else
                {
                    if (item->uItemState & CDIS_HOT)
                    {
                        if (push_hotbrush2 == NULL)
                            push_hotbrush2 = CreateGradientBrush(RGB(255, 230, 0), RGB(245, 0, 0), item);

                        pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));

                        old_pen = SelectObject(item->hdc, pen);
                        old_brush = SelectObject(item->hdc, push_hotbrush2);

                        RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 10, 10);

                        SelectObject(item->hdc, old_pen);
                        SelectObject(item->hdc, old_brush);
                        DeleteObject(pen);

                        return CDRF_DODEFAULT;
                    }

                    if (push_uncheckedbrush == NULL)
                        push_uncheckedbrush = CreateGradientBrush(RGB(255, 180, 0), RGB(180, 0, 0), item);

                    pen = CreatePen(PS_INSIDEFRAME, 0, RGB(0, 0, 0));

                    old_pen = SelectObject(item->hdc, pen);
                    old_brush = SelectObject(item->hdc, defaultbrush);

                    RoundRect(item->hdc, item->rc.left, item->rc.top, item->rc.right, item->rc.bottom, 10, 10);

                    SelectObject(item->hdc, old_pen);
                    SelectObject(item->hdc, old_brush);
                    DeleteObject(pen);

                    return CDRF_DODEFAULT;
                }
            }
            return CDRF_DODEFAULT;
        }
        break;
        case WM_CTLCOLORBTN: //In order to make those edges invisble when we use RoundRect(),
            {                //we make the color of our button's background match window's background
                return (LRESULT)GetSysColorBrush(COLOR_WINDOW+1);
            }
        break;
        case WM_CLOSE:
            {
                DestroyWindow(hwnd);
                return 0;
            }
        break;
        case WM_DESTROY:
            {
                DeleteObject(defaultbrush);
                DeleteObject(selectbrush);
                DeleteObject(hotbrush);
                DeleteObject(push_checkedbrush);
                DeleteObject(push_hotbrush1);
                DeleteObject(push_hotbrush2);
                DeleteObject(push_uncheckedbrush);
                PostQuitMessage(0);
                return 0;
            }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;
    const wchar_t ClassName[] = L"Main_Window";

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = MainWindow;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = ClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        exit(EXIT_FAILURE);
    }

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, ClassName, L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 368, 248, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        exit(EXIT_FAILURE);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.message;
}