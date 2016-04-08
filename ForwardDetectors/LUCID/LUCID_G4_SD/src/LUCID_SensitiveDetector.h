/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_G4_SD_LUCID_SensitiveDetector_h
#define LUCID_G4_SD_LUCID_SensitiveDetector_h

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Athena headers
#include "LUCID_SimEvent/LUCID_SimHitCollection.h"
#include "StoreGate/WriteHandle.h"

class LUCID_HitHelper;

class G4Step;
class G4TouchableHistory;

class LUCID_SensitiveDetector : public G4VSensitiveDetector
{
public:
  // Constructor
  LUCID_SensitiveDetector(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~LUCID_SensitiveDetector() { /* I don't own myHitColl if all has gone well */ }

  // Initialize from G4
  void Initialize(G4HCofThisEvent *) override final;

  // Process the hits from G4
  bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

 private:

  LUCID_SensitiveDetector            (const LUCID_SensitiveDetector&);
  LUCID_SensitiveDetector& operator= (const LUCID_SensitiveDetector&);

  SG::WriteHandle<LUCID_SimHitCollection> m_HitColl;
  LUCID_HitHelper*           m_hit;
};

#endif //LUCID_G4_SD_LUCID_SensitiveDetector_h
