#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd;
    int i;
    // Mở thiết bị kernel
    fd = open("/dev/name", O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open the device");
        return -1;
    }

    // Gửi dữ liệu xuống thiết bị kernel
    char data[] = {'0','1'};
    while(1)
    {	
	 i=(i+1)%2;
   	 if (write(fd,&data[i], strlen(data)) < 0)
   	 {
       		perror("Failed to write to the device");
        	close(fd);
       		return -1;
   	 }
	 sleep(1);
    }

    // Đóng thiết bị kernel
    close(fd);

    return 0;
}
