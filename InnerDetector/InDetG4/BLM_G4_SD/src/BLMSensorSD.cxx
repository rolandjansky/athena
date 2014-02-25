/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BLM Sensitive Detector class
//   Bostjan Macek 14.february.2008
//###############################################

#include "BLM_G4_SD/BLMSensorSD.h"
// BLM Sensitive Detector.
#include <fstream>
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
//
// Geant4 Stuff
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

// CLHEP transform
#include "CLHEP/Geometry/Transform3D.h"
// Units
#include "CLHEP/Units/SystemOfUnits.h"

// to permit access to StoreGate
#include "GaudiKernel/ISvcLocator.h"
#include "SimHelpers/DetectorGeometryHelper.h"
#include "MCTruth/TrackHelper.h"

// logging
#include "AthenaBaseComps/AthMsgStreamMacros.h"

//////////////////////////////////////////////////////////////////////////////////
// Initialize static data
///////////////////////////////////////////////////////////////////////////////

static FADS::SensitiveDetectorEntryT<BLMSensorSD> mdtsd("BLMSensorSD");

BLMSensorSD::BLMSensorSD(G4String name) :
  FADS::FadsSensitiveDetector(name),
  BLM_HitColl(0)
{
}

BLMSensorSD::~BLMSensorSD()
{
}

void BLMSensorSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  ATH_MSG_DEBUG("Initializing BLM_SD");
  // Create a fresh map to store the hits
  //BLM_HitColl = new SiHitCollection("BLMHits");
  BLM_HitColl = m_hitCollHelp.RetrieveNonconstCollection<SiHitCollection>("BLMHits");
}

G4bool BLMSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
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

  //BLM hit stuff
  if(BEcopyNo == 2009)
    {
      TrackHelper trHelp(aStep->GetTrack());
      int barcode = trHelp.GetBarcode();
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
      //std::cout << "BLMBarcode == " << barcode << " Vertex: " << aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() << std::endl;

      int produced_in_diamond = 0;
      if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Pixel::blmDiamondLog")
        produced_in_diamond = 1;
      else if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Pixel::blmModLog")
        produced_in_diamond = 2;
      else if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Pixel::blmWallLog")
        produced_in_diamond = 3;

      SiHit newBLMHit(lP1, lP2, edep, aStep->GetPreStepPoint()->GetGlobalTime(), barcode, 0, 0, myTouch->GetVolume(1)->GetCopyNo()-222, 0, primaren, produced_in_diamond);
      BLM_HitColl->Insert(newBLMHit);
    }
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BLMSensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  //exporting BCM hits
  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection(BLM_HitColl);
}
