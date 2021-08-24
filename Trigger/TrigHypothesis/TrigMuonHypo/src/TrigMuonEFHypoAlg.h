/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPO_TRIGMUONEFHYPOALG_H
#define TRIGMUONHYPO_TRIGMUONEFHYPOALG_H 1

#include "TrigMuonEFHypoTool.h"
#include "DecisionHandling/HypoBase.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
    bool checkOvlp(const xAOD::Muon *mu, std::vector<TrigMuonEFHypoTool::MuonEFInfo> toolInput) const;
    ToolHandleArray<TrigMuonEFHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey{
	this, "MuonDecisions", "MuonEFCombiner_MuonData", "Name of the input data on xAOD::MuonContainer produced by MuonCreatorAlg"};
    Gaudi::Property<bool> m_mapToPrevDec{
      this, "MapToPreviousDecisions", false, "Map to decisions from previous decisions (needed if IM has mergeUsingFeature=True)"};
    Gaudi::Property< bool > m_checkOvlp {
      this, "DoOverlapRemoval", true, "Check for overlapping muons"};


};

#endif
