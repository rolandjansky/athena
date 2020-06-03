/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/EscapedEnergyProcessing.h"
#include "LArG4Code/LArG4CalibSD.h"


// From Geant4 10.2
#include "G4AtlasTools/G4MultiSensitiveDetector.hh"

#include "G4TouchableHandle.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "CxxUtils/AthUnlikelyMacros.h"

#undef DEBUG_PROCESS

//using namespace LArG4;

EscapedEnergyProcessing::EscapedEnergyProcessing(LArG4CalibSD* SD)
  : m_defaultSD(SD)
{}

EscapedEnergyProcessing::~EscapedEnergyProcessing()
{;}

G4bool EscapedEnergyProcessing::Process( G4Step* fakeStep ) {
  // Escaped energy requires special processing.  The energy was not
  // deposited in the current G4Step, but at the beginning of the
  // particle's track.  For example, if a neutrino escapes the
  // detector, the energy should be recorded at the point where the
  // neutrino was created, not at the point where it escaped the
  // detector.

  // The calibration sensitive detector has a special routine to
  // handle this: CalibrationSensitiveDetector::SpecialHit().  We
  // have to locate the correct sensitive detector in order to
  // deposit the escaped energy.

  // Escaped energies from non-sensitive volumes requires a special
  // sensitive detector.

  G4StepPoint*        fakePreStepPoint = fakeStep->GetPreStepPoint();
  G4StepPoint*        fakePostStepPoint = fakeStep->GetPostStepPoint();
  G4TouchableHandle a_touchableHandle = fakePreStepPoint->GetTouchableHandle();
  // Here is the actual location of the escaped energy, as the
  // fourth component of a vector.  (Why not just pass the energy in
  // the G4Step?  Because CalibrationSensitiveDetector::SpecialHit()
  // is not designed just for escaped energy, but any unusual cases
  // in which we have to deposit any of the calibration energies in
  // a place other than the current step.)

  std::vector<G4double> energies;
  energies.resize(4,0.);
  energies[3] = fakeStep->GetTotalEnergyDeposit();//a_energy;

  // If we find a calibration sensitive detector in the original
  // volume, use that sensitive detector.  If we don't, use a
  // "default" sensitive detector that's designed to accumulate
  // these "dead material" energies.

  // Find out in which physical volume our hit is located.
  G4VPhysicalVolume* physicalVolume = a_touchableHandle->GetVolume();

  // If the volume is valid...
  if (ATH_LIKELY(physicalVolume)) {
#ifdef DEBUG_PROCESS
    G4cout << "LArG4::EscapedEnergyProcessing::Process - "
           << " particle created in volume '"
           << physicalVolume->GetName()
           << "'" << G4endl;
#endif

    G4LogicalVolume* logicalVolume = physicalVolume->GetLogicalVolume();

    // To prevent some potential problems with uninitialized values,
    // set the material of the step points.
    fakePreStepPoint->SetMaterial( logicalVolume->GetMaterial()  );
    fakePostStepPoint->SetMaterial( logicalVolume->GetMaterial()  );

    // Is this volume associated with a sensitive detector?
    G4VSensitiveDetector* sensitiveDetector =
      logicalVolume->GetSensitiveDetector();

    if (ATH_LIKELY(sensitiveDetector)) {
#ifdef DEBUG_PROCESS
      G4cout << "   ... which has sensitive detector '" << sensitiveDetector->GetName() << "'" << G4endl;
#endif
      LArG4CalibSD* larG4CalibSD(nullptr);
      G4MultiSensitiveDetector* larG4MultSD = dynamic_cast<G4MultiSensitiveDetector*>(sensitiveDetector);
      // Most probably this is a LArG4MultSD
      if (ATH_LIKELY(larG4MultSD)) {
        G4bool found(false);
        for (unsigned int i=0; i<larG4MultSD->GetSize(); i++) {
          larG4CalibSD = dynamic_cast<LArG4CalibSD*>(larG4MultSD->GetSD(i));
          if (larG4CalibSD) {
            // We found one.  Process the energy.
            found = true;
#ifdef DEBUG_PROCESS
            G4cout << "   ... which contains calibration sensitive detector '" << larG4CalibSD->GetName() << "'" << G4endl;
#endif
            larG4CalibSD->SpecialHit(fakeStep,energies);
          }
        }// -- for (1)
        if (ATH_UNLIKELY(!found)) {
#ifdef DEBUG_PROCESS
          const G4ThreeVector& a_point = fakePreStepPoint->GetPosition();
          G4cout << "LArG4::EscapedEnergyProcessing::Process - "
                 << " particle (x,y,z)=("
                 << a_point.x() << ","
                 << a_point.y() << ","
                 << a_point.z() << ") with energy="
                 << fakeStep->GetTotalEnergyDeposit()
                 << " was created in volume '"
                 << physicalVolume->GetName()
                 << "'; could not find a CalibrationSensitiveDetector"
                 << " within  LArG4MultSD'"
                 << larG4MultSD->GetName() << "'"
                 << G4endl;
          G4cout << "   Using default sensitive detector." << G4endl;
#endif
          m_defaultSD->SpecialHit( fakeStep, energies );
          return false; // error
        }// - !found (1)

      } else {// larG4MultSD !=0
        // Next possibility - LArG4CalibSD
        larG4CalibSD = dynamic_cast<LArG4CalibSD*>(sensitiveDetector);
        if (ATH_LIKELY(larG4CalibSD)) {
#ifdef DEBUG_PROCESS
          G4cout << "   ... which is a LArG4CalibSD " << G4endl;
#endif
          larG4CalibSD->SpecialHit( fakeStep, energies );
        }
        else {// larG4CalibSD!=0
              // --- backwards compatibility ---

              // We've found a sensitive detector.  Is it a CalibrationSensitiveDetector?
          LArG4CalibSD* calibSD = dynamic_cast<LArG4CalibSD*>(sensitiveDetector);
          if ( calibSD ) {
            // It is!  Process the energy.
#ifdef DEBUG_PROCESS
            G4cout << "   ... which is a calibration sensitive detector " << G4endl;
#endif
            calibSD->SpecialHit( fakeStep, energies );
          } else {
#ifdef DEBUG_PROCESS
            const G4ThreeVector& a_point = fakePreStepPoint->GetPosition();
            G4cout << "LArG4::EscapedEnergyProcessing::Process - "
                   << " particle (x,y,z)=("
                   << a_point.x() << ","
                   << a_point.y() << ","
                   << a_point.z() << ") with energy="
                   << fakeStep->GetTotalEnergyDeposit()
                   << " was created in volume '"
                   << physicalVolume->GetName()
                   << "' with sensitive detector '"
                   << sensitiveDetector->GetName()
                   << "' which is not a CalibrationSensitiveDetector."
                   << G4endl;
            G4cout << "   Using default sensitive detector." << G4endl;
#endif
            m_defaultSD->SpecialHit( fakeStep, energies );
            return false; // error
          }
        }
      }
    } else {
      // If we reach this point, then the particle whose energy
      // has escaped was created outside of a sensitive region.
#ifdef DEBUG_PROCESS
      const G4ThreeVector& a_point = fakePreStepPoint->GetPosition();
      G4cout << "LArG4::EscapedEnergyProcessing::Process - "
             << " particle (x,y,z)=("
             << a_point.x() << ","
             << a_point.y() << ","
             << a_point.z() << ") with energy="
             << fakeStep->GetTotalEnergyDeposit()
             << " was created in volume '"
             << physicalVolume->GetName()
             << "' which is not sensitive."
             << G4endl;
      G4cout << "   Using default sensitive detector." << G4endl;
#endif
      m_defaultSD->SpecialHit( fakeStep, energies );
      return false; // error
    }
  }

  // Processing was OK.
  return true;
}
