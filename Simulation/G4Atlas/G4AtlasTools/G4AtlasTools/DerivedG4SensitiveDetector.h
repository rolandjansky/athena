/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4MySensitiveDetector_h
#define G4MySensitiveDetector_h

#include "G4VSensitiveDetector.hh"

class G4MySensitiveDetector : public G4VSensitiveDetector {

public:
  
  G4MySensitiveDetector(G4String SDname):G4VSensitiveDetector(SDname)
  {

  }

  ~G4MySensitiveDetector() {}

  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist)
  {
    (void)aStep;
    (void)ROhist;
    return true;
  }

};
#endif
