#ifndef __STATUS_H__
#define __STATUTS_H__
#define STATUS int
#define OK	0
#define ERROR	(-1)
#define UINT32 unsigned long 
#define INT32 int 





#if !(defined KERNEL_MTD_WR) || !(defined ROOTFS_MTD_WR)
#ifdef LINUX26
#if (defined MTD_NFLASH)
#else
#define BOOT_MTD_RD              "/dev/mtdblock0"
#define BOOT_MTD_WR              "/dev/mtd0"
/*Foxconn modify by Hank 08/21/2012*/
#if (defined U12H189) || (defined R6300v2) || defined(R6250) || (defined AC1450)
#define KERNEL_MTD_RD            "/dev/mtdblock15"
#define KERNEL_MTD_WR            "/dev/mtd15"
#define ROOTFS_MTD_RD            "/dev/mtdblock16"
#define ROOTFS_MTD_WR            "/dev/mtd16"
#elif defined(R7000) 
#define KERNEL_MTD_RD            "/dev/mtdblock2"
#define KERNEL_MTD_WR            "/dev/mtd2"
#define ROOTFS_MTD_RD            "/dev/mtdblock3"
#define ROOTFS_MTD_WR            "/dev/mtd2"
#elif defined(R6200v2) 
#define KERNEL_MTD_RD            "/dev/mtdblock2"
#define KERNEL_MTD_WR            "/dev/mtd2"
#define ROOTFS_MTD_RD            "/dev/mtdblock3"
#define ROOTFS_MTD_WR            "/dev/mtd3"
#else
#define KERNEL_MTD_RD            "/dev/mtdblock1"
#define KERNEL_MTD_WR            "/dev/mtd1"

#define ROOTFS_MTD_RD            "/dev/mtdblock2"
#define ROOTFS_MTD_WR            "/dev/mtd2"
#endif	//U12H189

#define LANG_TBL1_MTD_RD         "/dev/mtdblock3"
#define LANG_TBL1_MTD_WR         "/dev/mtd3"
#define LANG_TBL2_MTD_RD         "/dev/mtdblock4"
#define LANG_TBL2_MTD_WR         "/dev/mtd4"
#endif
#else /* LINUX26 */
#define BOOT_MTD_RD              "/dev/mtdblock/0"
#define BOOT_MTD_WR              "/dev/mtd/0"

#define KERNEL_MTD_RD            "/dev/mtdblock/1"
#define KERNEL_MTD_WR            "/dev/mtd/1"

#define ROOTFS_MTD_RD            "/dev/mtdblock/2"
#define ROOTFS_MTD_WR            "/dev/mtd/2"

/* Foxconn add start, Max Ding, 11/20/2008 @multi_language*/
#define LANG_TBL1_MTD_RD            "/dev/mtdblock/3"
#define LANG_TBL1_MTD_WR            "/dev/mtd/3"
#define LANG_TBL2_MTD_RD            "/dev/mtdblock/4"
#define LANG_TBL2_MTD_WR            "/dev/mtd/4"
#endif /* LINUX26 */
/* Foxconn add end, Max Ding, 11/20/2008 */
#endif


#endif
