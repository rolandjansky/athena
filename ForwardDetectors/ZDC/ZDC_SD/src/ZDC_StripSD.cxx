/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "ZDC_StripSD.h"

// Geant4 headers
#include "G4Poisson.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// STL headers
#include <cmath>

ZDC_StripSD::ZDC_StripSD(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name ), m_HitColl(hitCollectionName)
{
}

void ZDC_StripSD::StartOfAthenaEvent()
{
  if(verboseLevel>5)
    {
      G4cout << "ZDC_StripSD::StartOfAthenaEvent()" << G4endl;
    }
  for (int I=0; I<2; I++)
    {
      for (int J=0; J<4; J++)
        {
          m_Edep_Cherenkov_Strip   [I][J] = 0;
          m_NPhoton_Cherenkov_Strip[I][J] = 0;
        }
    }
}

void ZDC_StripSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = std::make_unique<ZDC_SimStripHit_Collection>();
}

G4bool ZDC_StripSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  const G4DynamicParticle* aParticle = aStep->GetTrack()->GetDynamicParticle();
  const G4double           charge    = aParticle->GetDefinition()->GetPDGCharge();

  if (charge==0) { return true; }

  const G4StepPoint* pPreStepPoint  = aStep->GetPreStepPoint();
  const G4StepPoint* pPostStepPoint = aStep->GetPostStepPoint();
  const G4double beta         = (pPreStepPoint ->GetBeta() + pPostStepPoint->GetBeta())/2.;

  if (beta==0) { return true; }

  G4double MeanNumberOfPhotons = 291.93*(charge/CLHEP::eplus)*(charge/CLHEP::eplus)*(1.0 - 1.0/(beta*1.48*beta*1.48))/(CLHEP::cm);

  if (MeanNumberOfPhotons <= 0.0) { return true; }

  const float step_length(aStep->GetStepLength());

  MeanNumberOfPhotons = MeanNumberOfPhotons * step_length;

  const G4int NumPhotons = (G4int)G4Poisson(MeanNumberOfPhotons);

  if (NumPhotons <= 0) { return true; }

  //THIS PART LOCATES THE MODULE WHERE THE HIT OCCURED

  int Zloc(-1);
  int Side(-1);

  G4int Location_From_Copy_No = pPreStepPoint->GetPhysicalVolume()->GetCopyNo();

  Zloc = (Location_From_Copy_No/1000) %10 -1; // can be 0,1,2,3
  Side = (Location_From_Copy_No/10000)%10 -1; //Side==0 is A-side; and Side ==1 is C-Side

  if (Side<0 || Zloc <0)
    {

      G4cout << "ERROR: LOCATING MODULE " << Side << "   " << Zloc << G4endl;
      G4ExceptionDescription description;
      description << "ProcessHits: COULD NOT LOCATE MODULE(From Copy)";
      G4Exception("ZDC_StripSD", "FailedToLocateModule", FatalException, description);
      return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

  if (Side>1 || Zloc>3)
    {
      G4cout << "ERROR: Impossible values obtained. Values are (Side,Zloc) = "
             << Side << "   "
             << Zloc << "   " << G4endl;
      G4ExceptionDescription description;
      description << "ProcessHits: IMPOSSIBLE MODULE LOCATION (From Copy)";
      G4Exception("ZDC_StripSD", "ImpossibleModuleLocation", FatalException, description);
      return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }
  const G4ThreeVector p0 = aStep->GetDeltaPosition().unit();
  const float Pmin   = 2.761*CLHEP::eV; // MinPhotonEnergy :: Cherenkov Photons below this energy are not produced
  //float Pmax = 3.550*CLHEP::eV; // MaxPhotonEnergy :: Cherenkov Photons above this energy are not produced
  const float dp     = 0.789*CLHEP::eV; // Pmax - Pmin;
  const float BetaInverse = 1./beta;

  for (G4int I = 0; I < NumPhotons; I++) {

    G4double rand;
    float sampledEnergy, sampledRI;
    float cosTheta, sin2Theta;

    // sample an energy for Photon
    rand = G4UniformRand();
    sampledEnergy = Pmin + rand * dp;
    sampledRI = 1.48;
    cosTheta  = BetaInverse / sampledRI;
    sin2Theta = (1.0 - cosTheta)*(1.0 + cosTheta);

    // Generate random position of photon on cone surface defined by Theta
    rand = G4UniformRand();
    const float phi = 2.*M_PI*rand;
    const float sinPhi = std::sin(phi);
    const float cosPhi = std::cos(phi);

    // calculate x,y, and z components of photon momentum
    // (in coord system with primary particle direction aligned with the z axis)
    // and Rotate momentum direction back to global reference system
    const float sinTheta = std::sqrt(sin2Theta);
    const float px = sinTheta*cosPhi;
    const float py = sinTheta*sinPhi;
    const float pz = cosTheta;

    G4ParticleMomentum photonMomentum(px, py, pz);
    photonMomentum.rotateUz(p0);

#define TRANSMISSION_PER_INTERNAL_REFLECTION 1.00  //Probability for photon to survive each time it undergoes a total internal reflection
#define LENGTH_OF_STRIP 33*CLHEP::cm
#define DIAMETER_OF_STRIP 1.5*CLHEP::mm
#define THETA_CRITICAL_QUARTZ 0.742

    float Transmission=1.0;
    const float PT = sqrt(photonMomentum.getX()*photonMomentum.getX() + photonMomentum.getZ()*photonMomentum.getZ());
    const float PY = photonMomentum.getY();

    if      (PY<=0) continue;         // If photon is travelling with -ve PY, Its not going to reach the PMT
    else if (PT==0) Transmission=1.0; // if photon is along Y-axis it will be completely transmitted into the PMT
    else {

      const float Theta = M_PI/2.0-std::atan(PT/PY);

      if (Theta < THETA_CRITICAL_QUARTZ) continue;
      else Transmission=1.0;
    }

    m_Edep_Cherenkov_Strip   [Side][Zloc] +=sampledEnergy*Transmission;
    m_NPhoton_Cherenkov_Strip[Side][Zloc] +=1;
  }

  return true;
}

void ZDC_StripSD::EndOfAthenaEvent()
{
  if(verboseLevel>5)
    {
      G4cout << "ZDC_StripSD::EndOfAthenaEvent(): Printing Final Energy(eV) deposited in Strip " << G4endl;
    }

  for (int I=0; I<2; I++)
    {
      for (int J=0; J<4; J++)
        {
          const double Edep_C  =    m_Edep_Cherenkov_Strip[I][J]/CLHEP::eV;
          const int    NPhoton = m_NPhoton_Cherenkov_Strip[I][J];
          if(verboseLevel>5)
            {
              G4cout << "ZDC_StripSD::EndOfAthenaEvent(): Final Energy(eV) deposited in Strip "
                     << I << "  "
                     << J << "  = "
                     << Edep_C << " eV and Number of Photons deposited = " << NPhoton << G4endl;
            }
          m_HitColl->Emplace(I, J, NPhoton, Edep_C);
        }
    }
  return;
}
