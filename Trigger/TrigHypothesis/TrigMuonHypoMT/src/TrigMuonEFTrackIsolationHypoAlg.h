/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUONEFTRACKISOLATIONHYPOALG_H
#define TRIGMUONHYPOMT_TRIGMUONEFTRACKISOLATIONHYPOALG_H 1

#include "TrigMuonEFTrackIsolationHypoTool.h"
#include "DecisionHandling/HypoBase.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFTrackIsolationHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFTrackIsolationHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
   ToolHandleArray<TrigMuonEFTrackIsolationHypoTool> m_hypoTools { this, "HypoTools", {}, "Tools to perform selection" };

   SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey {
	this, "EFMuonsName", "MuonEFInfo", "Name of EF muons conatiner" };

};

#endif
