/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigMuisoHypo.h
// PACKAGE:  Trigger/TrigHypotheis/TrigMuonHypoMT/TrigMuisoHypo
//
// PURPOSE:  LVL2 Muon Isolation Hypothesis Algorithm: V4.0 
//           for athenaMT
// ********************************************************************
#ifndef TRIGMUISOHYPO_TRIGMUISOHYPOALG_H 
#define TRIGMUISOHYPO_TRIGMUISOHYPOALG_H 1

#include <string>

#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "DecisionHandling/HypoBase.h"
#include "TrigMuisoHypoTool.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuisoHypoAlg
   : public ::HypoBase
{
  public:

    TrigMuisoHypoAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator );

    virtual ~TrigMuisoHypoAlg();

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;
    virtual StatusCode  finalize() override;
    
  private:

  TrigMuisoHypoAlg();
  ToolHandleArray<TrigMuisoHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"};
 
  SG::ReadHandleKey<xAOD::L2IsoMuonContainer> m_muIsoKey {
	this, "MuonL2ISInfoName", "MuonL2ISInfo", "Name of the input data produced by muIso"};


};

#endif

