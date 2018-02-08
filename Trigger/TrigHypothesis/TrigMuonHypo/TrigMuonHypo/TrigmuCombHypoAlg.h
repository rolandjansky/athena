/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUCOMBHYPO_TRIGMUCOMBHYPOALG_H 
#define TRIGMUCOMBHYPO_TRIGMUCOMBHYPOALG_H 1 

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/View.h"

#include "TrigMuonHypo/TrigmuCombHypoTool.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigmuCombHypoAlg
   : public ::AthReentrantAlgorithm
{
  public:

   TrigmuCombHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigmuCombHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute_r( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;
    
  private:
 
    TrigmuCombHypoAlg(); 
    ToolHandleArray<TrigmuCombHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
	this, "Decisions", "MuonL2CBDecisions", "Name of the decision objects produced by TrigmuCombHypo"};

    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_muonDecisionsKey{
	this, "L2MuonFastDecisions", "L2MuonFastDecisions", "Name of the decisions objects to input from TrigMufastHypo"};

    SG::ReadHandleKey<std::vector< SG::View*>> m_viewsKey{
	this, "ViewRoIs", "MUTrkViewRoIs", "Name of the views produced by EVCA"};

    SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_combinedKey{
	this, "MuonL2CBInfoFromMuCombAlg", "MuonL2CBInfo", "Name of the input data produced by muCombMT"};
};

DECLARE_ALGORITHM_FACTORY( TrigmuCombHypoAlg )
#endif
