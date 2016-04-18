/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <vector>
#include <string>

class Token;

namespace pool {
  class IFileCatalog;

  class TestDriver {
  public:
    TestDriver();
    ~TestDriver();
    void loadLibraries( const std::vector<std::string>& libraries );
    void write();
    void read();
  private:
    IFileCatalog*         m_fileCatalog;
    std::string           m_fileName;
    int                   m_events;
    std::vector< Token* > m_tokens;
  };

}

#endif
