/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicOrFilter.h"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "StoreGate/ReadHandle.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

  G4CosmicOrFilter::G4CosmicOrFilter(const Config& config)
    : AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >("MessageSvc"), "G4CosmicOrFilter"),
      m_config(config), m_report()
  {
  }

  void G4CosmicOrFilter::EndOfEventAction(const G4Event*)
  {
    int counterOne(0), counterTwo(0), counterThree(0);
    //need way to get "and" or "or" in
    m_report.ntot++;

    SG::ReadHandle <TrackRecordCollection> coll(m_config.collectionName);
    if (! coll.isValid()) {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
    }
    else {
      counterOne = coll->size();
    }

    SG::ReadHandle <TrackRecordCollection> coll2(m_config.collectionName2);
    if (!coll2.isValid()) {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
    }
    else {
      counterTwo = coll2->size();
    }

    SG::ReadHandle <TrackRecordCollection> coll3(m_config.collectionName3);
    if (! coll3.isValid()) {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection" );
    }
    else {
      counterThree = coll3->size();
    }

    if (counterOne==0 && counterTwo==0 && counterThree==0) {
      ATH_MSG_INFO("aborting event due to failing OR filter");
      G4RunManager::GetRunManager()->AbortEvent();
    }
    else {
      m_report.npass++;
    }
  }

} // namespace G4UA
