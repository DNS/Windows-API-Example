#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

void main (int argc, const char **argv, const char **env) {
	char buf[512];
	FILE *ph;

	ph = _popen("ver", "rt");
	if (ph == NULL) perror("_popen error");


	while (fgets(buf, sizeof(buf), ph))
		puts(buf);
 
	_pclose(ph);

	system("pause");
}


