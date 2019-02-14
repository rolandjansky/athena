/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFCOMBINERHYPO_TRIGMUONEFCOMBINERHYPOALG_H 
#define TRIGMUONEFCOMBINERHYPO_TRIGMUONEFCOMBINERHYPOALG_H 1 

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "TrigMuonEFCombinerHypoTool.h"

#include "DecisionHandling/HypoBase.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFCombinerHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFCombinerHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigMuonEFCombinerHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;
    
  private:
 
    TrigMuonEFCombinerHypoAlg(); 
    ToolHandleArray<TrigMuonEFCombinerHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey{
	this, "MuonDecisions", "MuonEFCombiner_MuonData", "Name of the input data on xAOD::MuonContainer produced by MuonCreatorAlg"};

};

#endif
