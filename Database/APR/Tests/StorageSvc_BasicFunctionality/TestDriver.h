/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include "StorageSvc/DbType.h"

#include <vector>
#include <string>

class TestDriver {
public:
  TestDriver();
  ~TestDriver();
  void loadLibraries( const std::vector<std::string>& libraries );
  void testWriting();
  void testReading();

  // default values for this test
  std::string           m_filename      = "pool_test.root";
  std::string           m_containerName = "MyContainer";
  int                   m_nObjects      = 100;

  pool::DbType          m_storageType   = pool::ROOTTREE_StorageType;

};

#endif
