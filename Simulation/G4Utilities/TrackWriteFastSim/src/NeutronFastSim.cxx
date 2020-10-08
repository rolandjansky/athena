/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackWriteFastSim/NeutronFastSim.h"
#include "MCTruth/AtlasG4EventUserInfo.h"

#include "G4Event.hh"
#include "G4Neutron.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4EventManager.hh"
#include "TrackWriteFastSim/TrackFastSimSD.h"

NeutronFastSim::NeutronFastSim(const std::string& name, const std::string& fsSDname, const double etaCut, const double timeCut)
  : G4VFastSimulationModel(name)
  , m_Energy(5)
  , m_fsSD(0)
  , m_init(false)
  , m_fsSDname(fsSDname)
  , m_etaCut(etaCut)
  , m_timeCut(timeCut)
{
}

G4bool NeutronFastSim::IsApplicable(const G4ParticleDefinition&)
{
  if (!m_init){
    m_init = true;

    G4SDManager *sdm = G4SDManager::GetSDMpointer();
    G4VSensitiveDetector * vsd = sdm->FindSensitiveDetector( m_fsSDname );
    if (!vsd) {
      G4cout << "NeutronFastSim::IsApplicable WARNING Could not get TrackFastSimSD sensitive detector.  If you are not writing track records this is expected." << G4endl;
    } else {
      m_fsSD = dynamic_cast<TrackFastSimSD*>(vsd);
      if (!m_fsSD) {
        G4cout << "NeutronFastSim::IsApplicable WARNING Could not cast the SD.  If you are not writing track records this is expected." << G4endl;
      }
    } // found the SD
  } // End of lazy init
  return true;
}

G4bool NeutronFastSim::ModelTrigger(const G4FastTrack& fastTrack)
{
  // Trigger if the neutron energy is below our threshold or if the time is over 150 ns
  if (fastTrack.GetPrimaryTrack()->GetDefinition() == G4Neutron::NeutronDefinition() ){
    return (m_Energy<0?true:fastTrack.GetPrimaryTrack()->GetKineticEnergy()<m_Energy) || fastTrack.GetPrimaryTrack()->GetGlobalTime()>m_timeCut;
  }

  // Not a neutron... Pick it up if the primary had eta>6.0
  AtlasG4EventUserInfo *atlasG4EvtUserInfo=static_cast<AtlasG4EventUserInfo*>(G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
  HepMC::ConstGenParticlePtr gp = atlasG4EvtUserInfo->GetCurrentPrimary();
  if (std::abs(gp->momentum().eta())>m_etaCut && HepMC::barcode(gp)<200000){
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
