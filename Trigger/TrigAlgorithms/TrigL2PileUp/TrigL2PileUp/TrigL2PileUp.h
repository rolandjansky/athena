/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
* @Project: HLT, PESA algorithms
* @Package: TrigL2PileUp
* @Class  : TrigL2PileUp
*
* @author Alex Pinder     <alexander.pinder@cern.ch>>     - Oxford
* @author George Lewis    <ghl@cern.ch>>                  - New York University
*
* File and Version Information:
* $Id: TrigL2PileUp.h,v 1.00 2010-16-07 14:00:00 ghl Exp $
**********************************************************************************/

#ifndef TrigL2PileUp_L2PileUp_h
#define TrigL2PileUp_L2PileUp_h

#include <string>
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigMissingEtEvent/TrigPileUpInfo.h" 

namespace HLT {
  class TriggerElement;
}


class  TrigL2PileUp : public HLT::AllTEAlgo {
  
 public:
  
  TrigL2PileUp(const std::string& name, ISvcLocator* pSvcLocator); 
  
  HLT::ErrorCode hltInitialize(); // { return HLT::OK; }  // Want to do a printout here
  HLT::ErrorCode hltFinalize();
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			    unsigned int output);
  
 private:
  
  std::string m_featureLabel;
  TrigPileUpInfo* m_feature;
  
  
};

#endif
