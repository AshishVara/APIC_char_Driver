#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "apic.h"

int main(int argc, char *argv[])
{

	int fd,ret;
	unsigned int id,maxirq, stat, type;
	unsigned int buf;
	fd = open("/dev/apic", O_RDWR);
	if(fd < 0)
	{
		printf("failed acquiring file descriptor status = %d\n", fd);
		return fd;
	}

	ret = ioctl(fd, APIC_GETID, &id);
	if(ret < 0)
	{
		printf("couldn't find ID\n");
		close(fd);
		exit(0);
	}

	printf("APIC ID: %d\n",id);

//	getchar();

	ret = ioctl(fd, APIC_GETIRQS, &maxirq);
        if(ret < 0)
        {
                printf("couldn't find IRQ\n");
                close(fd);
                exit(0);
        }
	printf("Interrupt lines : %d\n", maxirq);

//	getchar();

	stat =atoi(argv[1]);
	ret = ioctl(fd, APIC_GETIRQSTATUS, &stat);
        if(ret < 0)
        {
                printf("status not found\n");
                close(fd);
                exit(0);
        }
	if(stat == 1)
		printf("Interrupt line is disable with status %d\n", stat);
	else
		printf("Interrupt line is enable with status %d\n", stat);

//  	getchar();

	type = atoi(argv[1]);
	ret = ioctl(fd, APIC_GETIRQTYPE, &type);
        if(ret < 0)
        {
                printf("type not found\n");
                close(fd);
                exit(0);
        }
	if(type == 1)
                printf("Interrupt type : Level trigger with type %d\n", stat);
        else
                printf("Interrupt type : edge trigger with type %d\n", stat);


	close(fd);

}
