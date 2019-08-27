/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERMUFASTALG_H 
#define TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERMUFASTALG_H 1 

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/View.h"

#include "TrigL2MuonOverlapRemoverTool.h"
#include "DecisionHandling/HypoBase.h"

class TriggerElement;

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
