/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// HGTD Sensitive Detector.
// The Hits are processed here. For every hit, the position and information
// on the sensor in which the interaction happened are obtained.

// Class header
#include "HGTDSensorGmxSD.h"

// Athena headers
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4VisExtent.hh" // Added to get extent of sensors
#include "G4VSolid.hh" // Added to get extent of sensors

// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <stdint.h>
#include <string.h>

HGTDSensorGmxSD::HGTDSensorGmxSD(const std::string& name, const std::string& hitCollectionName)
    : G4VSensitiveDetector( name ), 
      m_HitColl( hitCollectionName )
{

}

// Initialize from G4
void HGTDSensorGmxSD::Initialize(G4HCofThisEvent *)
{
    if (!m_HitColl.isValid()) m_HitColl = std::make_unique<SiHitCollection>();
}

G4bool HGTDSensorGmxSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
    if (verboseLevel>5) G4cout << "Process Hit" << G4endl;

    G4double edep = aStep->GetTotalEnergyDeposit();
    edep *= CLHEP::MeV;

    if (edep==0.) {
        if (aStep->GetTrack()->GetDefinition() != G4Geantino::GeantinoDefinition() &&
            aStep->GetTrack()->GetDefinition() != G4ChargedGeantino::ChargedGeantinoDefinition())
            return false;
    }

    //
    // Get the Touchable History:
    //
    const G4TouchableHistory*  myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());

    if(verboseLevel>5){
        for (int i=0;i<myTouch->GetHistoryDepth();i++){
            std::string detname = myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
            int copyno = myTouch->GetVolume(i)->GetCopyNo();
            G4cout << "Volume " << detname << " Copy Nr. " << copyno << G4endl;
        }
    }

    //
    // Get the hit coordinates. Start and End Point
    //
    G4ThreeVector startCoord = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector endCoord   = aStep->GetPostStepPoint()->GetPosition();

    // Create the SiHits

    const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();

    G4ThreeVector localPosition1 = transformation.TransformPoint(startCoord);
    G4ThreeVector localPosition2 = transformation.TransformPoint(endCoord);

    HepGeom::Point3D<double> lP1,lP2;
    //TODO need to check
    lP1[SiHit::xEta] = localPosition1[1]*CLHEP::mm; //long edge of the module
    lP1[SiHit::xPhi] = localPosition1[0]*CLHEP::mm; //short edge of the module
    lP1[SiHit::xDep] = localPosition1[2]*CLHEP::mm; //depth (z)

    lP2[SiHit::xEta] = localPosition2[1]*CLHEP::mm;
    lP2[SiHit::xPhi] = localPosition2[0]*CLHEP::mm;
    lP2[SiHit::xDep] = localPosition2[2]*CLHEP::mm;

  
  // For the XML-based Pixel geometry, the identifier is equivalent
  // to the copy number of our volume.


    int id = myTouch->GetVolume()->GetCopyNo();


    TrackHelper trHelp(aStep->GetTrack());
    m_HitColl->Emplace(lP1,
                       lP2,
                       edep,
                       aStep->GetPreStepPoint()->GetGlobalTime(),
                       trHelp.GetParticleLink(),
                       id);

    return true;
}
