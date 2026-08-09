#include <unistd.h>
#include <stddef.h>

size_t string_length(const char *str)
{
	size_t length = 0;

	while (str[length] != '\0')
	{
		length++;
	}
	
	return length;
}

int main(int argc, char *argv[])
{
	char *path = ".";

	write(1, "Parse values \n", 15);

	if (argc > 1)
	{
		for (int i = 0; i < argc; ++i)
		{
			if (argv[i][0] == '-')
			{
				write(1, "Flag: ", 6);
				write(1, argv[i], string_length(argv[i]));
				write(1, "\n", 1);
			}
			else
			{
				write(1, "Path: ", 6);
				write(1, argv[i], string_length(argv[i]));
				write(1, "\n", 1);

				path = argv[i];
			}
		}

	}

	write (1, "\n Final path: ", 13);
	write (1, path, string_length(path));
	write (1, "\n", 1);

	return 0;
}
