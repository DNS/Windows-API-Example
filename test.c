

#include <stdio.h>
#include <windows.h>


INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	LPTSTR s;
	s = GetCommandLine();

	MessageBox(NULL, s, "DEBUG", MB_OK);



	return 0;
}

