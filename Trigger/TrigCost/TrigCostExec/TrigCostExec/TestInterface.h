/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_TESTINTERFACE_H
#define ANP_TESTINTERFACE_H

#include <string>

namespace Anp
{
  class TestInterface {
  public:
    
    TestInterface() {}
    ~TestInterface() {}
    
    void RunTest(const std::string &config_path, int nevent);
  };
}

#endif
