

#undef UNICODE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>


void main () {
	HANDLE hmutex;
	

	hmutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "test123");
	if (hmutex != NULL) {
		printf("Same program already running!\n");
		getch();
		exit(-1);
	}

	hmutex = CreateMutexA(NULL, FALSE, "test123");

	printf("Press [any key] to close mutex: ");
	while (1) {
		getch();
		break;
	}

	printf("\nClosing mutex\n");
	ReleaseMutex(hmutex);

	getch();
}

