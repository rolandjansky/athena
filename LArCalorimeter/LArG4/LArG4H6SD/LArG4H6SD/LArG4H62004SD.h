/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H62004SD_H__
#define __LArG4H62004SD_H__

#include "LArG4Code/LArG4Identifier.h"
#include "LArSimEvent/LArHit.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

#include <set>
#include <vector>

// Forward declarations.
class LArVCalculator;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class StoreGateSvc;

class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArHitContainer;


class LArVHitMerger;

class LArG4H62004SD : public FADS::FadsSensitiveDetector
{
public:
  LArG4H62004SD(G4String name);
  virtual ~LArG4H62004SD();

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

  // Width of the time bins for summing hits:
  G4float m_timeBinWidth;

  // LArHitMerger
  std::vector<LArVHitMerger*> m_hitMergers;

  // The name of the sensitive detector.
  G4String m_detectorName;

  // Count the number of invalid hits.
  G4int m_numberInvalidHits;

  // Pointer to the detector store
  StoreGateSvc* m_detStore;

  // Pointers to the identifier helpers
  const LArEM_ID*     m_larEmID;
  const LArFCAL_ID*   m_larFcalID;
  const LArHEC_ID*    m_larHecID;

  // collections
  LArHitContainer* m_emecHitCollection;
  LArHitContainer* m_fcalHitCollection;
  LArHitContainer* m_hecHitCollection;
};

#endif // __LArG4H62004SD_H__
