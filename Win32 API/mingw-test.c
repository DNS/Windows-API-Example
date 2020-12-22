/*
MinGW gcc/clang bug:
One thing to note is that Visual C++ supports a “wWinMain” entry point where the “lpCmdLine” 
parameter is a “LPWSTR”. You would typically use the “_tWinMain” preprocessor definition for 
your entry point and declare “LPTSTR lpCmdLine” so that you can easily support both ANSI and 
Unicode builds. However, the MinGW CRT startup library does not support wWinMain, so you’ll 
have to stick with the standard “WinMain” and use “GetCommandLine()” if you need to access 
command line arguments.

Use the plain (non unicode) WinMain function, and then get your unicode command line 
using GetCommandLineW. MinGW doesn't know about wWinMain.

MinGW usage:
i686-w64-mingw32-gcc -mwindows mingw-test.c
x86_64-w64-mingw32-gcc -mwindows mingw-test.c
x86_64-w64-mingw32-clang -mwindows mingw-test.c

x86_64-w64-mingw32-strip a.exe

*/

#include <windows.h>
#include <stdio.h>
/*
INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
//INT WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {

	//s = GetCommandLineA();

	MessageBox(NULL, L"Hello World \nダイスキ ارقد في سلام", L"title", MB_OK);
	return 0;
}
*/

//*
int main () {
	//puts("hello world");
	MessageBox(NULL, L"Hello World \nダイスキ ارقد في سلام", L"title", MB_OK);
	return 0;
}
//*/

