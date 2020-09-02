/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUROI_H
#define TRIGMUROI_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigInterfaces/AllTEAlgo.h"

#include "GaudiKernel/ToolHandle.h"

#include "TrigT1Interfaces/RecMuonRoiSvc.h"

#include "TrigMuonToolInterfaces/ITrigMuonRoITool.h"

#include <string>
#include <stdint.h>

namespace HLT {
  class TriggerElement;
}

class TrigmuRoI : public HLT::AllTEAlgo
{
   public:

      TrigmuRoI(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor
      
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize
      
      HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*input*/,
				unsigned int output);

      HLT::ErrorCode hltEndEvent();

   private:
   
      MsgStream m_log;

      /// Source identifiers for ROB fragments
      IntegerProperty                  m_daqCTPROBid ;
      IntegerProperty                  m_daqMuCTPiROBid ;

      IntegerProperty                  m_minValueForOutOfTimeBC;
      IntegerProperty                  m_maxValueForOutOfTimeBC;
      
      bool m_executed;
      
      ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool;
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;
      
      std::vector<int> m_RpcOutOfTime;
      std::vector<int> m_TgcOutOfTime;
      
      std::vector<float> m_etaOutOfTime;
      std::vector<float> m_phiOutOfTime;
};

#endif
