/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// class TileEscapedEnergyProcessing
//
// Author: Gia Khoriauli  <gia@mail.cern.ch>
// May, 2005
//
// Tile escaped energy calculator
///////////////////////////////////////////////////////////////

#include "TileEscapedEnergyProcessing.h"
#include "TileGeoG4CalibSD.h"

#include "G4TouchableHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

TileEscapedEnergyProcessing::TileEscapedEnergyProcessing(const int /*verboseLevel*/)
  : m_escaped(false),
    m_escapedEnergy(0),
    m_energy5(0)//,
    //m_verboseLevel(verboseLevel)
{
  ;
}
TileEscapedEnergyProcessing::~TileEscapedEnergyProcessing() {

}

G4bool TileEscapedEnergyProcessing::Process(G4TouchableHandle& a_touchableHandle, G4ThreeVector& a_point,
                                            G4double a_energy) {
  // Escaped energy requires special processing.  The energy was not
  // deposited in the current G4Step, but at the beginning of the
  // particle's track.  For example, if a neutrino escapes the
  // detector, the energy should be recorded at the point where the
  // neutrino was created, not at the point where it escaped the
  // detector.

  // The first time this routine is called, we have to set up some
  // Geant4 structures for the fake step.

  static G4StepPoint* fakePreStepPoint = 0;
  static G4StepPoint* fakePostStepPoint = 0;
  static G4Step* fakeStep = 0;

  // Create a phoney pre-step and post-step point, and use them to
  // initialize a phoney G4Step.

  if (fakeStep == 0) {
    fakeStep = new G4Step();
    fakePreStepPoint = fakeStep->GetPreStepPoint();
    fakePostStepPoint = fakeStep->GetPostStepPoint();
  }

  // Set the touchable volume at PreStepPoint:
  fakePreStepPoint->SetTouchableHandle(a_touchableHandle);
  fakePreStepPoint->SetPosition(a_point);
  fakePreStepPoint->SetGlobalTime(0.);

  // For now, try setting this to be the same as the
  // PreStepPoint.
  fakePostStepPoint->SetTouchableHandle(a_touchableHandle);
  fakePostStepPoint->SetPosition(a_point);
  fakePostStepPoint->SetGlobalTime(0.);

  // The total energy deposit in the step is actually irrelevant.
  fakeStep->SetTotalEnergyDeposit(a_energy);

  // Find out in which physical volume our hit is located.
  G4VPhysicalVolume* volume = fakeStep->GetPreStepPoint()->GetPhysicalVolume();

  // If the volume is valid...
  if (volume != 0) {
    // Is this volume associated with a sensitive detector?
    G4VSensitiveDetector* sensitiveDetector = volume->GetLogicalVolume()->GetSensitiveDetector();

    if (sensitiveDetector != 0) {
      // We've found a sensitive detector.
      TileGeoG4CalibSD* calibSD = dynamic_cast<TileGeoG4CalibSD*>(sensitiveDetector);

      if (calibSD != 0) {
        G4String volumeName = volume->GetName();
        if (volumeName.find("Tile") == G4String::npos) {
          G4cout << "WARNING TileEscapedEnergyProcessing::Process - escaped particle was created in "
                 << volume->GetName() << ", out of TILE" << G4endl;
          G4cout << "WARNING: add escaped energy " << a_energy <<" to energy5 " << G4endl;
          m_energy5 += a_energy;
          return false;}  //World volume or no Tile volume / false sensitive

        m_escaped = true;
        m_escapedEnergy = a_energy;

        G4String SDname = calibSD->GetName();
        //store escaped energy in the appropriate hit
        Call_SD_ProcessHits(fakeStep, SDname);
      } else {
        return false; // error
      }
    } else {
      // If we reach this point, then the particle whose energy
      // has escaped was created outside of a sensitive region.
      G4cout << "WARNING: TileEscapedEnergyProcessing::Process - Escaped particle was created in "
             << volume->GetName() << " which is not sensitive." << G4endl;
      G4cout << " add escaped energy " << a_energy <<" to energy5 " << G4endl;
      m_energy5 += a_energy;
      return false;
    }
  }

  return true;  // All went well.
}

//this method will store escaped particle energy in the particle 'mother'hit.
//That's when escaped particle is created in real sensitive volume of Tile
G4bool TileEscapedEnergyProcessing::Call_SD_ProcessHits(G4Step* aStep, G4String &SDname) {
  G4TouchableHistory* ROhis = (G4TouchableHistory*) (aStep->GetPreStepPoint()->GetTouchable());

  G4SDManager *sdManager = G4SDManager::GetSDMpointer();
  G4VSensitiveDetector * TileCalibSD_G4 = sdManager->FindSensitiveDetector(SDname);
  TileGeoG4CalibSD * TileCalibSD = dynamic_cast<TileGeoG4CalibSD*>(TileCalibSD_G4);

  G4bool esc_ok = TileCalibSD ? TileCalibSD->ProcessHits(aStep, ROhis) : false;
  return esc_ok;
}
