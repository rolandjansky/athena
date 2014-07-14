/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
	if (argc==1) {
		puts(" usage: releaseFileCache <filename>\n removes from memory all the cache associtated to the file given.");
		return 0;
    }
    int fd;
    fd = open(argv[1], O_RDONLY);
#ifndef __APPLE__
    fdatasync(fd);
    posix_fadvise(fd, 0,0,POSIX_FADV_DONTNEED);
#else
	fsync(fd);
#endif
    close(fd);
    return 0;
}

