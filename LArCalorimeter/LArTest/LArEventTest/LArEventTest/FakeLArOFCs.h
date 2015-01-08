/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELAROFS
#define FAKELAROFS

#include "AthenaBaseComps/AthAlgorithm.h"

class FakeLArOFCs : public AthAlgorithm {
  
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
