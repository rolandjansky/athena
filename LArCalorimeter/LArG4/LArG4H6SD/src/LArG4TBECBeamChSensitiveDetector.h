/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChSensitiveDetector.hh
#ifndef  LARG4H6SD_LARG4TBECBEAMCHSENSITIVEDETECTOR_H
#define  LARG4H6SD_LARG4TBECBEAMCHSENSITIVEDETECTOR_H

#include "LArG4TBECBeamChHit.h"

#include "G4VSensitiveDetector.hh"

#include <set>
#include <vector>

// Forward declarations.
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class LArG4TBECBeamChCalculator;

class LArG4TBECBeamChSensitiveDetector : public G4VSensitiveDetector
{
public:
  LArG4TBECBeamChSensitiveDetector(G4String name);//,
  ~LArG4TBECBeamChSensitiveDetector();

  // The required functions for all sensitive detectors:
  void Initialize(G4HCofThisEvent* HCE);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);
  void EndOfEvent(G4HCofThisEvent* HCE);

  // Accessor method
  inline LArG4TBECBeamChCalculator* GetCalculator() { return m_calculator; }

private:
  // Pointer to a calculator class.
  LArG4TBECBeamChCalculator* m_calculator;

  // The name of the sensitive detector.
  G4String m_detectorName;

  // The name associated the hit collection of this sensitive
  // detector.
  G4String m_HCname;

  public:
  typedef std::vector< LArG4TBECBeamChHit*>  m_Hit_t;
  typedef m_Hit_t::iterator m_Hit_pointer;
  // Used to have static access, but I see no place in LXR where that's needed
  m_Hit_t m_Hits;

 private:
  LArG4TBECBeamChSensitiveDetector (const LArG4TBECBeamChSensitiveDetector&);
  LArG4TBECBeamChSensitiveDetector& operator= (const LArG4TBECBeamChSensitiveDetector&);
};

#endif // __LArG4TBECBeamChSensitiveDetector_H__
