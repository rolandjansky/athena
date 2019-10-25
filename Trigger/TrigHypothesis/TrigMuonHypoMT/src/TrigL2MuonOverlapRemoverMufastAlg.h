/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERMUFASTALG_H 
#define TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERMUFASTALG_H 1 

#include "TrigL2MuonOverlapRemoverTool.h"
#include "DecisionHandling/HypoBase.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

using namespace TrigCompositeUtils;
class TrigL2MuonOverlapRemoverMufastAlg
   : public ::HypoBase
{
  public:

   TrigL2MuonOverlapRemoverMufastAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigL2MuonOverlapRemoverMufastAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
    TrigL2MuonOverlapRemoverMufastAlg(); 
    ToolHandleArray<TrigL2MuonOverlapRemoverTool> m_OverlapRemoverTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_OverlapRemoverKey {
	this, "L2MuonOverlapInfoFromMuFastAlg", "MuonL2SAInfo", "Key for overlap removal from previousDecisions"};

};

#endif
