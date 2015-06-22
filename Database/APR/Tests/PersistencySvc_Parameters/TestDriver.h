/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <map>
#include <string>

namespace pool {
  class IFileCatalog;

  class TestDriver {
  public:
    TestDriver();
    ~TestDriver();
    void write();
    void read();

  private:
    IFileCatalog*         m_fileCatalog;
    std::string           m_fileName;
    std::map< std::string, std::string > m_parameters;
  };

}

#endif
