/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUCOMBHYPOALG_H
#define TRIGMUONHYPOMT_TRIGMUCOMBHYPOALG_H 1

#include "TrigmuCombHypoTool.h"
#include "DecisionHandling/HypoBase.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigmuCombHypoAlg
   : public ::HypoBase
{
  public:

   TrigmuCombHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
    ToolHandleArray<TrigmuCombHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_muCombKey {
	this, "MuonL2CBInfoFromMuCombAlg", "MuonL2CBInfo", "Name of the input data produced by muCombMT"};
};

#endif
