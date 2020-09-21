/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <vector>
#include <string>
#include "SimpleTestClass.h"

class Token;

namespace pool {
  class IFileCatalog;

  class TestDriver {
  public:
    TestDriver();
    ~TestDriver();
    void loadLibraries( const std::vector<std::string>& libraries );
    void write();
    void addlfns();
    void read();
  private:
    IFileCatalog*         m_fileCatalog;
    std::string           m_fileName1;
    std::string           m_lfn1;
    std::string           m_fileName2;
    std::string           m_lfn2;
    std::string           m_fileName3;
    std::string           m_lfn3;
    int                   m_events;
  };

}

#endif
