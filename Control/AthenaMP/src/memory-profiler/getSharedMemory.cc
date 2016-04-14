/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>

namespace athenaMP_MemHelper
{
  void getPss(pid_t, unsigned long&, unsigned long&, unsigned long&, unsigned long&, bool verbose=false );
}

void
usage(int e)
{
  std::cerr << "Usage: shared_memory [-h|-v] <pid>\n" << "\n";
  exit(e);
}

int
main(int argc, char *argv[])
{
  int opt;
  long mpid;
  bool verbose=false;
  
  while ((opt = getopt(argc, argv, "vh")) != -1) {
    switch (opt) {
    case 'v':
      verbose = true;
      break;
    case 'h':
      usage(0);
      break;
    default:
      usage(-1);
    }
  }
  
  if (optind >= argc)
    {
      std::cerr << "Expected <pid> options\n";
      usage(-1);
    }
  
  if(verbose)
    std::cout << "verbose = " << verbose << "\n";
  
  if(verbose)
    std::cout << "name argument = " << argv[optind] << "\n";
  if( ! sscanf(argv[optind],"%80ld",&mpid) || mpid < 2 )
    {
      std::cerr << "Invalid <pid>\n";
      usage(-1);
    }
  
  if(verbose)
    std::cout << "Mother PID = " << mpid << "\n";
  
  /* Other code omitted */
  unsigned long size(0);
  unsigned long rss(0);
  unsigned long pss(0);
  unsigned long swap(0);
  
  athenaMP_MemHelper::getPss(mpid, pss, swap, rss, size );
  
  if(verbose)
    std::cout << "Memory usage (Pss,swap,rss,vmem,privatc,sharedc,privatd,sharedd): ";
  std::cout << pss << " " << swap << " " << rss << " " << size << "\n";
  
  if(verbose)
    {
      struct rusage r;
      getrusage(RUSAGE_SELF, &r);
      float tu=float(r.ru_utime.tv_sec) + float(r.ru_utime.tv_usec) * 1e-6;
      float ts=float(r.ru_stime.tv_sec) + float(r.ru_stime.tv_usec) * 1e-6;
      std::cout << "Used time (user/sys):  " << tu << " / " << ts << "\n";
    }
  
  exit(EXIT_SUCCESS);
}
