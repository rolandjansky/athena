/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EnergyConservationTest.h"

// For determining if something was a primary
#include "MCTruth/TrackHelper.h"
// For setting the error state if need be
#include "EventInfo/EventInfo.h"

// For the G4 information
#include "G4Step.hh"
#include "G4Track.hh"

// for std::fabs
#include <cmath>


void EnergyConservationTest::EndOfEvent(const G4Event*)
{
  //  static StoreGateSvc * m_storeGate = 0;
  ATH_MSG_DEBUG( "Event info: " << e_in << " in, " << e_out << " out, " << e_dep << " deposited." );
  if ( std::fabs( e_in-e_out-e_dep ) > 10000. ) { // 10 GeV threshold, which is really pretty generous!
    ATH_MSG_ERROR( "Energy conservation error! " << e_in << " in, " << e_out << " out, " << e_dep << " deposited." );

    // Set error state in eventInfo
    const DataHandle<EventInfo> eic = 0;
    if (evtStore()->retrieve( eic ).isFailure() || !eic ){
      ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
    } else {
      // Gotta cast away the const... sadface
      EventInfo *ei = const_cast< EventInfo * > (&(*eic));
      ei->setErrorState(EventInfo::Core,EventInfo::Error);
      ATH_MSG_WARNING( "Set error state in event info!" );
    }
  }

  // reset
  e_in=0.;
  e_out=0.;
  e_dep=0.;

}

void EnergyConservationTest::Step(const G4Step* s)
{
  e_dep += s->GetTotalEnergyDeposit();
}

void EnergyConservationTest::PreTracking(const G4Track* t)
{
  TrackHelper theHelper(t);
  if(theHelper.IsPrimary()) e_in += t->GetTotalEnergy();
}

void EnergyConservationTest::PostTracking(const G4Track* t)
{
  // If it has any energy left *after* being tracked, it should have left the world
  if (!t->GetNextVolume()) e_out += t->GetTotalEnergy();
}


StatusCode EnergyConservationTest::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
