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



#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{ 

  EnergyConservationTest::EnergyConservationTest():
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"EnergyConservationTest"),
    m_evtStore("StoreGateSvc/StoreGateSvc","EnergyConservationTest"),
    m_detStore("StoreGateSvc/DetectorStore","EnergyConservationTest"),
    e_in(0), e_out(0), e_dep(0){}
  
  void EnergyConservationTest::PreUserTrackingAction(const G4Track* t){
    TrackHelper theHelper(t);
    if(theHelper.IsPrimary()) e_in += t->GetTotalEnergy();
  }
  
  void EnergyConservationTest::PostUserTrackingAction(const G4Track* t){
    // If it has any energy left *after* being tracked, it should have left the world
    if (!t->GetNextVolume()) e_out += t->GetTotalEnergy();
  }
  
  void EnergyConservationTest::EndOfEventAction(const G4Event*){
    
    ATH_MSG_DEBUG( "Event info: " << e_in << " in, " << e_out << " out, " << e_dep << " deposited." );
    if ( std::fabs( e_in-e_out-e_dep ) > 10000. ) { // 10 GeV threshold, which is really pretty generous!
      ATH_MSG_ERROR( "Energy conservation error! " << e_in << " in, " << e_out << " out, " << e_dep << " deposited." );
      
      // Set error state in eventInfo
      EventInfo* ei = 0;
      if (m_evtStore->retrieve( ei ).isFailure() || !ei ){
	ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
      } else {
	// Gotta cast away the const... sadface
	//EventInfo *ei = const_cast< EventInfo * > (&(*eic));
	ei->setErrorState(EventInfo::Core,EventInfo::Error);
	ATH_MSG_WARNING( "Set error state in event info!" );
      }
    }

    // reset
    e_in=0.;
    e_out=0.;
    e_dep=0.;

  }
  
  void EnergyConservationTest::UserSteppingAction(const G4Step* s){
    e_dep += s->GetTotalEnergyDeposit();
  }
  
} // namespace G4UA 
