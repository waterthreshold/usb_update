#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <linux/reboot.h>

#include "usb_updatefw.h"
#define USB_PATH "/tmp/mnt/usb0/part1"
#define FIRMWARE_NAME "firmware.chk"

int main (int argc ,char *argv[]){
	char firmware_path[128];
	if (argc == 2)
	//if (strlen (argv[1]) >0 )
		snprintf (firmware_path,sizeof (firmware_path),"%s/%s",argv[1],FIRMWARE_NAME);
	else
		snprintf (firmware_path,sizeof (firmware_path),"%s/%s",USB_PATH,FIRMWARE_NAME);
	
	if (access(firmware_path,F_OK)!=0){
		perror ("Access");
		exit(-1);
	}
	printf ("firmware_path=%s\n",firmware_path);
	struct stat s;
	
	if (stat(firmware_path,&s)!=0){
		perror("stat");
		exit(-1);
	}
	
	printf ("Breakpoint 1  read file to buffer size =%d \n",s.st_size);
	
	FILE *f = NULL;
	unsigned char *file_buf=NULL;
	
	if ( (f=fopen(firmware_path,"rb")) ==NULL){
		perror ("fopen");
		exit(-1);
	}
	
	
	file_buf = (unsigned char *)malloc (s.st_size +2*300);
	int SIZE= s.st_size +2*300;
	memset (file_buf,0,SIZE);
	printf ("step 1 read file to buffer size =%d \n",s.st_size);
	int n=fread (file_buf,sizeof (unsigned char ), MAX_SIZE_KERNEL,f) ;
	
	ImageInfo_s image;
	image = buf_getImageInfo(file_buf);
	//unsigned long cksum=calculate_checksum (image.buf, n);
	printf ("%s %d %d %d %c filesize=%d \n",image.version,image.kernerlSize,image.rootfsSize,image.kernelChksum,file_buf,n);
	if (check_fw_header (&image,file_buf+image.header_len) != OK){
		 // perror ("check_fw");
		 goto final;
	}
	
	int ret =upgradeCgi_flashMemBlock (file_buf+image.header_len,image.kernerlSize,image.rootfsSize,image.kernelChksum);	
	
	if (ret == 0){
		printf("firmware OK!! \nreboot\n");
		reboot(0x1234567);
	}
final:
	if (f)
		fclose (f);
	if (file_buf)
		free(file_buf);
	if (image.buf) 
		free(image.buf);
	return OK;

}