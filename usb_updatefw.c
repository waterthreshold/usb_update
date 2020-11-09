#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "status.h"
#ifdef LINUX26
#include <mtd/mtd-user.h>
#else
#include <linux/mtd/mtd.h>
#endif
#include "usb_updatefw.h"
#include "../include/ambitCfg.h"
#define DEBUG
unsigned long calculate_checksum (unsigned char *s, int size)
{
    UINT32 c0=0,c1=0;
    UINT32 checksum=0, b;
    int i;

  
   
	for (i = 0; i < size; i++)
	{
		c0 += s[i] & 0xff;
		c1 += c0;
	}

	b = (c0 & 65535) + ((c0 >> 16) & 65535);
	c0 = ((b >> 16) + b) & 65535;

	b = (c1 & 65535) + ((c1 >> 16) & 65535);
	c1 = ((b >> 16) + b) & 65535;

	checksum = ((c1 << 16) | c0);

	return checksum;
    
    
}
/*
static unsigned long calculate_checksum (int action, char *s, int size)
{
    static UINT32 c0, c1;
    UINT32 checksum, b;
    int i;

    switch (action)
    {
    case 0:
        c0 = c1 = 0;
        break;
    case 1:
        for (i = 0; i < size; i++)
        {
            c0 += s[i] & 0xff;
            c1 += c0;
        }
        break;
    case 2:
        b = (c0 & 65535) + ((c0 >> 16) & 65535);
        c0 = ((b >> 16) + b) & 65535;

        b = (c1 & 65535) + ((c1 >> 16) & 65535);
        c1 = ((b >> 16) + b) & 65535;

        checksum = ((c1 << 16) | c0);

        return checksum;
    }
    return 0;
}

*/
static void kill_program (){
	printf ("Kill all user process\n");
	system("killall -9 swresetd > /dev/null 2> /dev/null; "
           "killall -9 wlanconfigd > /dev/null 2> /dev/null; "
           "killall -9 dnsRedirectReplyd > /dev/null 2> /dev/null; "
           "killall -9 dnsmasq > /dev/null 2> /dev/null; "
           "killall -9 udhcpd > /dev/null 2> /dev/null; "
           "killall -9 telnetenabled > /dev/null 2> /dev/null; "
           "killall -9 ddnsd > /dev/null 2> /dev/null; "
           "killall -9 upnpd > /dev/null 2> /dev/null; "
           "killall -9 soapd > /dev/null 2> /dev/null; "
           "killall -9 email > /dev/null 2> /dev/null; "
           "killall -9 lld2d > /dev/null 2> /dev/null; "
           "killall -9 wsc > /dev/null 2> /dev/null; "
           "killall -9 wsccmd > /dev/null 2> /dev/null; "
           "killall -9 wsccmd > /dev/null 2> /dev/null; "
           "killall -9 timesync > /dev/null 2> /dev/null; "
#ifdef INCLUDE_DOWNLOADER
           "killall -9 transmission-cli > /dev/null 2> /dev/null; "/* Foxconn added by Max Ding, 01/29/2013 */
           "killall -9 wget > /dev/null 2> /dev/null; "/* Foxconn added by Max Ding, 01/29/2013 */
#endif
#ifdef CONFIG_FUNJSQ
           "/tmp/funjsq/bin/funjsq.sh killall;"
#endif
           "killall -9 pot > /dev/null 2> /dev/null ");
		   
		   
	unlink("/tmp/ppp/ip-up");
    unlink("/tmp/ppp/ip-down");
}

STATUS upgradeCgi_flashMemBlock (char *file_buf,unsigned long kernelLen,unsigned long  rootfsLen,unsigned long kernelChksum)
{
     unsigned long error = 0;
   

    if (kernelLen)
    {
		printf ("KERNEL_MTD_WR=%s\n",KERNEL_MTD_WR);
        error = upgradeXndmtd (KERNEL_MTD_WR, file_buf, kernelLen, rootfsLen,
                               kernelChksum);
        /* Foxconn add start, Jasmine Yang, 12/05/2006 */
		/*if (file_buf != NULL)
        {
            free (file_buf);
            file_buf = NULL;
        }*/
        /* Foxconn add end, Jasmine Yang, 12/05/2006 */
        /* wklin modified start, 12/15/2006 */
        if (error) {
            return ERROR;
        } 
		kill_program();
    }

    return OK; 
}


int upgradeXndmtd (char *mtdFileName, unsigned char *file_buf,
                          unsigned long imageSize, unsigned long imageOffset,
                          unsigned long checksum)
{
    mtd_info_t mtd_info;
    erase_info_t erase_info;
    int mtd_fd = -1;
    unsigned long block_offset = 0, count = 0;
    unsigned long block_size;
    unsigned char *pImage = NULL;
    char partition_data[8];
    off_t offset;
    int curr_offset, last_block_offset;
    int n_written = 0, fail_count = 0; /* wklin added, 01/17/1007 */
	printf ("%s:mtdFileName=%s ,file_buf=%c,imageSize=%d, imageOffset=%d, checksum=%d\n",__func__,mtdFileName,file_buf[0],imageSize,imageOffset,checksum);
    if (!file_buf)
        return -1;

    /* Open MTD device and get sector size */
    if ((mtd_fd = open (mtdFileName, O_RDWR)) < 0
        || ioctl (mtd_fd, MEMGETINFO, &mtd_info) != 0)
    {
        perror ("mtd open");
        return -1;
    }

    erase_info.length = mtd_info.erasesize;
    block_size = mtd_info.erasesize;
    pImage = &file_buf[imageOffset];



    /* foxconn added start, 06/11/2011 */
    /* don't use MEMUNLOCK and MEMERASE for nflash */
    fail_count = 0;
nflash_try_again:
    n_written = write (mtd_fd, pImage, imageSize);
    if ((n_written < 0) || (n_written != imageSize))
    {
        fail_count++;
        fprintf(stderr, "[%d]fail. n_written=%d imageSize=%lu\n",
                    fail_count, n_written, imageSize);
        if (fail_count > 3)
            goto bye_reboot;
        else
        {
            lseek(mtd_fd, 0, SEEK_SET);
            goto nflash_try_again;
        }
    }
  
    fprintf (stderr, "done.\r\n");
    
    

bye_reboot:
    if (mtd_fd >= 0)
        close (mtd_fd);

    return 0; /* return 0 indicating system has to reboot */
}



STATUS check_fw_header(ImageInfo_s *info,unsigned char *buf){
	unsigned long  checksum=0;
	checksum=calculate_checksum(buf,info->kernerlSize);
	/*
	calculate_checksum (0, NULL, 0);

    calculate_checksum (1, info->buf, info->kernerlSize);

    checksum = calculate_checksum (2, NULL, 0);
	*/
	if (checksum != info->kernelChksum){
		fprintf (stderr,"Kernel checksum=%lu failed\n",checksum);
		return ERROR;
	}
	
	if (strncmp (info->board_id,AMBIT_HARDWARE_VERSION,7)!=0 )
	{	
		fprintf (stderr,"board ID failed AMBIT_HARDWARE_VERSION=%s board_id=%s\n",AMBIT_HARDWARE_VERSION,info->board_id);
		return ERROR;
		
	}
	
	return OK;
}
static unsigned long buf2Uint (char *buf,char *option,int size){
	int i;
	unsigned long sum=0;
	for (i=0;i<size;i++){
		//printf ("%s buf =%x\n",__func__,buf[i]);
		if (i>0)
		sum = (sum << 8);
		sum = sum | ( buf[i]&0xff);
	}
	#ifdef DEBUG
	//printf ("%s sum =%lu\n",__func__,sum);
	#endif
	return sum;
}

unsigned int HEX2UINT (unsigned char c ){
	int sum=0;
	sum = ((c & 0xf0)>> 4) *16;
	sum+= (c&0x0f);
	return sum;
	
} 

ImageInfo_s buf_getImageInfo (unsigned char *buf){
	ImageInfo_s image;
	imageHdr_s header;
	char magic [5]={0};
	//unsigned int readcount=0;
	memset (&image,0, sizeof (ImageInfo_s));
	unsigned char *tmp=buf;
	unsigned long checksum=0;
	
	
	//magic
	snprintf (magic,sizeof(magic),"%c%c%c%c",buf[0],buf[1],buf[2],buf[3]);
	
	buf+=sizeof(magic);
	
	//header_len
	image.header_len = buf2Uint(buf,header.header_len,sizeof(header.header_len));
	buf+=sizeof(header.header_len);
	image.header_len=58;
	#ifdef DEBUG
	//printf ("%s headerlen =%d\n",__func__,sizeof(header.header_len));
	#endif
	//version
	snprintf (image.version,sizeof (image.version),"V%d.%d.%d.%d_%d.%d.%d",HEX2UINT (buf[0]),HEX2UINT (buf[1]), HEX2UINT (buf[2]),HEX2UINT (buf[3]), HEX2UINT (buf[4]), HEX2UINT (buf[5]), HEX2UINT (buf[6]));
	buf+=7;
	
	//kernel_chksum
	image.kernelChksum = buf2Uint(buf,header.kernel_chksum,sizeof(header.kernel_chksum));
	buf+=sizeof(header.kernel_chksum);
	
	//rootfs_chksum
	image.rootfsChksum = buf2Uint(buf,header.rootfs_chksum,sizeof(header.rootfs_chksum));
	buf+=sizeof(header.rootfs_chksum);
	
	//kernel_len
	image.kernerlSize = buf2Uint(buf,header.kernel_len,sizeof(header.kernel_len));
	buf+=sizeof (header.kernel_len);
	
	//rootfs_len
	image.rootfsSize = buf2Uint(buf,header.rootfs_len,sizeof(header.rootfs_len));
	buf+=sizeof (header.rootfs_len);
	
	//image_chksum
	image.chksum=buf2Uint(buf,header.image_chksum,sizeof(header.image_chksum));
	buf += sizeof (header.image_chksum);
	
	//header_chksum;
	image.headerChksum=buf2Uint(buf,header.header_chksum,sizeof(header.header_chksum));
	buf += sizeof (header.header_chksum);
	
	
	char board_id[32]={0};
	strncpy(board_id,buf,image.header_len-40);
	strncpy(image.board_id,board_id,sizeof(image.board_id));
	buf += (image.header_len-40);
	printf ("111111111111111111111 ===%c\n",buf);
	//image.buf=(unsigned char * )malloc (image.kernerlSize+1);
	// strncpy(image.buf,tmp+image.header_len,image.kernerlSize);
	/*
	image.kernerlSize = kernel_len;
	image.rootfsSize = rootfs_len;
	image.chksum = image_chksum;
	
	image.kernelChksum = kernel_chksum;
	image.rootfsChksum = rootfs_chksum;
	*/
	
	#ifdef DEBUG
	printf ("Image info:\n");
	printf ("-----------------------------------------\n");
	printf ("magic=%s\n",magic );
	printf ("header_len=%lu\n",image.header_len);
	printf ("version=%s\n",image.version);
	printf ("kernerlSize=%lu\n",image.kernerlSize);
	printf ("board_id=%s\n",image.board_id);
	printf ("rootfsSize=%lu\n",image.rootfsSize);
	printf ("chksum=%lu\n",image.chksum);
	printf ("kernelChksum=%lu\n",image.kernelChksum);
	printf ("rootfsChksum=%lu\n",image.rootfsChksum);
	printf ("-----------------------------------------\n");
	#endif 
	return image;
}