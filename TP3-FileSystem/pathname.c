
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DIR_MAX_LEN 14


int nombre(struct unixfilesystem *fs, int dirinumber, const char* path); 

/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns a negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
	int cmp = strcmp(pathname, "/");
	if(cmp == 0) {
		return ROOT_INUMBER;
	} else {
		const char* path = pathname + strlen("/");	
		return nombre(fs, ROOT_INUMBER, path);
	}
}

int nombre(struct unixfilesystem *fs, int dirinumber, const char* path) {
	char* slash_start = strchr(path, '/');
	if(slash_start == NULL) {		
		struct direntv6 entry;
		int err = directory_findname(fs, path, dirinumber, &entry);
		if(err < 0) 
			return -1;
		return entry.d_inumber;
	} else {						
		char* newpath = slash_start + strlen("/");	
		int dirlen = strlen(path) - strlen(newpath);
		char dir[DIR_MAX_LEN];	
		strncpy(dir, path, dirlen);
		dir[dirlen- 1] = '\0';		
		struct direntv6 entry;
		int err = directory_findname(fs, dir, dirinumber, &entry);
		if(err < 0) 
			return -1;
		return nombre(fs, entry.d_inumber, newpath);
	}
}
