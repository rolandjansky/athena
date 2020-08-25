/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VerboseSelector.h"

#include "MCTruth/AtlasG4EventUserInfo.h"
#include "MCTruth/TrackHelper.h"

#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PropagatorInField.hh"
#include "G4RunManagerKernel.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TransportationManager.hh"
#include "G4VProcess.hh"

#include <iostream>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
// to retrieve the event number
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/ReadHandle.h"

namespace G4UA
{

  VerboseSelector::VerboseSelector(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"VerboseSelector"),
    m_evtStore("StoreGateSvc/StoreGateSvc", "VerboseSelector"),
    m_detStore("StoreGateSvc/DetectorStore", "LooperKiller"),
    m_config(config),m_evtCount(0)
  {}

  void VerboseSelector::BeginOfEventAction(const G4Event*)
  {
    SG::ReadHandle<EventInfo> eic("McEventInfo");
    if (!eic.isValid()){
      ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
    } else {
      m_evtCount = eic->event_ID()->event_number();
    }
  }

  void VerboseSelector::UserSteppingAction(const G4Step* aStep)
  {
    if(m_evtCount==(uint64_t)m_config.targetEvent||m_config.targetEvent<0){

      const G4ThreeVector& myPos = aStep->GetPostStepPoint()->GetPosition();
      if ( ( myPos.x() < m_config.Xmax && myPos.x() > m_config.Xmin &&
             myPos.y() < m_config.Ymax && myPos.y() > m_config.Ymin &&
             myPos.z() < m_config.Zmax && myPos.z() > m_config.Zmin ) ||
           m_config.verb==2){

        G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
        tm->GetNavigatorForTracking()->SetVerboseLevel(m_config.verboseLevel);
        tm->GetPropagatorInField()->SetVerboseLevel(m_config.verboseLevel);

        G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
        rmk->GetTrackingManager()->SetVerboseLevel(m_config.verboseLevel);
        rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(m_config.verboseLevel);
        rmk->GetStackManager()->SetVerboseLevel(m_config.verboseLevel);

        G4Track *tr = aStep->GetTrack();
        const G4ThreeVector& mom = tr->GetMomentumDirection();

        std::cout << "Moving " << tr->GetDefinition()->GetParticleName() << " at (" << myPos.x()
                  << ", " << myPos.y() << ", " << myPos.z() << ") to (" << mom.x() << ", " << mom.y()
                  << ", " << mom.z() << ") from ";
        if (aStep->GetPreStepPoint()->GetPhysicalVolume()) std::cout << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
        else std::cout << "noVolume";
        std::cout << " to ";
        if (aStep->GetPostStepPoint()->GetPhysicalVolume()) std::cout << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
        else std::cout << "outOfWorld";
        std::cout << " with KE=" << tr->GetKineticEnergy() << " pT=" << tr->GetMomentum().perp()
                  << " eta=" << tr->GetMomentum().eta() << " length " << aStep->GetStepLength() << " energy "
                  << aStep->GetTotalEnergyDeposit() << " with process ";
        if (aStep->GetPostStepPoint()->GetProcessDefinedStep()) std::cout << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
        else std::cout << "Unknown";
        std::cout << " from a ";
        if (tr->GetCreatorProcess()) std::cout << tr->GetCreatorProcess()->GetProcessName();
        else if (tr->GetParentID()==0) std::cout << "Generator";
        else std::cout << "pid="<<tr->GetParentID();
        if (tr->GetLogicalVolumeAtVertex()) std::cout << " in " << tr->GetLogicalVolumeAtVertex()->GetName() << ", ";
        else std::cout << " nowhere, ";
        std::cout << tr->GetTrackLength() << " mm ago" << std::endl;

        if (m_config.verb==1) m_config.verb=2;

      }
    }
  }

  void VerboseSelector::PreUserTrackingAction(const G4Track* aTrack)
  {
    if(m_evtCount==(uint64_t)m_config.targetEvent||m_config.targetEvent<0)
      {
        int trackID = aTrack->GetTrackID();
        // FIXME: can we remove this const_cast?
        G4Track *itr = const_cast<G4Track*>(aTrack);
        TrackHelper trackHelper(itr);

        AtlasG4EventUserInfo* atlasG4EvtUserInfo = static_cast<AtlasG4EventUserInfo*>
          (G4EventManager::GetEventManager()->GetConstCurrentEvent()->
           GetUserInformation());

        int currentBarcode(0);

        if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary()) {
          // FIXME: re-evaluate this comment below.
          // ADS this code crashes in MT runs, since atlasG4EvtUserInfo->GetCurrentlyTraced()
          // is NULL untill we migrate the truth
          currentBarcode = HepMC::barcode(atlasG4EvtUserInfo->GetCurrentlyTraced());
        }

        bool p1 = m_config.targetTrack<0 && m_config.targetBarcode<0;
        bool p2 = trackID==m_config.targetTrack;
        bool p3 = currentBarcode==m_config.targetBarcode;
        bool p4 = false;

        for (auto& pdgID : m_config.targetPdgIDs) {
          if (std::abs(aTrack->GetParticleDefinition()->GetPDGEncoding()) == pdgID ) {
            p4 = true;
            break;   
          }
        }

        if(p1 || p2 || p3 || p4) {
          ATH_MSG_INFO(std::endl << "---------> Dumping now track #"
                       << trackID << " barcode " << currentBarcode
                       << " pdgID " << aTrack->GetParticleDefinition()->GetPDGEncoding()
                       << " in event " << m_evtCount);
          G4EventManager::GetEventManager()->GetTrackingManager()->
            SetVerboseLevel(m_config.verboseLevel);
        }
      }
  }

  void VerboseSelector::PostUserTrackingAction(const G4Track* aTrack)
  {
    if(m_evtCount==(uint64_t)m_config.targetEvent||m_config.targetEvent<0){
      if(aTrack->GetTrackID()==m_config.targetTrack||m_config.targetTrack<0)
        G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(0);
      for (auto& pdgID : m_config.targetPdgIDs) {
        if (std::abs(aTrack->GetParticleDefinition()->GetPDGEncoding()) == pdgID ) {
          G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(0);
          break;
        }
      }
    }
  }

} // namespace G4UA
