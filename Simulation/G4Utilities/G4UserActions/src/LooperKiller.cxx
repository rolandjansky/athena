/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/LooperKiller.h"
#include <iostream>
#include "G4RunManagerKernel.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4StackManager.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

// For setting an error state in event info
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

static LooperKiller lkill("LooperKiller");


void LooperKiller::BeginOfEventAction(const G4Event*) {
  if (!m_initted) ParseProperties();
}


void LooperKiller::EndOfEventAction(const G4Event*) { }


void LooperKiller::BeginOfRunAction(const G4Run*) {
  if (!m_initted) ParseProperties();
}


void LooperKiller::EndOfRunAction(const G4Run*)
{
  log() << MSG::INFO << "******* Report from LooperKiller *******" << std::endl
         << " Set to kill tracks over " << m_max_steps << " steps" << std::endl
         << " and give " << m_print_steps << " steps of verbose output" << std::endl
         << " We killed " << m_killed_tracks << " tracks this run." << std::endl
         << " Was set to " << (m_abortEvent?"":"not ") << "abort events and " 
         << (m_setError?"":"not ") << "set an error state." << endreq;
}


void LooperKiller::SteppingAction(const G4Step* aStep)
{
  if (aStep->GetTrack()->GetCurrentStepNumber() < m_max_steps) {
    if (m_count_steps==0) return;
    // Track recovered...
    log()<<MSG::WARNING<<"Track finished on its own.  Congrats.  Moving on with the event."<<endreq;
    m_count_steps = 0;
    G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
    tm->GetNavigatorForTracking()->SetVerboseLevel(0);
    tm->GetPropagatorInField()->SetVerboseLevel(0);
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    rmk->GetTrackingManager()->SetVerboseLevel(0);
    rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(0);
    rmk->GetStackManager()->SetVerboseLevel(0);
    return;
  } else if (aStep->GetTrack()->GetCurrentStepNumber() == m_max_steps) {
    log()<<MSG::WARNING << "LooperKiller triggered!! Hold on to your hats!!!!!!!!" << endreq;
  }

  G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
  tm->GetNavigatorForTracking()->SetVerboseLevel(m_verbose_level);
  tm->GetPropagatorInField()->SetVerboseLevel(m_verbose_level);

  G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
  rmk->GetTrackingManager()->SetVerboseLevel(m_verbose_level);
  rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(m_verbose_level);
  rmk->GetStackManager()->SetVerboseLevel(m_verbose_level);

  m_count_steps++;

  if (m_count_steps>m_print_steps) {
    m_count_steps = 0;
    m_killed_tracks++;
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    tm->GetNavigatorForTracking()->SetVerboseLevel(0);
    tm->GetPropagatorInField()->SetVerboseLevel(0);
    rmk->GetTrackingManager()->SetVerboseLevel(0);
    rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(0);
    rmk->GetStackManager()->SetVerboseLevel(0);

    // Bail out...
    if (m_abortEvent){
      rmk->GetEventManager()->AbortCurrentEvent();
      rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
    }
    if (m_setError){

      static StoreGateSvc * m_storeGate = 0;
      if (!m_storeGate){
        ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
        StatusCode status = svcLocator->service("StoreGateSvc", m_storeGate);
        if (status.isFailure()){
          log()<<MSG::WARNING<< "HitWrapper::EndOfEventAction could not access StoreGateSvc"<<endreq;
        }
      }

      // Set error state in eventInfo
      const DataHandle<EventInfo> eic = 0;
      if (!m_storeGate || m_storeGate->retrieve( eic ).isFailure() || !eic ){
        log() << MSG::WARNING << "Failed to retrieve EventInfo" << endreq;
      } else {
        // Gotta cast away the const... sadface
        EventInfo *ei = const_cast< EventInfo * > (&(*eic));
        ei->setErrorState(EventInfo::Core,EventInfo::Error);
        log() << MSG::WARNING << "Set error state in event info!" << endreq;
      }

    } // End of set error
  } // End of handling end of error time
}


void LooperKiller::ParseProperties() {
  if (theProperties.find("MaxSteps")==theProperties.end()) {
    theProperties["MaxSteps"]="1000000";
  }
  if (theProperties.find("PrintSteps")==theProperties.end()) {
    theProperties["PrintSteps"]="100";
  }
  if (theProperties.find("VerboseLevel")==theProperties.end()) {
    theProperties["VerboseLevel"]="4";
  }
  if (theProperties.find("AbortEvent")==theProperties.end()) {
    theProperties["AbortEvent"]="1";
  }
  if (theProperties.find("SetError")==theProperties.end()) {
    theProperties["SetError"]="0";
  }

  m_max_steps     = strtol(theProperties["MaxSteps"].c_str(),0,0);
  m_print_steps   = strtol(theProperties["PrintSteps"].c_str(),0,0);
  m_verbose_level = strtol(theProperties["VerboseLevel"].c_str(),0,0);
  m_abortEvent    = strtol(theProperties["AbortEvent"].c_str(),0,0)>0;
  m_setError      = strtol(theProperties["SetError"].c_str(),0,0)>0;
  m_initted = true;


}

