/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONLATEMUROIHYPO_TRIGMUONLATEMUROIHYPOALG_H 
#define TRIGMUONLATEMUROIHYPO_TRIGMUONLATEMUROIHYPOALG_H 1 

#include <string>

#include "StoreGate/ReadHandleKey.h"
#include "TrigMuonLateMuRoIHypoTool.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonLateMuRoIHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonLateMuRoIHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual ~TrigMuonLateMuRoIHypoAlg();

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
    
  private:
 
    ToolHandleArray<TrigMuonLateMuRoIHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiKey{
	this, "LateRoIs", "rois", "Name of the out-of-time RoIs produced by TrigmuRoI"};

};

#endif
