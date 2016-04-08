/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HIPLArVolumeAccept.h"
#include <iostream>

#include "G4Event.hh"
#include "G4Step.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"


void HIPLArVolumeAccept::BeginOfEvent(const G4Event*)
{
  HIPacc=0;
}
void HIPLArVolumeAccept::EndOfEvent(const G4Event*)
{
  HIPevts++;
  if(!HIPacc)
  {
    //anEvent->SetEventAborted();
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    rmk->GetEventManager()->AbortCurrentEvent();
    rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
    HIPevts_failed++;
    ATH_MSG_INFO("HIPLArVolumeAccept: no HIP reach the LAr detector volumes. Event aborted.");
  }
}
void HIPLArVolumeAccept::BeginOfRun(const G4Run*)
{
  ATH_MSG_INFO("#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "## HIPLArVolumeAccept - BeginOfRun     ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "## This method will kill events where  ##"<<std::endl<<
		   "## no highly-ionizing particle reach   ##"<<std::endl<<
		   "## the LAr main volume                 ##"<<std::endl<<
		   "#########################################");
  HIPevts=0;
  HIPevts_failed=0;
}
void HIPLArVolumeAccept::EndOfRun(const G4Run*)
{
   ATH_MSG_INFO("#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    HIPLArVolumeAccept - EndOfRun    ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################");
   ATH_MSG_INFO(HIPevts       <<" events were processed by HIPLArVolumeAccept");
   ATH_MSG_INFO(HIPevts_failed<<" events were killed because they had no HIP in EMB or EMEC");
   double HIPfraction=1.*(HIPevts-HIPevts_failed)/HIPevts;
   ATH_MSG_INFO("HIP Acceptance: "<<HIPfraction);
          
}
void HIPLArVolumeAccept::Step(const G4Step* aStep)
{
  int PDGcode=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  // check if PDG code compatible with HIP (Monopole:411xxx0 or Qball:100xxxx0)
  if(abs(PDGcode)>4000000 && abs(PDGcode)<90000000)          // likely a HIP, no heavy ion 
  {
    // in EM calorimeter barrel mother volume (beyond presampler)  
    if(!HIPacc && aStep->GetTrack()->GetVolume()->GetName()=="Total LAR Volume") // EMB
    {
      HIPacc=1;
      ATH_MSG_INFO("HIPLArVolumeAccept: a particle with PDG code "
               <<PDGcode
	       <<" entered the volume "
	       <<aStep->GetTrack()->GetVolume()->GetName());
    }
    // in EM calorimeter endcap mother volume  
    if(!HIPacc && aStep->GetTrack()->GetVolume()->GetName()=="LArEndcapPos")     // EMEC
    {
      HIPacc=1;
      ATH_MSG_INFO("HIPLArVolumeAccept: a particle with PDG code " 
               <<PDGcode
	       <<" entered the volume "
	       <<aStep->GetTrack()->GetVolume()->GetName());
    }
  }
}



StatusCode HIPLArVolumeAccept::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
