#include <unistd.h>

int main(void)
{
	write(0, "hello\n", 6);

	return 0;
}
