/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HIPLArVolumeAccept.h"

#include "G4Event.hh"
#include "G4Step.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"


namespace G4UA
{

  //---------------------------------------------------------------------------
  HIPLArVolumeAccept::HIPLArVolumeAccept()
    : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                   "HIPLArVolumeAccept")
    , m_report()
    , m_HIPacc(false)
  {}

  //---------------------------------------------------------------------------
  void HIPLArVolumeAccept::UserSteppingAction(const G4Step* aStep)
  {
    int PDGcode=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

    // check if PDG code compatible with HIP (Monopole:411xxx0 or Qball:100xxxx0)
    if(abs(PDGcode)>4000000 && abs(PDGcode)<90000000)          // likely a HIP, no heavy ion
      {
        // in EM calorimeter barrel mother volume (beyond presampler)
        if(!m_HIPacc && aStep->GetTrack()->GetVolume()->GetName()=="Total LAR Volume") // EMB
          {
            m_HIPacc=1;
            ATH_MSG_INFO("HIPLArVolumeAccept: a particle with PDG code "
                         <<PDGcode
                         <<" entered the volume "
                         <<aStep->GetTrack()->GetVolume()->GetName());
          }
        // in EM calorimeter endcap mother volume
        if(!m_HIPacc && aStep->GetTrack()->GetVolume()->GetName()=="LArEndcapPos")     // EMEC
          {
            m_HIPacc=1;
            ATH_MSG_INFO("HIPLArVolumeAccept: a particle with PDG code "
                         <<PDGcode
                         <<" entered the volume "
                         <<aStep->GetTrack()->GetVolume()->GetName());
          }
      }

  }

  //---------------------------------------------------------------------------
  void HIPLArVolumeAccept::BeginOfEventAction(const G4Event*){
    m_HIPacc = false;
  }

  //---------------------------------------------------------------------------
  void HIPLArVolumeAccept::EndOfEventAction(const G4Event*)
  {
    m_report.HIPevts++;
    if(!m_HIPacc)
      {
        G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
        rmk->GetEventManager()->AbortCurrentEvent();
        rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
        m_report.HIPevts_failed++;
        ATH_MSG_INFO("HIPLArVolumeAccept: no HIP reach the LAr detector volumes. Event aborted.");
      }
  }

} // namespace G4UA
