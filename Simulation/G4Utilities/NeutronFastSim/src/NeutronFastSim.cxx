/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutronFastSim/NeutronFastSim.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

#include "G4Neutron.hh"
#include "NeutronFastSim/NeutronFastSimSD.h"
#include <iostream>

static FastSimModelProxy<NeutronFastSim> cavfastsim("NeutronFastSim");

G4bool NeutronFastSim::IsApplicable(const G4ParticleDefinition& particleType)
{
  return ( &particleType == G4Neutron::NeutronDefinition() );
}

G4bool NeutronFastSim::ModelTrigger(const G4FastTrack& fastTrack)
{
  // Trigger if the energy is below our threshold or if the time is over 150 ns
  return (m_Energy<0?true:fastTrack.GetPrimaryTrack()->GetKineticEnergy()<m_Energy) || fastTrack.GetPrimaryTrack()->GetGlobalTime()>150;
}

void NeutronFastSim::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{
  if (!m_init){
    m_init = true;

    FADS::SensitiveDetectorCatalog * fsdc = FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
    if (!fsdc) { 
      std::cout << "NeutronFastSim could not get sensitive detector catalog.  If you are not writing track records this is acceptable." << std::endl;
    } else {
      FADS::FadsSensitiveDetector * fsd = fsdc->GetSensitiveDetector("NeutronFastSimSD");
      if (!fsd) { 
        std::cout << "NeutronFastSim could not get NeutronFastSimSD sensitive detector.  If you are not writing track records this is expected." << std::endl; 
      } else {
        m_neutronSD = dynamic_cast<NeutronFastSimSD*>(fsd);
        if (!m_neutronSD) {
          std::cout << "NeutronFastSim could not cast the SD.  If you are not writing track records this is expected." << std::endl; 
        } // succeeded in cast
      } // found the SD
    } // got the catalog
  }

  if (m_neutronSD) m_neutronSD->WriteNeutron( fastTrack.GetPrimaryTrack() );
  fastStep.KillPrimaryTrack();
}

