#ifndef __USB_UPDATEFW_H__
#define __USB_UPDATEFW_H__
#include "status.h"
#define MAX_SIZE_KERNEL 128*1024*1024
typedef struct ImageInfo        /* Image information structure */
{
    unsigned long kernerlSize;
    unsigned long rootfsSize;
    unsigned long kernelChksum; 
    unsigned long rootfsChksum; 
    unsigned long chksum;       /*checksum of image */
	unsigned char version [32];
	unsigned long header_len;
	unsigned char board_id[32];
	unsigned long headerChksum;
	unsigned char* buf;
	
	
} ImageInfo_s;

typedef struct imageHdr_s
{
    unsigned char magic[4];     /* magic */
    unsigned char header_len[4];        /* Length of header */
    unsigned char version[8];
    unsigned char kernel_chksum[4];
    unsigned char rootfs_chksum[4];
    unsigned char kernel_len[4];        /* Length of kernel */
    unsigned char rootfs_len[4];        /* Length of rootfs */
    unsigned char image_chksum[4];      /* checksum across length of image */
    unsigned char header_chksum[4];     /* checksum across length of header */
} imageHdr_s;


ImageInfo_s buf_getImageInfo(unsigned char *buf);
STATUS upgradeCgi_flashMemBlock (char *file_buf,unsigned long kernelLen,unsigned long rootfsLen,unsigned long kernelChksum);
STATUS check_fw_header(ImageInfo_s *info,unsigned char *buf );
unsigned long calculate_checksum (unsigned char *s, int size);
#endif
 