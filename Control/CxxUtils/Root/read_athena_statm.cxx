/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <errno.h>
#include <stdio.h>
#include <string.h> /* strerror */
#include <unistd.h>
#ifdef __APPLE__
#   include <mach/task_info.h>
#   include <mach/mach.h>
#endif // __APPLE__
#include "CxxUtils/read_athena_statm.h"
struct athena_statm
read_athena_statm()
{
  struct athena_statm res = {0, 0};
#ifndef __APPLE__
  
    const char *filename = "/proc/self/statm";
    FILE* fd = fopen(filename, "r");
    if (0==fd) {
      char errbuf[1024];
      strerror_r (errno, errbuf, sizeof(errbuf));
      fprintf(stderr,
	      "read_statm: problem opening file %s:\n%s\n", filename, errbuf);
      return res;
    }

    fscanf(fd, "%80u %80u", &res.vm_pages, &res.rss_pages);
    fclose(fd);
#else
    int pagesize = getpagesize();
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    if (KERN_SUCCESS == task_info(mach_task_self(),
      TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count)) {
  	// On the Mac, the virtual and resident sizes are returned in units of bytes
  	// whereas CoreDumpSvc expects them to be returned in units of pages
      res.vm_pages  = t_info.virtual_size/pagesize;
      res.rss_pages = t_info.resident_size/pagesize;
    }
#endif
    return res;
}

