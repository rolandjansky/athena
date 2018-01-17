/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUFASTHYPOALG_H 
#define TRIGMUFASTHYPO_TRIGMUFASTHYPOALG_H 1 

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "TrigMuonHypo/TrigMufastHypoTool.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMufastHypoAlg
   : public ::AthReentrantAlgorithm
{
  public:

   TrigMufastHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigMufastHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute_r( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;
    
  private:
 
    TrigMufastHypoAlg(); 
    ToolHandleArray<TrigMufastHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
	this, "Decisions", "MufastHypo_Decisions", "Name of the decisions object attached by TrigMufastHypo"};

    SG::ReadHandleKey<std::vector< SG::View*>> m_viewsKey{
	this, "ViewRoIs", "MUViewRoIs", "Name of the input data on Views produced by EventCreatorAlgorithms"};

    SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_muFastKey{
	this, "MuFastDecisions", "MuFastAlg_MuonData", "Name of the input data on xAOD::L2StandAloneMuonContainer produced by MuFastSteering"};

    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiKey{
	this, "RoIs", "MURoIs", "Name of the input data from L1Decoder"};

    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_previousDecisionsKey{
	this, "L1Decisions", "MuFastAlg_Decisions", "Name of the input decisions object produced by MuFastSteering"};
};

DECLARE_ALGORITHM_FACTORY( TrigMufastHypoAlg )
#endif
