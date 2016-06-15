/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "getMemUsage.h"
#include <unistd.h>
#include <fstream>
#include <sstream>

namespace PSUtils
{
  MemStruct getMemUsage()
  {
    int pid = getpid();
    std::ostringstream ost;
    ost<<"/proc/"<<pid<<"/statm";
    std::ifstream ifile( ( ost.str() ).c_str(), std::ifstream::in );
    
    MemStruct ms;
    ifile>>ms;
    ifile.close();
    return ms;
  }

  std::ostream& operator<<(std::ostream& os, const MemStruct& s)
  {
    // sizes in pages
    /*
    os <<
      "  totalSize "      <<s.totalSize<<
      "  physicalMemSize "<<s.physicalMemSize<<
      "  sharedMemSize "  <<s.sharedMemSize<<
      "  textSize "       <<s.textSize<<
      "  sharedLibSize "  <<s.sharedLibSize<<
      "  stackSize "      <<s.stackSize<<
      "  nDirtyPages "    <<s.nDirtyPages<<
      '\n';
    */
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
