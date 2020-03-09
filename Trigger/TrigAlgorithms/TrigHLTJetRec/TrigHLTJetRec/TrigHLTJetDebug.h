/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTJETDEBUG_H
#define TRIGHLTJETREC_TRIGHLTJETDEBUG_H

#include "TrigInterfaces/HypoAlgo.h"

class TrigHLTJetDebug: public HLT::HypoAlgo {
	
public:
  TrigHLTJetDebug(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetDebug();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);


private:
  std::string m_chainName{""};
  std::string m_eventShapeSGKey{""};

}; 

#endif
