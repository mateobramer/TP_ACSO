<<<<<<< HEAD
#ifndef _PATHNAME_H_
#define _PATHNAME_H_

#include "unixfilesystem.h"

/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns a negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname);

#endif // _PATHNAME_H_
=======
#ifndef _PATHNAME_H_
#define _PATHNAME_H_

#include "unixfilesystem.h"

/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns a negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname);

#endif // _PATHNAME_H_
>>>>>>> 694cfe4f06a52f4159638b2be6a5b56ec4122a42
