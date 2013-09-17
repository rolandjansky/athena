/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELAROFS
#define FAKELAROFS

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

class FakeLArOFCs : public Algorithm {
  
 public:
  FakeLArOFCs(const std::string & name, ISvcLocator * pSvcLocator);
  ~FakeLArOFCs();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  short m_maxSample, m_NSample;
  std::string m_keyOFC;
};
#endif
