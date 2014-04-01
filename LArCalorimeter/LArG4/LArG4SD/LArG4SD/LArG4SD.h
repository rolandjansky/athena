/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4SD_H__
#define __LArG4SD_H__

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include <vector>

// Forward declarations.
class LArVCalculator;
class LArVHitMerger;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class MsgStream;

class LArG4SD : public FADS::FadsSensitiveDetector
{
public:
  LArG4SD(G4String name);
  virtual ~LArG4SD();

  // The required functions for all sensitive detectors:
  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

  // Accessor method
  inline LArVCalculator* GetCalculator() { return m_calculator; }

protected:
  inline void SetCalculator(LArVCalculator* calculator) { m_calculator = calculator; }

private:
  // Pointer to a calculator class.
  LArVCalculator* m_calculator;

  // LArHitMerger pointers
  std::vector<LArVHitMerger*> m_hitMergers;

  // The name of the sensitive detector.
  G4String m_detectorName;

  // Count the number of invalid hits.
  G4int m_numberInvalidHits;

  // Message stream
  MsgStream* m_log;
};

#endif // __LArG4SD_H__
