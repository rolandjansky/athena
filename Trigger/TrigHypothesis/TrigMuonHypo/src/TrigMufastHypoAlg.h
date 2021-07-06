/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPO_TRIGMUFASTHYPOALG_H
#define TRIGMUONHYPO_TRIGMUFASTHYPOALG_H 1

#include "TrigMufastHypoTool.h"
#include "DecisionHandling/HypoBase.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMufastHypoAlg
   : public ::HypoBase
{
  public:

   TrigMufastHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
    ToolHandleArray<TrigMufastHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_muFastKey{
	this, "MuonL2SAInfoFromMuFastAlg", "MuonL2SAInfo", "Name of the input data on xAOD::L2StandAloneMuonContainer produced by MuFastSteering"};


};

#endif
