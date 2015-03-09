/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELARTIMEOFFSET
#define FAKELARTIMEOFFSET

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"

class FakeLArTimeOffset : public AthAlgorithm {
  
 public:
  FakeLArTimeOffset(const std::string & name, ISvcLocator * pSvcLocator);
  ~FakeLArTimeOffset();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  std::vector<unsigned> m_FEBids;
  std::vector<float> m_FEBtimeOffsets;
  float m_globalTimeOffset;
  
  std::string m_keyFebOffset, m_keyGlobalOffset;
};

#endif
