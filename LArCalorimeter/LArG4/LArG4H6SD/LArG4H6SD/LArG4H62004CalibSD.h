/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H62004CalibSD_H__
#define __LArG4H62004CalibSD_H__

#include "LArG4Code/LArG4Identifier.h"
#define private protected
#include "LArG4Code/CalibrationSensitiveDetector.h"
#undef private
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

#include <set>
#include <vector>

// Forward declarations.

class LArVCalibHitMerger;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class LArG4H62004CalibSD : public LArG4::CalibrationSensitiveDetector, public FADS::FadsSensitiveDetector
{
public:
  LArG4H62004CalibSD(G4String name,bool doInit=true);
//  ~LArG4H62004CalibSD();

  // The required functions for all sensitive detectors:
  virtual void Initialize(G4HCofThisEvent* HCE);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

  // The calibration sensitive detector can receive a message that
  // is outside Geant4's normal hit processing: for a particular
  // G4Step, get the identifier and deposite a specified amount of
  // energy.
  virtual G4bool SpecialHit(G4Step* step, const std::vector<G4double>& energy);

  // Accessor method
//  inline LArG4::VCalibrationCalculator* GetCalculator() { return m_calculatorH6; }

// protected:
//  inline void SetCalculator(LArG4::VCalibrationCalculator* calculator) { m_calculatorH6 = calculator; }

private:
  // Pointer to a calculator class.
//  LArG4::VCalibrationCalculator* m_calculator;

  // LArCalibHitMerger pointers
//  std::vector<LArVCalibHitMerger*> m_hitMergers;

  // The name of the sensitive detector.
//  G4String m_detectorName;

  // The name associated with the hit collection(s) of this
  // sensitive detector.  Note that the name of the hit collections
  // does not have to be the same as the name of the detector, and
  // we make use of this in LArG4.
//  G4String m_HCname;

  // Count the number of invalid steps.
//  G4int m_numberInvalidSteps;

  // Pointer to the message service
//  IMessageSvc* m_msgSvc;

  // Static flag + method which creates instances of
  // ** EscapedEnergyProcessing
  // ** CalibrationDefaultProcessing
  static bool m_initProcessing;

//  void PlotHitPos(G4Step* step, LArG4Identifier id);

 protected:

  virtual void initializeProcessing();

private:
  LArG4H62004CalibSD (const LArG4H62004CalibSD&);
  LArG4H62004CalibSD& operator= (const LArG4H62004CalibSD&);
};

#endif // __LArG4H62004CalibSD_H__
