/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_TRIGTOWERMAKER_H
#define TRIGCALOREC_TRIGTOWERMAKER_H

#include "TrigInterfaces/AllTEAlgo.h"

#include "GaudiKernel/ToolHandle.h"

namespace LVL1BS {
  
  class ITrigT1CaloDataAccessV2;
}

class TrigL1BSTowerMaker: public HLT::AllTEAlgo {
  
 public:
  
  TrigL1BSTowerMaker(const std::string & name, ISvcLocator* pSvcLocator);
  
  ~TrigL1BSTowerMaker();
  
  HLT::ErrorCode hltInitialize();      
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int output);
  HLT::ErrorCode hltFinalize();
  
 private:
  
  ToolHandle<LVL1BS::ITrigT1CaloDataAccessV2> m_tool;
  
};

#endif
