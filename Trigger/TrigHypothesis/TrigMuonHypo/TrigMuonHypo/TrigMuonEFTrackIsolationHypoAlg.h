/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUONEFTRACKISOLATIONHYPOALG_H 
#define TRIGMUFASTHYPO_TRIGMUONEFTRACKISOLATIONHYPOALG_H 1 

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "AthViews/View.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/TrigCompositeUtils.h"

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
   virtual StatusCode  execute_r( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;
    
  private:
 
    TrigMuonEFTrackIsolationHypoAlg(); 

};

#endif
