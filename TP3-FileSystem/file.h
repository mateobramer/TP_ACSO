<<<<<<< HEAD
#ifndef _FILE_H_
#define _FILE_H_

#include "unixfilesystem.h"

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNo, void *buf); 

#endif // _FILE_H_
=======
#ifndef _FILE_H_
#define _FILE_H_

#include "unixfilesystem.h"

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNo, void *buf); 

#endif // _FILE_H_
>>>>>>> 694cfe4f06a52f4159638b2be6a5b56ec4122a42
