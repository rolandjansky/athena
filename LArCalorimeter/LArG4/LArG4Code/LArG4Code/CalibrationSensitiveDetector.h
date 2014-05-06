/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationSensitiveDetector.h

// The sensitive-detector class for detectors that can have
// calibration hits in LArG4.

// 08-Jan-2004 Bill Seligman

#ifndef LArG4_CalibrationSensitiveDetector_H
#define LArG4_CalibrationSensitiveDetector_H

#include "G4VSensitiveDetector.hh"
#include <vector>


// Forward declarations.
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class LArVCalibHitMerger;
class EventInformation;

namespace LArG4 {

  // Forward declarations within this namespace.
  class VCalibrationCalculator;

  class CalibrationSensitiveDetector : public G4VSensitiveDetector
  {
  public:
    CalibrationSensitiveDetector(G4String name, VCalibrationCalculator* calc);
    ~CalibrationSensitiveDetector();
  
    // The required functions for all sensitive detectors:
    virtual void Initialize(G4HCofThisEvent* HCE);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);
    virtual void EndOfEvent(G4HCofThisEvent* HCE);

    // The calibration sensitive detector can receive a message that
    // is outside Geant4's normal hit processing: for a particular
    // G4Step, get the identifier and deposite a specified amount of
    // energy.

    virtual G4bool SpecialHit(G4Step* step, const std::vector<G4double>& energy);

    // Accessor method
    inline VCalibrationCalculator* GetCalculator() { return m_calculator; }

  private:
    // Pointer to a calculator class.
    VCalibrationCalculator* m_calculator;

    // LArCalibHitMerger pointers
    std::vector<LArVCalibHitMerger*> m_hitMergers;

    // The name of the sensitive detector.
    G4String m_detectorName;

    // Count the number of invalid steps.
    G4int m_numberInvalidSteps;

    // variables used for producing calibration hits signed with primary particle ID
    EventInformation *m_event_info;
    bool   m_doCalibHitParticleID;
  };

} // namespace LArG4

#endif // LArG4_CalibrationSensitiveDetector_H
