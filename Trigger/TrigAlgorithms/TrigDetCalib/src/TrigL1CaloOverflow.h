/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDETCALIB_TRIGL1CALOOVERFLOW_H
#define TRIGDETCALIB_TRIGL1CALOOVERFLOW_H

#include <string>
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

namespace HLT {
  class TriggerElement;
}

/**
 * Algorithm that accepts all events with L1Calo TOB overflows 
 **/

class TrigL1CaloOverflow : public HLT::AllTEAlgo {
  
public:
  TrigL1CaloOverflow(const std::string& name, ISvcLocator* pSvcLocator); 
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize() {return HLT::OK;}
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                            unsigned int output);
  
private:
  ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool;
  bool m_ignoreAbortGap;
  bool m_acceptCMXOverflows;
  bool m_acceptCaloRoIBOverflows;
  bool m_acceptLvl1Consist;
};


#endif 
