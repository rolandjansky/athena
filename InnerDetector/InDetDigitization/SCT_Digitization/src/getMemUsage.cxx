/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "getMemUsage.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

namespace PSUtils
{
  MemStruct getMemUsage()
  {
    int pid = getpid();
    const std::string ost="/proc/"+std::to_string(pid)+"/statm";
    std::ifstream ifile( ost.c_str(), std::ifstream::in );
    
    MemStruct ms;
    ifile>>ms;
    ifile.close();
    return ms;
  }

  std::ostream& operator<<(std::ostream& os, const MemStruct& s)
  {
    // sizes in pages
   
    os <<
      "  totalSize "      <<s.totalSize<<
      "  physicalMemSize "<<s.physicalMemSize<<
      "  sharedMemSize "  <<s.sharedMemSize<<
      "  stackSize "      <<s.stackSize;
       return os;
  }
  std::istream& operator>>(std::istream& is, MemStruct& s)
  {
    is
      >>s.totalSize
      >>s.physicalMemSize
      >>s.sharedMemSize
      >>s.textSize
      >>s.sharedLibSize
      >>s.stackSize
      >>s.nDirtyPages;
    return is;
  }
}
