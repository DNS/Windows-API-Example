

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <windows.h>


#define ENABLE_USBSTOR 0x00000003
#define DISABLE_USBSTOR 0x00000004

#ifndef NULL
#define NULL (void *) 0
#endif





int main (int argc, char **argv, char **env) {
	DWORD usb_mode;// default behavior: DISABLE_USBSTOR
	
	if (argc <= 1) {
		printf("usage: mass-stor [0|1]\n"
						"0 to disable USB mass storage\n"
						"1 to enable USB mass storage"
					);
		usb_mode = DISABLE_USBSTOR;
	} else if (stricmp(argv[1], "0") == 0) {
		usb_mode = DISABLE_USBSTOR;
	} else {
 		usb_mode = ENABLE_USBSTOR;
	}
	
	
	
	HKEY hKey = HKEY_LOCAL_MACHINE;
	HKEY phkResult;
	LONG result;
	result = RegOpenKeyEx(hKey, "SYSTEM\\CurrentControlSet\\services\\USBSTOR\\", 0, KEY_ALL_ACCESS, &hKey);
	
	if (result == ERROR_SUCCESS) {
		fprintf(stderr, "open ok\n");
	} else {
		fprintf(stderr, "open error\n");
	}

	DWORD lpData = usb_mode;
	result = RegSetValueEx(hKey, "Start", 0, REG_DWORD, (const BYTE*) &usb_mode, sizeof(DWORD));
	if (result == ERROR_SUCCESS) {
		fprintf(stderr, "set ok\n");
	} else {
		fprintf(stderr, "set error\n");
	}

/**
	DWORD buf = 999;
	DWORD dwType = REG_DWORD;
	WORD lpcbData = sizeof(DWORD);
	result = RegQueryValueEx(hKey, TEXT("Start"), 0, &dwType, (LPBYTE) &buf, (LPDWORD) &lpcbData);
	printf("dword: %d\n", buf);
	
	
	//DWORD dwDisp;
	//result = RegCreateKeyEx(hKey, "XXX", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &phkResult, &dwDisp);

	
	if (result == ERROR_SUCCESS) {
	fprintf(stderr, "query ok\n");
	} else {
	fprintf(stderr, "query error\n");
	}

	*/

	RegCloseKey(hKey);
	
	//getch();
	return 0;
}

