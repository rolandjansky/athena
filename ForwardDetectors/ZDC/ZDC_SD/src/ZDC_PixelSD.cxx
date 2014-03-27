/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>
#include "ZDC_SD/ZDC_PixelSD.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h" 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4Poisson.hh"

static FADS::SensitiveDetectorEntryT<ZDC_PixelSD> zdc_pix_sd("ZDCPixelSD");

void ZDC_PixelSD::Initialize(G4HCofThisEvent*)
{
  ATH_MSG_DEBUG("IN INITIALIZE OF PIXELSD");

  for (int I=0; I<=1; I++) for (int J=0; J<=1; J++) for (int K=0; K<80; K++) {
    
    Edep_Cherenkov_Pixel   [I][J][K] = 0;
    NPhoton_Cherenkov_Pixel[I][J][K] = 0;
  }
}

G4bool ZDC_PixelSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  G4StepPoint*             pPreStepPoint  = aStep->GetPreStepPoint();
  G4StepPoint*             pPostStepPoint = aStep->GetPostStepPoint();
  const G4DynamicParticle* aParticle      = aStep->GetTrack()->GetDynamicParticle();
  const G4double           charge         = aParticle->GetDefinition()->GetPDGCharge();                
  const G4double           beta           = (pPreStepPoint ->GetBeta() + pPostStepPoint->GetBeta())/2.;

  G4double MeanNumberOfPhotons = 291.93*(charge/CLHEP::eplus)*(charge/CLHEP::eplus)* (1.0 - 1.0/(beta*1.48*beta*1.48))/(CLHEP::cm);

  if (MeanNumberOfPhotons <= 0.0) return 1;
  
  G4double step_length = aStep->GetStepLength();

  MeanNumberOfPhotons = MeanNumberOfPhotons * step_length;
  
  G4int NumPhotons = (G4int)G4Poisson(MeanNumberOfPhotons);
  
  if (NumPhotons <= 0) return 1;

  //THIS PART LOCATES THE SIDE, MODULE AND PIXEL WHERE THE HIT OCCURED

  int Pixel_No = -1;
  int Module   = -1;
  int Side     = -1;

  G4int Location_From_Copy_No = pPreStepPoint->GetPhysicalVolume()->GetCopyNo();
  
  Pixel_No =    (Location_From_Copy_No%100);
  Module   = int(Location_From_Copy_No/999) %10 - 1;
  Side     = int(Location_From_Copy_No/9999)%10 - 1;
  
  if (Pixel_No<0 || Module<0 || Side<0) {
    
    ATH_MSG_ERROR("COULDNOT LOCATE PIXEL(From Copy) Values are (Side,Module,Pixel_No) = "
		  << Side     << "   "
		  << Module   << "   "
		  << Pixel_No << "   ");
    return 0;
  }
  
  //THIS PART CALCULATES THE CHERENKOV ENERGY DEPOSITED IN PIXELS (AFTER CHECKING THE CUTS)
  
  G4ThreeVector p0 = aStep->GetDeltaPosition().unit();
  G4double Pmin   = 2.761*CLHEP::eV;  // Rindex->GetMinPhotonEnergy();
  //G4double Pmax = 3.550*CLHEP::eV;  // Rindex->GetMaxPhotonEnergy();
  G4double dp     = 0.789*CLHEP::eV;  // Pmax - Pmin;
  G4double BetaInverse = 1./beta;
  
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
    G4double phi = 2.*M_PI*rand;
    G4double sinPhi = sin(phi);
    G4double cosPhi = cos(phi);
    
    // calculate x,y, and z components of photon momentum 
    // (in coord system with primary particle direction aligned with the z axis) 
    // and Rotate momentum direction back to global reference system
    G4double sinTheta = sqrt(sin2Theta);
    G4double px = sinTheta*cosPhi;
    G4double py = sinTheta*sinPhi;
    G4double pz = cosTheta;
    G4ParticleMomentum photonMomentum(px, py, pz);
    photonMomentum.rotateUz(p0);

#define TRANSMISSION_PER_INTERNAL_REFLECTION 1.00  //Probability for photon to survive each time it undergoes a total internal reflection
#define LENGTH_OF_PIXEL 33*cm
#define DIAMETER_OF_PIXEL 1.0*mm
#define THETA_CRITICAL_QUARTZ 0.742

    G4double Transmission = 1.0;
    G4double PT = sqrt(photonMomentum.getX()*photonMomentum.getX() + photonMomentum.getY()*photonMomentum.getY());
    G4double PZ = photonMomentum.getZ();

    if      (PZ<0)  continue;         // If photon is travelling with -ve PZ, Its not going to reach the PMT
    else if (PT==0) Transmission=1.0; // if photon is along Z-axis it will be completely transmitted into the PMT
    else {

      G4double Theta = M_PI/2.0-atan(PT/PZ);  
      
      if (Theta < THETA_CRITICAL_QUARTZ) continue;
      else Transmission=1.0;
    }
    
    Edep_Cherenkov_Pixel   [Side][Module][Pixel_No] += sampledEnergy*Transmission;
    NPhoton_Cherenkov_Pixel[Side][Module][Pixel_No] += 1;
  }
  
  return 1;
}  

void ZDC_PixelSD::EndOfEvent(G4HCofThisEvent*)
{
  ATH_MSG_DEBUG("Printing Final Energy(eV) deposited in Pixel");
  
  ZDC_SimPixelHit_Collection* m_ZDC_SimPixelHit_Collection = new ZDC_SimPixelHit_Collection("ZDC_SimPixelHit_Collection");
  
  for (int I=0; I<=1; I++) {

    int Jmin = 0; if (I==1) Jmin = 1; 

    for (int J=Jmin; J<=1; J++) {
      
      int Kmax = 24; // CHANGE TO 80 IF YOU WANT EACH PIXEL IN HMXY TP BE READ OUT SEPARATELY (Also make corresponding change in ZDC_GeoM file)
      
      if (J==0) Kmax = 64;
      
      for (int K=0; K<Kmax; K++) {
	
	int    NPhoton = NPhoton_Cherenkov_Pixel[I][J][K];
	double Edep_C  = Edep_Cherenkov_Pixel   [I][J][K]/CLHEP::eV;
	
	ATH_MSG_DEBUG(" Final Cherenkov Energy(eV) deposited in Pixel "
		      << I << "   "
		      << J << "   "
		      << K << "  = "
		      << Edep_C << " eV and Number of Photons deposited = " << NPhoton);
	
	ZDC_SimPixelHit* ahit = new ZDC_SimPixelHit(I, J, K, NPhoton, Edep_C);
	
	m_ZDC_SimPixelHit_Collection->Insert(*ahit);

	delete ahit;
      }
    }	    
  }
  
  //for (ZDC_SimPixelHit_ConstIterator it = m_ZDC_SimPixelHit_Collection->begin(); it != m_ZDC_SimPixelHit_Collection->end(); it++)
  // ATH_MSG_DEBUG(" BUG_TRACKER "
  // 		<< " Side: "     << it->GetSide()
  // 		<< " ModuleNo: " << it->GetMod()
  // 		<< " PixNo: "    << it->GetPix()
  // 		<< " NPhotons: " << it->GetNPhotons()
  // 		<< " Edep: "     << it->GetEdep());
  
  m_HitCollHelp.ExportCollection<ZDC_SimPixelHit_Collection>(m_ZDC_SimPixelHit_Collection);
}
