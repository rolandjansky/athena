/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUONEFINVMASSHYPOALG_H 
#define TRIGMUONHYPOMT_TRIGMUONEFINVMASSHYPOALG_H 1 

#include <string>

#include "StoreGate/ReadHandleKey.h"
#include "TrigMuonEFInvMassHypoTool.h"
#include "DecisionHandling/HypoBase.h"

class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFInvMassHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFInvMassHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigMuonEFInvMassHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
    
  private:
 
    ToolHandleArray<TrigMuonEFInvMassHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey{
	this, "MuonDecisions", "MuonEFInvMass_MuonData", "Name of the input data on xAOD::MuonContainer produced by MuonCreatorAlg"};

};

#endif
