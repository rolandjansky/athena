/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKETBPHASE
#define FAKETBPHASE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TBEvent/TBPhase.h"

class FakeTBPhase : public AthAlgorithm {
  
 public:
  FakeTBPhase(const std::string & name, ISvcLocator * pSvcLocator);
  ~FakeTBPhase();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  float m_phase;
  short m_phaseInd;
  
  std::string m_keyTBPhase;
  
  float m_delta;  // to fake TTC reco
  int m_timeBins; // to fake TTC reco
};

#endif
