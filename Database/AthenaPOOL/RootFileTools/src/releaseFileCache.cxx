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
    if (fd < 0) {
      perror ("open");
      return 1;
    }
#ifndef __APPLE__
    fdatasync(fd);
    if (posix_fadvise(fd, 0,0,POSIX_FADV_DONTNEED) < 0) {
      perror ("posix_fadvise");
      return 1;
    }
#else
	fsync(fd);
#endif
    close(fd);
    return 0;
}

