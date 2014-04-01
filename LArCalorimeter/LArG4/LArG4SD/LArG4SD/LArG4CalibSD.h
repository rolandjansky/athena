/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4CalibSD_H__
#define __LArG4CalibSD_H__

#include "LArG4Code/LArG4Identifier.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include <vector>

// Forward declarations.

namespace LArG4
{
  class VCalibrationCalculator;
}

class LArVCalibHitMerger;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class MsgStream;
class EventInformation;

class LArG4CalibSD : public FADS::FadsSensitiveDetector
{
public:
  LArG4CalibSD(G4String name,bool doInit=true);
  ~LArG4CalibSD();

  // The required functions for all sensitive detectors:
  void Initialize(G4HCofThisEvent* HCE);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);
  void EndOfEvent(G4HCofThisEvent* HCE);

  // The calibration sensitive detector can receive a message that
  // is outside Geant4's normal hit processing: for a particular
  // G4Step, get the identifier and deposite a specified amount of
  // energy.
  G4bool SpecialHit(G4Step* step, const std::vector<G4double>& energy);

  // Accessor method
  inline LArG4::VCalibrationCalculator* GetCalculator() { return m_calculator; }

protected:
  inline void SetCalculator(LArG4::VCalibrationCalculator* calculator) { m_calculator = calculator; }

private:

  LArG4CalibSD(const LArG4CalibSD&);//coverity issue fix. Declared, but not implemented
  LArG4CalibSD& operator=(const LArG4CalibSD&);//coverity issue fix. Declared, but not implemented

  // Pointer to a calculator class.
  LArG4::VCalibrationCalculator* m_calculator;

  // LArCalibHitMerger pointers
  std::vector<LArVCalibHitMerger*> m_hitMergers;

  // The name of the sensitive detector.
  G4String m_detectorName;

  // Count the number of invalid steps.
  G4int m_numberInvalidSteps;

  // Message stream
  MsgStream* m_log;

  // Static flag + method which creates instances of
  // ** EscapedEnergyProcessing
  // ** CalibrationDefaultProcessing
  static bool m_initProcessing;

  void PlotHitPos(G4Step* step, LArG4Identifier id);

  // variables used for producing calibration hits signed with primary particle ID
  EventInformation *m_event_info;
  bool   m_doCalibHitParticleID;

protected:

  virtual void initializeProcessing();

};

#endif // __LArG4CalibSD_H__
