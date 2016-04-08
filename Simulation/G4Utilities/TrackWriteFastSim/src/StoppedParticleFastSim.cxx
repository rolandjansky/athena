/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackWriteFastSim/StoppedParticleFastSim.h"
#include "TrackWriteFastSim/TrackFastSimSD.h"

#include "G4FastTrack.hh"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4Material.hh"
#include "G4ElementVector.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

StoppedParticleFastSim::StoppedParticleFastSim(const std::string& name, const std::string& fsSDname)
  : G4VFastSimulationModel(name)
  , m_fsSD(nullptr)
  , m_init(false)
  , m_fsSDname(fsSDname)
{
}

G4bool StoppedParticleFastSim::IsApplicable(const G4ParticleDefinition&)
{
  return true;
}

G4bool StoppedParticleFastSim::ModelTrigger(const G4FastTrack& fastTrack)
{
  // Trigger if the energy is below our threshold or if the time is over 150 ns
  int id = fastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
  if (id<1000000 || id>1100000) return true;
  else if (isSUSYParticle(id)){
    G4Material * mat = fastTrack.GetPrimaryTrack()->GetMaterial();
    double minA=1500000.;
    for (unsigned int i=0;i<mat->GetNumberOfElements();++i){
      if (mat->GetElement(i) &&
          minA>mat->GetElement(i)->GetN()){
        minA=mat->GetElement(i)->GetN();
      }
    }
    if (fastTrack.GetPrimaryTrack()->GetVelocity()<0.15*std::pow(minA,-2./3.)*CLHEP::c_light) return true;
    return false;
  }
  return true;
}

void StoppedParticleFastSim::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{
  if (!m_init){
    m_init = true;

    G4SDManager *sdm = G4SDManager::GetSDMpointer();
    G4VSensitiveDetector * vsd = sdm->FindSensitiveDetector( m_fsSDname );
    if (!vsd) {
      G4cout << "StoppedParticleFastSim::DoIt WARNING Could not get TrackFastSimSD sensitive detector.  If you are not writing track records this is expected." << G4endl;
      m_fsSD = dynamic_cast<TrackFastSimSD*>(vsd);
      if (!m_fsSD) {
        G4cout << "StoppedParticleFastSim::DoIt WARNING Could not cast the SD.  If you are not writing track records this is expected." << G4endl;
      }
    } // found the SD
  } // End of lazy init

  if (isSUSYParticle(fastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding()) &&
      m_fsSD) {
    m_fsSD->WriteTrack( fastTrack.GetPrimaryTrack() , false , true );
  }
  fastStep.KillPrimaryTrack();
}

bool StoppedParticleFastSim::isSUSYParticle(const int id) const
{
  if (id==1000021 || id==1000005 || id==1000006 || id==1000512 || id==1000522 || id==1000991 || id==1000993 ||
      id==1000612 || id==1000622 || id==1000632 || id==1000642 || id==1000652 || id==1005211 ||
      id==1006113 || id==1006211 || id==1006213 || id==1006223 || id==1006311 ||
      id==1006313 || id==1006321 || id==1006323 || id==1006333 ||
      id==1009111 || id==1009113 || id==1009211 || id==1009213 || id==1009311 ||
      id==1009313 || id==1009321 || id==1009323 || id==1009223 || id==1009333 ||
      id==1092112 || id==1091114 || id==1092114 || id==1092212 || id==1092214 || id==1092224 ||
      id==1093114 || id==1093122 || id==1093214 || id==1093224 || id==1093314 || id==1093324 || id==1093334)
    return true;
  return false;
}
