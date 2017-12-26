/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <vector>
#include <string>
#include "SimpleTestClass.h"
#include "TestClassPrimitives.h"
#include "TestClassVectors.h"
#include "TestClassSimpleContainers.h"

class Token;

namespace pool {
  class IFileCatalog;
}

namespace pool {

  class TestDriver {
  public:
    TestDriver();
    ~TestDriver();
    void loadLibraries( const std::vector<std::string>& libraries );
    void write();
    void read();
    void readCollections();
    void updateObjects();
    void readBackUpdatedObjects();
    void clearCache();
    void readFileSizes();

  private:
    pool::IFileCatalog*   m_fileCatalog;
    std::string           m_fileName1;
    std::string           m_fileName2;
    int                   m_events;
    int                   m_eventsToCommitAndHold;
    std::vector< Token* > m_tokens;
    std::vector< SimpleTestClass > m_simpleTestClass;
    std::vector< TestClassPrimitives > m_testClassPrimitives;
    std::vector< TestClassVectors > m_testClassVectors;
    std::vector< TestClassSimpleContainers > m_testClassSimpleContainers;
  };

}

#endif
