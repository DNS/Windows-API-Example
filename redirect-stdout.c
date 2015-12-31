
#undef UNICODE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

//void main (int argc, const char **argv, const char **env) {
INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
	char buf[512];
	FILE *ph;

	ph = _popen("ver", "rt");
	if (ph == NULL) perror("_popen error");


	while (fgets(buf, sizeof(buf), ph));
	
	_pclose(ph);


	MessageBoxA(NULL, buf, "DEBUG STRING:", MB_OK);

	return 0;
}


