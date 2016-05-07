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
#include "StoreGate/WriteHandle.h"

// For setting an error state in event info
#include "EventInfo/EventInfo.h"


LooperKiller::LooperKiller(const std::string& type, const std::string& name, const IInterface* parent) :
  UserActionBase(type,name,parent), m_max_steps(1000000), m_print_steps(100), m_count_steps(0),
  m_killed_tracks(0), m_verbose_level(1), m_abortEvent(true),
  m_setError(false) {

  declareProperty("MaxSteps", m_max_steps);
  declareProperty("PrintSteps",m_print_steps);
  declareProperty("VerboseLevel", m_verbose_level);
  declareProperty("AbortEvent", m_abortEvent);
  declareProperty("SetError", m_setError);

}


StatusCode LooperKiller::finalize()
{

  ATH_MSG_INFO( "******* Report from "<< name()<< " *******");
  ATH_MSG_INFO(" Set to kill tracks over " << m_max_steps << " steps");
  ATH_MSG_INFO(" and give " << m_print_steps << " steps of verbose output");
  ATH_MSG_INFO(" We killed " << m_killed_tracks << " tracks this run.");
  ATH_MSG_INFO(" Was set to " << (m_abortEvent?"":"not ") << "abort events and ");
  ATH_MSG_INFO( (m_setError?"":"not ") << "set an error state." );

  return StatusCode::SUCCESS;

}


void LooperKiller::Step(const G4Step* aStep)
{
  if (aStep->GetTrack()->GetCurrentStepNumber() < m_max_steps) {
    if (m_count_steps==0) return;
    // Track recovered...
    ATH_MSG_WARNING("Track finished on its own.  Congrats.  Moving on with the event.");
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
    ATH_MSG_WARNING("LooperKiller triggered!! Hold on to your hats!!!!!!!!" );
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

      // Set error state in eventInfo
      SG::WriteHandle<EventInfo> eic("McEventInfo");
      if (! eic.isValid()  ){
        ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
      } else {
	eic->setErrorState(EventInfo::Core,EventInfo::Error);
        ATH_MSG_WARNING( "Set error state in event info!" );
      }

    } // End of set error
  } // End of handling end of error time
}

StatusCode LooperKiller::queryInterface(const InterfaceID& riid, void** ppvInterface)
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


//=============================================================================
// New design of user actions for multithreading
//=============================================================================


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  LooperKiller::LooperKiller(const Config& config)
    : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                   "LooperKiller"),
      m_evtStore("StoreGateSvc/StoreGateSvc", "LooperKiller"),
      m_detStore("StoreGateSvc/DetectorStore", "LooperKiller"),
      m_config(config), m_report(), m_count_steps(0)
  {
  }

  //---------------------------------------------------------------------------
  void LooperKiller::processStep(const G4Step* aStep)
  {

    if (aStep->GetTrack()->GetCurrentStepNumber() < m_config.MaxSteps) {
      if (m_count_steps==0) return;
      // Track recovered...
      ATH_MSG_WARNING("Track finished on its own.  Congrats.  Moving on with the event.");
      m_count_steps = 0;
      G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
      tm->GetNavigatorForTracking()->SetVerboseLevel(0);
      tm->GetPropagatorInField()->SetVerboseLevel(0);
      G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
      rmk->GetTrackingManager()->SetVerboseLevel(0);
      rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(0);
      rmk->GetStackManager()->SetVerboseLevel(0);
      return;
    } else if (aStep->GetTrack()->GetCurrentStepNumber() == m_config.MaxSteps) {
      ATH_MSG_WARNING("LooperKiller triggered!! Hold on to your hats!!!!!!!!" );
    }

    G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
    tm->GetNavigatorForTracking()->SetVerboseLevel(m_config.VerboseLevel);
    tm->GetPropagatorInField()->SetVerboseLevel(m_config.VerboseLevel);

    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    rmk->GetTrackingManager()->SetVerboseLevel(m_config.VerboseLevel);
    rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(m_config.VerboseLevel);
    rmk->GetStackManager()->SetVerboseLevel(m_config.VerboseLevel);

    m_count_steps++;

    if (m_count_steps>m_config.PrintSteps) {
      m_count_steps = 0;
      m_report.killed_tracks++;
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
      tm->GetNavigatorForTracking()->SetVerboseLevel(0);
      tm->GetPropagatorInField()->SetVerboseLevel(0);
      rmk->GetTrackingManager()->SetVerboseLevel(0);
      rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(0);
      rmk->GetStackManager()->SetVerboseLevel(0);

      // Bail out...
      if (m_config.AbortEvent){
	rmk->GetEventManager()->AbortCurrentEvent();
	rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
      }
      if (m_config.SetError){
	
	// Set error state in eventInfo
	SG::WriteHandle<EventInfo> eic("McEventInfo");
	if (! eic.isValid()){
	  ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
	} else {
	  eic->setErrorState(EventInfo::Core,EventInfo::Error);
	  ATH_MSG_WARNING( "Set error state in event info!" );
	}
	
      } // End of set error
    } // End of handling end of error time
  }

} // namespace G4UA
