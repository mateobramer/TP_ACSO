<<<<<<< HEAD
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define INDIR_ADDR 7

/**
 * Fetches the specified inode from the filesystem. 
 * Returns 0 on success, -1 on error.  
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
	inumber = inumber - 1;
	int inode_num = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
	int sector_offset = inumber / inode_num;
	int inumber_offset = inumber % inode_num;

	int fd = fs->dfd;
	struct inode inodes[inode_num];
	int err = diskimg_readsector(fd, INODE_START_SECTOR + sector_offset, inodes);
	if(err < 0) 
		return -1;
	
	*inp = inodes[inumber_offset];
	
	return 0;	
}


/**
 * Given an index of a file block, retrieves the file's actual block number
 * of from the given inode.
 *
 * Returns the disk block number on success, -1 on error.  
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
	int fd = fs->dfd;


	int small_file = ((inp->i_mode & ILARG) == 0);

	
	if(small_file) {
		return inp->i_addr[blockNum];
	}	


	int address_number = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);

	int indirect_address_number = address_number * INDIR_ADDR;
	if(blockNum < indirect_address_number) {		
		int sector_offset = blockNum / address_number;
		int address_offset = blockNum % address_number;
		uint16_t address[address_number];
		int err = diskimg_readsector(fd, inp->i_addr[sector_offset], address);
		if(err < 0) return -1;	
		return address[address_offset];
	} else {							
		
		int blockNum_in_double = blockNum - indirect_address_number;
		int sector_offset_1 = INDIR_ADDR;
		int address_offset_1 = blockNum_in_double / address_number;
		uint16_t addrs_1[address_number];
		int err_1 = diskimg_readsector(fd, inp->i_addr[sector_offset_1], addrs_1);
		if(err_1 < 0) 
			return -1;

		int sector_2 = addrs_1[address_offset_1];
		int addr_offset_2 = blockNum_in_double % address_number;
		uint16_t addrs_2[address_number];
		int err_2 = diskimg_readsector(fd, sector_2, addrs_2);
		if(err_2 < 0) 
			return -1;
		return addrs_2[addr_offset_2];
	}	
}


/**
 * Computes the size in bytes of the file identified by the given inode
 */
int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
=======
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    //Implement Code Here
    return 0; 
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp,
    int blockNum) {  
        //Implement code here
    return 0;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
>>>>>>> 694cfe4f06a52f4159638b2be6a5b56ec4122a42
