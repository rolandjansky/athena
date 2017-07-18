/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BCM Sensitive Detector class
//   Bostjan Macek 15.may.2007
//###############################################

// Class header
#include "BCMSensorSD.h"

// Package headers
#include "BCMExtra.h"

// Athena headers
#include "CxxUtils/make_unique.h" // For make unique
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

BCMSensorSD::BCMSensorSD(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Initialize from G4 - necessary to new the write handle for now
void BCMSensorSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<SiHitCollection>();
}

G4bool BCMSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  edep *= CLHEP::MeV;

  //if there is no energy deposition skip everything
  if(edep==0.)
    {
      if(aStep->GetTrack()->GetDefinition()!=G4Geantino::GeantinoDefinition() && aStep->GetTrack()->GetDefinition()!=G4ChargedGeantino::ChargedGeantinoDefinition()) return false;
    }

  //Get the Touchable History:
  G4TouchableHistory*  myTouch = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  int BEcopyNo =  myTouch->GetVolume()->GetCopyNo();

  // Get the hit coordinates. Start and End Point
  G4ThreeVector coord1 = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector coord2 = aStep->GetPostStepPoint()->GetPosition();

  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector localPosition1 = transformation.TransformPoint(coord1);
  G4ThreeVector localPosition2 = transformation.TransformPoint(coord2);

  HepGeom::Point3D<double> lP1,lP2;
  lP1[SiHit::xEta] = localPosition1[2]*CLHEP::mm;
  lP1[SiHit::xPhi] = localPosition1[1]*CLHEP::mm;
  lP1[SiHit::xDep] = localPosition1[0]*CLHEP::mm;

  lP2[SiHit::xEta] = localPosition2[2]*CLHEP::mm;
  lP2[SiHit::xPhi] = localPosition2[1]*CLHEP::mm;
  lP2[SiHit::xDep] = localPosition2[0]*CLHEP::mm;

  //BCM hit stuff
  if(BEcopyNo == 11950 || BEcopyNo == 11951)
    {
      TrackHelper trHelp(aStep->GetTrack());
      //primary or not
      int primaren = 0;
      if(trHelp.IsPrimary())
        primaren = 1;
      else if(trHelp.IsRegeneratedPrimary())
        primaren = 2;
      else if(trHelp.IsSecondary())
        primaren = 3;
      else if(trHelp.IsRegisteredSecondary())
        primaren = 4;
      //std::cout << "BCMBarcode == " << trHelp.GetBarcode() << " Vertex: " << aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() << std::endl;

      int produced_in_diamond = 0;
      if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Pixel::bcmDiamondLog")
        produced_in_diamond = 1;
      else if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Pixel::bcmModLog")
        produced_in_diamond = 2;
      else if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Pixel::bcmWallLog")
        produced_in_diamond = 3;

      m_HitColl->Emplace(lP1, lP2, edep, aStep->GetPreStepPoint()->GetGlobalTime(), trHelp.GetParticleLink(),
                         0, 0, myTouch->GetVolume(1)->GetCopyNo()-951, BEcopyNo - 11950, primaren, produced_in_diamond);
    }
  return true;
}
