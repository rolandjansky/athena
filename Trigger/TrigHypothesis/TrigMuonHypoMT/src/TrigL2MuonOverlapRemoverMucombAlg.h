/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERMUCOMBALG_H 
#define TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERMUCOMBALG_H 1 

#include "TrigL2MuonOverlapRemoverTool.h"
#include "DecisionHandling/HypoBase.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigL2MuonOverlapRemoverMucombAlg
   : public ::HypoBase
{
  public:

   TrigL2MuonOverlapRemoverMucombAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigL2MuonOverlapRemoverMucombAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
    ToolHandleArray<TrigL2MuonOverlapRemoverTool> m_OverlapRemoverTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_OverlapRemoverKey {
	this, "L2MuonOverlapInfoFromMuCombAlg", "MuonL2CBInfo", "Key for overlap removal from previousDecisions"};

};

#endif
