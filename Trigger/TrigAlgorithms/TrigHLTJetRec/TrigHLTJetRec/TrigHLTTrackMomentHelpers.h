/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTTRACKMOMENTHELPERS_H
#define TRIGHLTJETREC_TRIGHLTTRACKMOMENTHELPERS_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

class IJetExecuteTool;

class TrigHLTTrackMomentHelpers: public HLT::FexAlgo {

 public:
  TrigHLTTrackMomentHelpers(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTTrackMomentHelpers();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();
 
private:

  // track vertex association tool to create track vertex association container used by jet track-based modifiers. 
  ToolHandle<IJetExecuteTool> m_tvassocTool;

  // fixed SG keys/aliases that tools are configured with
  std::string m_trackSGKey;
  std::string m_primVtxSGKey;

}; 
#endif
