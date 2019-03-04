/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT Sensitive Detectors where the sensitive volume identifier is already the required identifier
// (previous versions built up the identifier from the parent volume numeric identifiers)
// GeoModelXml introduced this way of doing things.
//
// The Geant 4 Hits are processed here. For every hit: get the local start and end points, 
// energy deposit, hit time, barcode, and identifier;  put these in a list to be persistified.
//

// class headers
#include "SctSensorGmxSD.h"

// athena includes
#include "MCTruth/TrackHelper.h"

// Geant4 includes
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

SctSensorGmxSD::SctSensorGmxSD(const std::string& name, const std::string& hitCollectionName)
  : SctSensorSD( name, hitCollectionName) { }

SctSensorGmxSD::~SctSensorGmxSD() { }

G4bool SctSensorGmxSD::ProcessHits(G4Step *aStep, G4TouchableHistory * /* not used */) {
  double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) {
    if (aStep->GetTrack()->GetDefinition() != G4Geantino::GeantinoDefinition() &&
       aStep->GetTrack()->GetDefinition() != G4ChargedGeantino::ChargedGeantinoDefinition())
      return false;
  }
  edep *= CLHEP::MeV;

  const G4TouchableHistory *myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());
  //
  // Get the hit start and end point local coordinates
  //
  G4ThreeVector coord1 = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector coord2 = aStep->GetPostStepPoint()->GetPosition();
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector localPosition1 = transformation.TransformPoint(coord1);
  G4ThreeVector localPosition2 = transformation.TransformPoint(coord2);
  HepGeom::Point3D<double> lP1, lP2;

  // GeoModelXml sensors with depth along local z
  lP1[SiHit::xEta] = localPosition1[0]*CLHEP::mm;
  lP1[SiHit::xPhi] = localPosition1[1]*CLHEP::mm;
  lP1[SiHit::xDep] = localPosition1[2]*CLHEP::mm;

  lP2[SiHit::xEta] = localPosition2[0]*CLHEP::mm;
  lP2[SiHit::xPhi] = localPosition2[1]*CLHEP::mm;
  lP2[SiHit::xDep] = localPosition2[2]*CLHEP::mm;

  //
  //    Get the indexes of which detector the hit is in
  //
  const int id = myTouch->GetVolume(0)->GetCopyNo();

  // get the HepMcParticleLink from the TrackHelper
  TrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1, lP2, edep, aStep->GetPreStepPoint()->GetGlobalTime(), trHelp.GetParticleLink(), id);

  return true;
}
