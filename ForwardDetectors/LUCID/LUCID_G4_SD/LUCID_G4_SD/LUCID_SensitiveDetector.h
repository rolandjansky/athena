/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SensitiveDetector_h
#define LUCID_SensitiveDetector_h

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

#include "LUCID_G4_SD/LUCID_HitHelper.h"
#include "LUCID_SimEvent/LUCID_SimHitCollection.h"

#include <iostream>
#include <fstream>

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class LUCID_SensitiveDetector : public FADS::FadsSensitiveDetector {
public:
  
  LUCID_SensitiveDetector(std::string);
  ~LUCID_SensitiveDetector();
  
  bool ProcessHits(G4Step*, G4TouchableHistory*);
  
  void Initialize  (G4HCofThisEvent*);
  void BeginOfEvent(G4HCofThisEvent*);
  void EndOfEvent  (G4HCofThisEvent*);
  
 private:

  LUCID_SensitiveDetector            (const LUCID_SensitiveDetector&);
  LUCID_SensitiveDetector& operator= (const LUCID_SensitiveDetector&);  

  LUCID_SimHitCollection*    m_lucidSimHitCollection;
  AthenaHitsCollectionHelper m_hitCollHelp;
  LUCID_HitHelper*           m_hit;
};

#endif

