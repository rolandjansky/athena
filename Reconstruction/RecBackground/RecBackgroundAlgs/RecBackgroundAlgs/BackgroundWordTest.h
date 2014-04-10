/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECBACKGROUNDALGS_CHBACKGROUNDWORDTEST
#define RECBACKGROUNDALGS_CHBACKGROUNDWORDTEST


#include "GaudiKernel/Algorithm.h"
#include <string>

class StoreGateSvc;

class BackgroundWordTest : public Algorithm
{

 public:

  BackgroundWordTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~BackgroundWordTest();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  StoreGateSvc* m_storeGate;

};

#endif
