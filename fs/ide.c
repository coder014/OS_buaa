/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

#define NSSDBLK 32
#define SSD_NO_MAPPING 0xFF
u_int ssd_map[NSSDBLK];
u_int ssd_bitmap[NSSDBLK];
u_int ssd_ecnt[NSSDBLK];
uint8_t ssd_zerodata[BY2SECT] = {0};

void ssd_init() {
	for(int i=0; i<NSSDBLK; i++) {
		ssd_map[i]=SSD_NO_MAPPING;
		ssd_bitmap[i]=1;
		ssd_ecnt[i]=0;
	}
}
int ssd_read(u_int logic_no, void *dst) {
	if(ssd_map[logic_no]==SSD_NO_MAPPING) return -1;
	ide_read(0, ssd_map[logic_no], dst, 1);
}
static u_int ssd_alloc(void) {
	u_int mincnt = 0x7fffffffU, findno;
	for(int i=0; i<NSSDBLK; i++) {
		if(ssd_bitmap[i] && ssd_ecnt[i]<mincnt)
			mincnt=ssd_ecnt[i];
	}
	for(findno=0; findno<NSSDBLK; findno++) {
		if(ssd_bitmap[findno] && ssd_ecnt[findno]==mincnt)
			break;
	}
	if(mincnt>=5) { // heavy-load swap
		mincnt = 0x7fffffffU;
		u_int findno1;
		for(int i=0; i<NSSDBLK; i++) {
                	if(!ssd_bitmap[i] && ssd_ecnt[i]<mincnt)
                        	mincnt=ssd_ecnt[i];
        	}
	        for(findno1=0; findno1<NSSDBLK; findno1++) {
        	        if(!ssd_bitmap[findno1] && ssd_ecnt[findno1]==mincnt)
                	        break;
        	}
		uint8_t swp[BY2SECT];
		ide_read(0, findno1, (void *)swp, 1);
		ide_write(0, findno, (void *)swp, 1);
		ssd_bitmap[findno]=0;
		for(int i=0; i<NSSDBLK; i++) {
			if(ssd_map[i]==findno1)
				ssd_map[i]=findno;
		}
		ide_write(0, findno1, (void *)ssd_zerodata, 1);
	        ssd_bitmap[findno1] = 1;
        	++ssd_ecnt[findno1];
	        return findno1;
	}
	return findno;
}
void ssd_write(u_int logic_no, void *src) {
	if(ssd_map[logic_no]!=SSD_NO_MAPPING) {
		ssd_erase(logic_no);
	}
	ssd_map[logic_no] = ssd_alloc();
	ide_write(0, ssd_map[logic_no], src, 1);
	ssd_bitmap[ssd_map[logic_no]] = 0;
}
void ssd_erase(u_int logic_no) {
	if(ssd_map[logic_no]==SSD_NO_MAPPING) return;
	ide_write(0, ssd_map[logic_no], (void *)ssd_zerodata, 1);
        ssd_bitmap[ssd_map[logic_no]] = 1;
	++ssd_ecnt[ssd_map[logic_no]];
	ssd_map[logic_no] = SSD_NO_MAPPING;
}

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		if (syscall_write_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_ID, sizeof(uint32_t)) != 0) {
			user_panic("failed to set disk id");
		}
		temp = begin + off;
		if (syscall_write_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_OFFSET, sizeof(uint32_t)) != 0) {
			user_panic("failed to set offset");
		}
		temp = DEV_DISK_OPERATION_READ;
		if (syscall_write_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_START_OPERATION, sizeof(uint32_t)) != 0) {
			user_panic("failed to operate disk read");
		}
		if (syscall_read_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_STATUS, sizeof(uint32_t)) != 0) {
			user_panic("failed to read status");
		}
		if (!temp) user_panic("read disk error");
		if (syscall_read_dev(dst+off, DEV_DISK_ADDRESS|DEV_DISK_BUFFER, DEV_DISK_BUFFER_LEN) != 0) {
			user_panic("failed to fetch read data");
		}
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		if (syscall_write_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_ID, sizeof(uint32_t)) != 0) {
			user_panic("failed to set disk id");
		}
		temp = begin + off;
		if (syscall_write_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_OFFSET, sizeof(uint32_t)) != 0) {
			user_panic("failed to set offset");
		}
		if (syscall_write_dev(src+off, DEV_DISK_ADDRESS|DEV_DISK_BUFFER, DEV_DISK_BUFFER_LEN) != 0) {
			user_panic("failed to push write data");
		}
		temp = DEV_DISK_OPERATION_WRITE;
		if (syscall_write_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_START_OPERATION, sizeof(uint32_t)) != 0) {
			user_panic("failed to operate disk write");
		}
		if (syscall_read_dev(&temp, DEV_DISK_ADDRESS|DEV_DISK_STATUS, sizeof(uint32_t)) != 0) {
			user_panic("failed to read status");
		}
		if (!temp) user_panic("write disk error");
	}
}
