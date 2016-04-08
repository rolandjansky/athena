/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<iostream>
#include "FileCatalog/MemoryReport.h"

#include <fstream>
#ifdef __linux__
MemoryReport::MemoryReport(const std::string & s) {
  unsigned int memsz;
  unsigned int rsssz;
  unsigned int sharedsz;
  unsigned int textsz;
  unsigned int datasz;
  unsigned int libsz;
  unsigned int dtsz;
  const unsigned int pagesz=4;
  std::cout <<"Memory use in kb," <<s<< std::endl;
  std::ifstream statm("/proc/self/statm");
  statm >> memsz >> rsssz >> sharedsz >> textsz >> datasz
	>> libsz >> dtsz;
  statm.close();
  std::cout << "Total memory=" << pagesz*memsz
       << ", Resident=" << pagesz*rsssz
       << ", Shared=" << pagesz*sharedsz
       << ", Text=" << pagesz*textsz
       << ", Data=" << pagesz*datasz
       << ", Libraries=" << pagesz*libsz
       << ", Dirty pages=" << pagesz*dtsz
	    << std::endl;
}
#else
MemoryReport::MemoryReport(const std::string & /* s */) {
}
#endif
