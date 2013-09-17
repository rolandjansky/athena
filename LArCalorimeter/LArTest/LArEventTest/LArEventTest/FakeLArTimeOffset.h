/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELARTIMEOFFSET
#define FAKELARTIMEOFFSET

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"

class FakeLArTimeOffset : public Algorithm {
  
 public:
  FakeLArTimeOffset(const std::string & name, ISvcLocator * pSvcLocator);
  ~FakeLArTimeOffset();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  float m_globalOffset;
  std::vector<unsigned> m_FEBids;
  std::vector<float> m_FEBtimeOffsets;
  float m_globalTimeOffset;
  
  std::string m_keyFebOffset, m_keyGlobalOffset;
};

#endif
