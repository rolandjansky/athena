/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackWriteFastSim/NeutronFastSim.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "MCTruth/TruthStrategyManager.h"
#include "MCTruth/EventInformation.h"

#include "G4Neutron.hh"
#include "TrackWriteFastSim/TrackFastSimSD.h"
#include <iostream>

static FastSimModelProxy<NeutronFastSim> cavfastsim("NeutronFastSim");

G4bool NeutronFastSim::IsApplicable(const G4ParticleDefinition&)
{
  if (!m_init){
    m_init = true;

    FADS::SensitiveDetectorCatalog * fsdc = FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
    if (!fsdc) {
      std::cout << "NeutronFastSim could not get sensitive detector catalog.  If you are not writing track records this is acceptable." << std::endl;
    } else {
      FADS::FadsSensitiveDetector * fsd = fsdc->GetSensitiveDetector("TrackFastSimSD");
      if (!fsd) {
        std::cout << "NeutronFastSim could not get TrackFastSimSD sensitive detector.  If you are not writing track records this is expected." << std::endl;
      } else {
        m_fsSD = dynamic_cast<TrackFastSimSD*>(fsd);
        if (!m_fsSD) {
          std::cout << "NeutronFastSim could not cast the SD.  If you are not writing track records this is expected." << std::endl;
        } else { // succeeded in cast
          m_fsSD->SetCollectionName("NeutronBG");
        }
      } // found the SD
    } // got the catalog
  }
  return true;
}

G4bool NeutronFastSim::ModelTrigger(const G4FastTrack& fastTrack)
{
  // Trigger if the neutron energy is below our threshold or if the time is over 150 ns
  if (fastTrack.GetPrimaryTrack()->GetDefinition() == G4Neutron::NeutronDefinition() ){
    return (m_Energy<0?true:fastTrack.GetPrimaryTrack()->GetKineticEnergy()<m_Energy) || fastTrack.GetPrimaryTrack()->GetGlobalTime()>150;
  }

  // Not a neutron... Pick it up if the primary had eta>6.0
  EventInformation *eventInfo=TruthStrategyManager::GetStrategyManager()->GetEventInformation();
  HepMC::GenParticle *gp = eventInfo->GetCurrentPrimary();
  if (fabs(gp->momentum().eta())>6.0 && gp->barcode()<200000){
    //std::cout << "Recording particle " << fastTrack.GetPrimaryTrack()->GetDefinition()->GetParticleName() << " " << fastTrack.GetPrimaryTrack()->GetKineticEnergy() << std::endl;
    return true;
  } else {
    return false;
  }
}

void NeutronFastSim::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{
  if (m_fsSD) m_fsSD->WriteTrack( fastTrack.GetPrimaryTrack() , false , false );
  fastStep.KillPrimaryTrack();
}
