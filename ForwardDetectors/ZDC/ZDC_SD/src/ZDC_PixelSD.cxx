/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "ZDC_PixelSD.h"

// Athena headers
#include "CxxUtils/make_unique.h" // For make unique

// Geant4 headers
#include "G4Poisson.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// STL headers
#include <cmath>

ZDC_PixelSD::ZDC_PixelSD(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

void ZDC_PixelSD::StartOfAthenaEvent()
{
  if (verboseLevel>5)
    {
      G4cout << "IN INITIALIZE OF PIXELSD" << G4endl;
    }

  for (int I=0; I<=1; I++)
    {
      for (int J=0; J<=1; J++)
        {
          for (int K=0; K<80; K++)
            {
              m_Edep_Cherenkov_Pixel   [I][J][K] = 0;
              m_NPhoton_Cherenkov_Pixel[I][J][K] = 0;
            }
        }
    }
}

void ZDC_PixelSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<ZDC_SimPixelHit_Collection>();
}

G4bool ZDC_PixelSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  const G4StepPoint*             pPreStepPoint  = aStep->GetPreStepPoint();
  const G4StepPoint*             pPostStepPoint = aStep->GetPostStepPoint();
  const G4DynamicParticle* aParticle      = aStep->GetTrack()->GetDynamicParticle();
  const G4double           charge         = aParticle->GetDefinition()->GetPDGCharge();
  const G4double           beta           = (pPreStepPoint ->GetBeta() + pPostStepPoint->GetBeta())/2.;

  G4double MeanNumberOfPhotons = 291.93*(charge/CLHEP::eplus)*(charge/CLHEP::eplus)* (1.0 - 1.0/(beta*1.48*beta*1.48))/(CLHEP::cm);

  if (MeanNumberOfPhotons <= 0.0) { return true; }

  const G4double step_length = aStep->GetStepLength();

  MeanNumberOfPhotons = MeanNumberOfPhotons * step_length;

  const G4int NumPhotons = (G4int)G4Poisson(MeanNumberOfPhotons);

  if (NumPhotons <= 0) { return true; }

  //THIS PART LOCATES THE SIDE, MODULE AND PIXEL WHERE THE HIT OCCURED

  int Pixel_No(-1);
  int Module(-1);
  int Side(-1);

  const G4int Location_From_Copy_No = pPreStepPoint->GetPhysicalVolume()->GetCopyNo();

  Pixel_No =    (Location_From_Copy_No%100);
  Module   = int(Location_From_Copy_No/999) %10 - 1;
  Side     = int(Location_From_Copy_No/9999)%10 - 1;

  if (Pixel_No<0 || Module<0 || Side<0) {

    G4cout << "ERROR: COULD NOT LOCATE PIXEL(From Copy) Values are (Side,Module,Pixel_No) = "
           << Side     << "   "
           << Module   << "   "
           << Pixel_No << "   " << G4endl;
    G4ExceptionDescription description;
    description << "ProcessHits: COULD NOT LOCATE PIXEL(From Copy)";
    G4Exception("ZDC_PixelSD", "FailedToLocatePixel", FatalException, description);
    return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  if (Pixel_No>79 || Module>1 || Side>1) {
    G4cout << "ERROR: Impossible values obtained. Values are (Side,Module,Pixel_No) = "
           << Side     << "   "
           << Module   << "   "
           << Pixel_No << "   " << G4endl;
    G4ExceptionDescription description;
    description << "ProcessHits: IMPOSSIBLE PIXEL LOCATION (From Copy)";
    G4Exception("ZDC_PixelSD", "ImpossiblePixelLocation", FatalException, description);
    return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  //THIS PART CALCULATES THE CHERENKOV ENERGY DEPOSITED IN PIXELS (AFTER CHECKING THE CUTS)

  const G4ThreeVector p0 = aStep->GetDeltaPosition().unit();
  const G4double Pmin   = 2.761*CLHEP::eV;  // Rindex->GetMinPhotonEnergy();
  //G4double Pmax = 3.550*CLHEP::eV;  // Rindex->GetMaxPhotonEnergy();
  const G4double dp     = 0.789*CLHEP::eV;  // Pmax - Pmin;
  const G4double BetaInverse = 1./beta;

  for (G4int I = 0; I < NumPhotons; I++) {

    G4double rand;
    G4double sampledEnergy,sampledRI;
    G4double cosTheta, sin2Theta;

    // sample an energy for Photon
    rand = G4UniformRand();
    sampledEnergy = Pmin + rand * dp;
    sampledRI = 1.48;
    cosTheta  = BetaInverse / sampledRI;
    sin2Theta = (1.0 - cosTheta)*(1.0 + cosTheta);

    // Generate random position of photon on cone surface defined by Theta
    rand = G4UniformRand();
    const G4double phi = 2.*M_PI*rand;
    const G4double sinPhi = sin(phi);
    const G4double cosPhi = cos(phi);

    // calculate x,y, and z components of photon momentum
    // (in coord system with primary particle direction aligned with the z axis)
    // and Rotate momentum direction back to global reference system
    const G4double sinTheta = sqrt(sin2Theta);
    const G4double px = sinTheta*cosPhi;
    const G4double py = sinTheta*sinPhi;
    const G4double pz = cosTheta;
    G4ParticleMomentum photonMomentum(px, py, pz);
    photonMomentum.rotateUz(p0);

#define TRANSMISSION_PER_INTERNAL_REFLECTION 1.00  //Probability for photon to survive each time it undergoes a total internal reflection
#define LENGTH_OF_PIXEL 33*cm
#define DIAMETER_OF_PIXEL 1.0*mm
#define THETA_CRITICAL_QUARTZ 0.742

    G4double Transmission = 1.0;
    const G4double PT = sqrt(photonMomentum.getX()*photonMomentum.getX() + photonMomentum.getY()*photonMomentum.getY());
    const G4double PZ = photonMomentum.getZ();

    if      (PZ<0)  continue;         // If photon is travelling with -ve PZ, Its not going to reach the PMT
    else if (PT==0) Transmission=1.0; // if photon is along Z-axis it will be completely transmitted into the PMT
    else {

      const G4double Theta = M_PI/2.0-atan(PT/PZ);

      if (Theta < THETA_CRITICAL_QUARTZ) continue;
      else Transmission=1.0;
    }

    m_Edep_Cherenkov_Pixel   [Side][Module][Pixel_No] += sampledEnergy*Transmission;
    m_NPhoton_Cherenkov_Pixel[Side][Module][Pixel_No] += 1;
  }

  return true;
}

void ZDC_PixelSD::EndOfAthenaEvent()
{
  if (verboseLevel>5)
    {
      G4cout << "Printing Final Energy(eV) deposited in Pixel" << G4endl;
    }

  for (int I=0; I<=1; I++)
    {
      int Jmin = 0;
      if (I==1) { Jmin = 1; }
      for (int J=Jmin; J<=1; J++)
        {
          int Kmax = 24; // CHANGE TO 80 IF YOU WANT EACH PIXEL IN HMXY TP BE READ OUT SEPARATELY (Also make corresponding change in ZDC_GeoM file)
          if (J==0) { Kmax = 64; }
          for (int K=0; K<Kmax; K++)
            {
              const int    NPhoton = m_NPhoton_Cherenkov_Pixel[I][J][K];
              const double Edep_C  = m_Edep_Cherenkov_Pixel   [I][J][K]/CLHEP::eV;
              if (verboseLevel>5)
                {
                  G4cout << " Final Cherenkov Energy(eV) deposited in Pixel "
                         << I << "   "
                         << J << "   "
                         << K << "  = "
                         << Edep_C << " eV and Number of Photons deposited = " << NPhoton << G4endl;
                }
              m_HitColl->Emplace(I, J, K, NPhoton, Edep_C);
            }
        }
    }
  //for (ZDC_SimPixelHit_ConstIterator it = m_ZDC_SimPixelHit_Collection->begin(); it != m_ZDC_SimPixelHit_Collection->end(); it++)
  // ATH_MSG_DEBUG(" BUG_TRACKER "
  //            << " Side: "     << it->GetSide()
  //            << " ModuleNo: " << it->GetMod()
  //            << " PixNo: "    << it->GetPix()
  //            << " NPhotons: " << it->GetNPhotons()
  //            << " Edep: "     << it->GetEdep());
  return;
}
