/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
int main(int argc, char *argv[]) {
    
    if (argc==1) {
		puts(" usage: checkCache <filename>\n prints out all the cached blocks of the file given. warning: for a large file that may mean a big printout.");
    	return 0;
    }
    int fd;
    struct stat file_stat;
    void *file_mmap;
#ifndef __APPLE__
    unsigned char *mincore_vec;
#else
    char *mincore_vec;
#endif
    size_t page_size = getpagesize();
    size_t page_index;

    fd = open(argv[1],0);
    if (fd < 0) {
      perror ("open");
      return 1;
    }
    if (fstat(fd, &file_stat) < 0) {
      perror ("ftsat");
      return 1;
    }
    file_mmap = mmap((void *)0, file_stat.st_size, PROT_NONE, MAP_SHARED, fd, 0);
#ifndef __APPLE__
    mincore_vec = (unsigned char *) calloc(1, (file_stat.st_size+page_size-1)/page_size);
#else
    mincore_vec = (char *) calloc(1, (file_stat.st_size+page_size-1)/page_size);
#endif
    mincore(file_mmap, file_stat.st_size, mincore_vec);
    printf("Cached Blocks of %s: ",argv[1]);
    for (page_index = 0; page_index <= file_stat.st_size/page_size; page_index++) {
        if (mincore_vec[page_index]&1) {
            printf("%lu ", (unsigned long)page_index);
        }
    }
    printf("\n");
    free(mincore_vec);
    munmap(file_mmap, file_stat.st_size);
    close(fd);
    return 0;
}

