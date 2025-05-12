#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  struct inode inode_mio;
	int err = inode_iget(fs, dirinumber, &inode_mio);
	if(err < 0) return -1;

	int is_dir = ((inode_mio.i_mode & IFMT) == IFDIR);
	if(!is_dir) return -1;

	int dir_size = inode_getsize(&inode_mio);
	if(dir_size < 0) return -1;
	if(dir_size == 0) return -1;

	int block_totales = (dir_size - 1) / DISKIMG_SECTOR_SIZE + 1;
	for(int i = 0; i < block_totales; i++) {		
		struct direntv6 entradas[DISKIMG_SECTOR_SIZE / sizeof(struct direntv6)];
		int bytes_validos = file_getblock(fs, dirinumber, i, entradas);
		if(bytes_validos < 0) return -1;
		int entry_totales = bytes_validos / sizeof(struct direntv6);
		for(int j = 0; j < entry_totales; j++) {	
			int cmp = strcmp(entradas[j].d_name, name);
			if(cmp == 0) {
				*dirEnt = entradas[j];
				return 0;	
			}
		}
	}

	return -1;
}