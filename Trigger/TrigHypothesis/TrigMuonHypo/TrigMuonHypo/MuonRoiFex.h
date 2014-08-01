/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONROIFEX_H 
#define MUONROIFEX_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"

class TriggerElement;

class MuonRoiFex: public HLT::FexAlgo
{
   public:
      MuonRoiFex(const std::string & name, ISvcLocator* pSvcLocator);
      ~MuonRoiFex();
      
      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
				HLT::TriggerElement* output);
      HLT::ErrorCode hltFinalize();
      
   private:
      bool    m_modeAcceptAll;
      bool    m_modeRpcOnly;
      bool    m_modeRpcHorizontal;
      double  m_dPhiForRpcHorizontal; 
};

#endif

