/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HIPLArVolumeAccept.h"
#include <iostream>

#include "G4Event.hh"
#include "G4Step.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"

static HIPLArVolumeAccept HIPL1A("HIPLArVolumeAccept");

void HIPLArVolumeAccept::BeginOfEventAction(const G4Event*)
{
  HIPacc=0;
}
void HIPLArVolumeAccept::EndOfEventAction(const G4Event*)
{
  HIPevts++;
  if(!HIPacc)
  {
    //anEvent->SetEventAborted();
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    rmk->GetEventManager()->AbortCurrentEvent();
    rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
    HIPevts_failed++;
    std::cout<<"HIPLArVolumeAccept: no HIP reach the LAr detector volumes. Event aborted."<<std::endl;
  }
}
void HIPLArVolumeAccept::BeginOfRunAction(const G4Run*)
{
	std::cout<<"#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "## HIPLArVolumeAccept - BeginOfRun     ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "## This method will kill events where  ##"<<std::endl<<
		   "## no highly-ionizing particle reach   ##"<<std::endl<<
		   "## the LAr main volume                 ##"<<std::endl<<
		   "#########################################"<<std::endl;
  HIPevts=0;
  HIPevts_failed=0;
}
void HIPLArVolumeAccept::EndOfRunAction(const G4Run*)
{
	std::cout<<"#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    HIPLArVolumeAccept - EndOfRun    ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################"<<std::endl;
   std::cout<<HIPevts       <<" events were processed by HIPLArVolumeAccept"<<std::endl;
   std::cout<<HIPevts_failed<<" events were killed because they had no HIP in EMB or EMEC"<<std::endl;
   double HIPfraction=1.*(HIPevts-HIPevts_failed)/HIPevts;
   std::cout<<"HIP Acceptance: "<<HIPfraction<<std::endl;
          
}
void HIPLArVolumeAccept::SteppingAction(const G4Step* aStep)
{
  int PDGcode=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  // check if PDG code compatible with HIP (Monopole:411xxx0 or Qball:100xxxx0)
  if(abs(PDGcode)>4000000 && abs(PDGcode)<90000000)          // likely a HIP, no heavy ion 
  {
    // in EM calorimeter barrel mother volume (beyond presampler)  
    if(!HIPacc && aStep->GetTrack()->GetVolume()->GetName()=="Total LAR Volume") // EMB
    {
      HIPacc=1;
      std::cout<<"HIPLArVolumeAccept: a particle with PDG code "
               <<PDGcode
	       <<" entered the volume "
	       <<aStep->GetTrack()->GetVolume()->GetName()<<std::endl;
    }
    // in EM calorimeter endcap mother volume  
    if(!HIPacc && aStep->GetTrack()->GetVolume()->GetName()=="LArEndcapPos")     // EMEC
    {
      HIPacc=1;
      std::cout<<"HIPLArVolumeAccept: a particle with PDG code " 
               <<PDGcode
	       <<" entered the volume "
	       <<aStep->GetTrack()->GetVolume()->GetName()<<std::endl;
    }
  }
}
