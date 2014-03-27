/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>
#include "ZDC_SD/ZDC_StripSD.h"
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h" 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4Poisson.hh"

static FADS::SensitiveDetectorEntryT<ZDC_StripSD> zdc_strip_sd("ZDCStripSD");

void ZDC_StripSD::Initialize(G4HCofThisEvent*)
{
  ATH_MSG_DEBUG("IN INITIALIZE OF STRIPSD");
  
  for (int I=0; I<2; I++) for (int J=0; J<4; J++) {
    
    Edep_Cherenkov_Strip   [I][J] = 0;
    NPhoton_Cherenkov_Strip[I][J] = 0;
  }
}

G4bool ZDC_StripSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  const G4DynamicParticle* aParticle = aStep->GetTrack()->GetDynamicParticle();
  const G4double           charge    = aParticle->GetDefinition()->GetPDGCharge();
  
  if (charge==0) return 1;

  G4StepPoint* pPreStepPoint  = aStep->GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep->GetPostStepPoint();
  const G4double beta         = (pPreStepPoint ->GetBeta() + pPostStepPoint->GetBeta())/2.;

  if (beta==0) return 1;

  G4double MeanNumberOfPhotons = 291.93*(charge/CLHEP::eplus)*(charge/CLHEP::eplus)*(1.0 - 1.0/(beta*1.48*beta*1.48))/(CLHEP::cm);
  
  if (MeanNumberOfPhotons <= 0.0) return 1;

  float step_length = aStep->GetStepLength();
  
  MeanNumberOfPhotons = MeanNumberOfPhotons * step_length;
  
  G4int NumPhotons = (G4int)G4Poisson(MeanNumberOfPhotons);
  
  if (NumPhotons <= 0) return 1;
  
  //THIS PART LOCATES THE MODULE WHERE THE HIT OCCURED

  int Zloc = -1;
  int Side = -1;

  G4int Location_From_Copy_No = pPreStepPoint->GetPhysicalVolume()->GetCopyNo();
  
  Zloc = (Location_From_Copy_No/1000) %10 -1; // can be 0,1,2,3
  Side = (Location_From_Copy_No/10000)%10 -1; //Side==0 is A-side; and Side ==1 is C-Side 

  if (Side<0 || Zloc <0) {

    ATH_MSG_DEBUG(" ERROR LOCATING MODULE " << Side << "   " << Zloc);
    return 0;
  }

  G4ThreeVector p0 = aStep->GetDeltaPosition().unit();
  float Pmin   = 2.761*CLHEP::eV; // MinPhotonEnergy :: Cherenkov Photons below this energy are not produced
  //float Pmax = 3.550*CLHEP::eV; // MaxPhotonEnergy :: Cherenkov Photons above this energy are not produced 
  float dp     = 0.789*CLHEP::eV; // Pmax - Pmin;
  float BetaInverse = 1./beta;

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
    float phi = 2.*M_PI*rand;
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);

    // calculate x,y, and z components of photon momentum 
    // (in coord system with primary particle direction aligned with the z axis) 
    // and Rotate momentum direction back to global reference system
    float sinTheta = sqrt(sin2Theta);
    float px = sinTheta*cosPhi;
    float py = sinTheta*sinPhi;
    float pz = cosTheta;
    
    G4ParticleMomentum photonMomentum(px, py, pz);
    photonMomentum.rotateUz(p0);

#define TRANSMISSION_PER_INTERNAL_REFLECTION 1.00  //Probability for photon to survive each time it undergoes a total internal reflection
#define LENGTH_OF_STRIP 33*CLHEP::cm
#define DIAMETER_OF_STRIP 1.5*CLHEP::mm
#define THETA_CRITICAL_QUARTZ 0.742

    float Transmission=1.0;
    float PT = sqrt(photonMomentum.getX()*photonMomentum.getX() + photonMomentum.getZ()*photonMomentum.getZ());
    float PY = photonMomentum.getY();

    if      (PY<=0) continue;         // If photon is travelling with -ve PY, Its not going to reach the PMT
    else if (PT==0) Transmission=1.0; // if photon is along Y-axis it will be completely transmitted into the PMT
    else {

      float Theta = M_PI/2.0-atan(PT/PY);
      
      if (Theta < THETA_CRITICAL_QUARTZ) continue;
      else Transmission=1.0;
    }

    Edep_Cherenkov_Strip   [Side][Zloc] +=sampledEnergy*Transmission;
    NPhoton_Cherenkov_Strip[Side][Zloc] +=1;
  }
  
  return 1;
}

void ZDC_StripSD::EndOfEvent(G4HCofThisEvent*)
{
  ATH_MSG_DEBUG(" Printing Final Energy(eV) deposited in Strip ");
  
  ZDC_SimStripHit_Collection* m_ZDC_SimStripHit_Collection = new ZDC_SimStripHit_Collection("ZDC_SimStripHit_Collection");
  
  for (int I=0; I<2; I++) for (int J=0; J<4; J++) {
    
    double Edep_C  =    Edep_Cherenkov_Strip[I][J]/CLHEP::eV;
    int    NPhoton = NPhoton_Cherenkov_Strip[I][J];
    
    ATH_MSG_DEBUG(" Final Energy(eV) deposited in Strip "
		  << I << "  "
		  << J << "  = "
		  << Edep_C << " eV and Number of Photons deposited = " << NPhoton);
    
    ZDC_SimStripHit* ahit = new ZDC_SimStripHit(I, J, NPhoton, Edep_C);
    
    m_ZDC_SimStripHit_Collection->Insert(*ahit);
  
    delete ahit;
  }
  
  // for (ZDC_SimStripHit_ConstIterator it = m_ZDC_SimStripHit_Collection->begin(); it != m_ZDC_SimStripHit_Collection->end(); it++)
  //   ATH_MSG_DEBUG(" BUG_TRACKER "
  // 		  << " Side: "     << it->GetSide()
  // 		  << " ModuleNo: " << it->GetMod()
  // 		  << " NPhotons: " << it->GetNPhotons());
		  
  m_HitCollHelp.ExportCollection<ZDC_SimStripHit_Collection>(m_ZDC_SimStripHit_Collection);
}
