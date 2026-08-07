#include <stdio.h>

int main(int argc, char *argv[])
{	
	char *path = ".";

	printf("Parsed values: \n");		

	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (argv[i][0] == '-')
			{
				printf("Flag: %s \n", argv[i]);
			}	
			else 
			{
				printf("Path: %s \n", argv[i]);  
				path = argv[i];
			}
		}

	}

	printf("\n");
	printf("Final path: %s \n", path);

	return 0;
}

