#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void main(void)
{
	int fd = open("file_test", O_CREAT | O_RDWR, 0644);
	close(fd);
}