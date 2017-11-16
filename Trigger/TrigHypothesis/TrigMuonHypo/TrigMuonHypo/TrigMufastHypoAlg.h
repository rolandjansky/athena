/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUFASTHYPOALG_H 
#define TRIGMUFASTHYPO_TRIGMUFASTHYPOALG_H 1 

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

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

   StatusCode  initialize() override;
   StatusCode  execute_r( const EventContext& context ) const override;
   StatusCode  finalize() override;
    
  private:
 
    TrigMufastHypoAlg(); 
    ToolHandleArray<TrigMufastHypoTool> m_hypoTools;

    SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey;

    SG::ReadHandleKey<std::vector< SG::View*>> m_viewsCollectionKey;
    SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_muFastCollectionKey;
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_previousDecisionsKey;
};

DECLARE_ALGORITHM_FACTORY( TrigMufastHypoAlg )
#endif
