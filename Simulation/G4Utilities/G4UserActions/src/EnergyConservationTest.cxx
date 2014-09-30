/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/EnergyConservationTest.h"

// For determining if something was a primary
#include "MCTruth/TrackHelper.h"

// For the G4 information
#include "G4Track.hh"
#include "G4Step.hh"

// For setting the error state if need be
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

// for std::fabs
#include <cmath>

static EnergyConservationTest ect1("EnergyConservationTest");

void EnergyConservationTest::BeginOfEventAction(const G4Event*)
{
  e_in=0.;
  e_out=0.;
  e_dep=0.;
}

void EnergyConservationTest::EndOfEventAction(const G4Event*)
{
  static StoreGateSvc * m_storeGate = 0;
  ATH_MSG_DEBUG( "Event info: " << e_in << " in, " << e_out << " out, " << e_dep << " deposited." );
  if ( std::fabs( e_in-e_out-e_dep ) > 10000. ) { // 10 GeV threshold, which is really pretty generous!
    ATH_MSG_ERROR( "Energy conservation error! " << e_in << " in, " << e_out << " out, " << e_dep << " deposited." );

    if (!m_storeGate){
      ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
      StatusCode status = svcLocator->service("StoreGateSvc", m_storeGate);
      if (status.isFailure()){
        ATH_MSG_WARNING( "Could not access StoreGateSvc" );
      }
    }

    // Set error state in eventInfo
    const DataHandle<EventInfo> eic = 0;
    if (!m_storeGate || m_storeGate->retrieve( eic ).isFailure() || !eic ){
      ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
    } else {
      // Gotta cast away the const... sadface
      EventInfo *ei = const_cast< EventInfo * > (&(*eic));
      ei->setErrorState(EventInfo::Core,EventInfo::Error);
      ATH_MSG_WARNING( "Set error state in event info!" );
    }
  }

}

void EnergyConservationTest::SteppingAction(const G4Step* s)
{
  e_dep += s->GetTotalEnergyDeposit();
}

void EnergyConservationTest::PreUserTrackingAction(const G4Track* t)
{
  TrackHelper theHelper(t);
  if(theHelper.IsPrimary()) e_in += t->GetTotalEnergy();
}

void EnergyConservationTest::PostUserTrackingAction(const G4Track* t)
{
  // If it has any energy left *after* being tracked, it should have left the world
  if (!t->GetNextVolume()) e_out += t->GetTotalEnergy();
}

