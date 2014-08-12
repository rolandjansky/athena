/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PS_GETMEMUSAGE_H
#define PS_GETMEMUSAGE_H
#include <iosfwd>

namespace PSUtils
{
  struct MemStruct
  {
    int totalSize;
    int physicalMemSize;
    int sharedMemSize;
    int textSize;
    int sharedLibSize;
    int stackSize;
    int nDirtyPages;
  };
    

  MemStruct getMemUsage();
  std::ostream& operator<<(std::ostream&, const MemStruct&); 
  std::istream& operator>>(std::istream&, MemStruct&); 
}
#endif
