/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFMSONLYHYPO_TRIGMUONEFMSONLYHYPOALG_H 
#define TRIGMUONEFMSONLYHYPO_TRIGMUONEFMSONLYHYPOALG_H 1 

#include "TrigMuonEFMSonlyHypoTool.h"
#include "DecisionHandling/HypoBase.h"

class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFMSonlyHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFMSonlyHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigMuonEFMSonlyHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;
    
  private:
 
    ToolHandleArray<TrigMuonEFMSonlyHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey{
	this, "MuonDecisions", "MuonEFMSonly_MuonData", "Name of the input data on xAOD::MuonContainer produced by MuonCreatorAlg"};

};

#endif
