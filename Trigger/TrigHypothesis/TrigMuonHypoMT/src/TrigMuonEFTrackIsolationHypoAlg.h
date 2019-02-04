/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUONEFTRACKISOLATIONHYPOALG_H 
#define TRIGMUFASTHYPO_TRIGMUONEFTRACKISOLATIONHYPOALG_H 1 

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "AthViews/View.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODMuon/MuonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigMuonEFTrackIsolationHypoTool.h"
#include "DecisionHandling/HypoBase.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFTrackIsolationHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFTrackIsolationHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigMuonEFTrackIsolationHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;
    
  private:
 
   TrigMuonEFTrackIsolationHypoAlg(); 
   
   ToolHandleArray<TrigMuonEFTrackIsolationHypoTool> m_hypoTools { this, "HypoTools", {}, "Tools to perform selection" };

   SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey {
	this, "EFMuonsName", "MuonEFInfo", "Name of EF muons conatiner" };

};

#endif
