#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"


int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode my_inode;
	int err = inode_iget(fs, inumber, &my_inode);
	if(err < 0) 
		return -1;

	int sector = inode_indexlookup(fs, &my_inode, blockNum);
	if(sector < 0) 
		return -1;

	int read_err = diskimg_readsector(fs->dfd, sector, buf);
	if(read_err < 0) 
		return -1;

	
	int total_bytes = inode_getsize(&my_inode);
	if(total_bytes < 0) 
		return -1;
	int total_blocks = total_bytes / DISKIMG_SECTOR_SIZE;
	if(blockNum == total_blocks) {
		return total_bytes % DISKIMG_SECTOR_SIZE;
	} else {
		return DISKIMG_SECTOR_SIZE;
	}
}
