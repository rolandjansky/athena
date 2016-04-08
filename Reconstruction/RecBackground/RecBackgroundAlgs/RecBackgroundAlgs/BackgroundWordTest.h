/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECBACKGROUNDALGS_CHBACKGROUNDWORDTEST
#define RECBACKGROUNDALGS_CHBACKGROUNDWORDTEST


#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class BackgroundWordTest : public AthAlgorithm
{

 public:

  BackgroundWordTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~BackgroundWordTest();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
};

#endif
