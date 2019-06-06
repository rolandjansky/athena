/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_TOWERMAKERHLTROB_H
#define TRIGCALOREC_TOWERMAKERHLTROB_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"

namespace LVL1BS {
  
  class ITrigT1CaloDataAccessV2;
}

class L1BSTowerMakerHLTROB: public AthAlgorithm {
  
 public:
  
  L1BSTowerMakerHLTROB(const std::string & name, ISvcLocator* pSvcLocator);
  
  ~L1BSTowerMakerHLTROB();
  
  StatusCode initialize();      
  StatusCode execute();
  StatusCode finalize();
  
 private:
  
  ToolHandle<LVL1BS::ITrigT1CaloDataAccessV2> m_tool;
  
};

#endif
