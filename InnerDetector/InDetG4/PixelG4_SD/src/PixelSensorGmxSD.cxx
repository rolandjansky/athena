/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// Pixel Sensitive Detector - specialisation for GeoModelXml
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//

// Class header
#include "PixelSensorGmxSD.h"

// Athena headers
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// For make unique
#include <memory>


PixelSensorGmxSD::PixelSensorGmxSD(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

// Initialize from G4 - necessary to new the write handle for now
void PixelSensorGmxSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = std::make_unique<SiHitCollection>();
}


G4bool PixelSensorGmxSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  if (verboseLevel>5) G4cout << "Process Hit" << G4endl;

  G4double edep = aStep->GetTotalEnergyDeposit();
  edep *= CLHEP::MeV;
  if(edep==0.) {
    if(aStep->GetTrack()->GetDefinition() != G4Geantino::GeantinoDefinition() &&
       aStep->GetTrack()->GetDefinition() != G4ChargedGeantino::ChargedGeantinoDefinition())
      return false;
  }

  //use the global time. i.e. the time from the beginning of the event
  //
  // Get the Touchable History:
  //
  const G4TouchableHistory *myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());

  if(verboseLevel>5){
    for (int i=0;i<myTouch->GetHistoryDepth();i++){
      std::string detname=myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
      int copyno=myTouch->GetVolume(i)->GetCopyNo();
      G4cout << "Volume " <<detname <<" Copy Nr. " << copyno << G4endl;
    }
  }
  //
  // Get the hit coordinates. Start and End Point
  //
  G4ThreeVector coord1 = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector coord2 = aStep->GetPostStepPoint()->GetPosition();

  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  //
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

  int id = myTouch->GetVolume()->GetCopyNo();
  // ignore hits from invalid detectors for now
  // TODO: remove when ready
  if (id == -1) {
    return true;
  }

  // For the XML-based Pixel geometry, the identifier is equivalent 
  // to the copy number of our volume. 
  TrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1,
                     lP2,
                     edep,
                     aStep->GetPreStepPoint()->GetGlobalTime(),
                     trHelp.GetParticleLink(),
                     id);
  return true; 
}
