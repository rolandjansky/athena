/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_G4_SD_AFP_TDSensitiveDetector_h
#define AFP_G4_SD_AFP_TDSensitiveDetector_h

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "StoreGate/WriteHandle.h"

// STL header
#include <string>

class G4Step;
class G4TouchableHistory;

class AFP_TDSensitiveDetector : public G4VSensitiveDetector
{

public:
  // Constructor
  AFP_TDSensitiveDetector(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~AFP_TDSensitiveDetector() { /* I don't own myHitColl if all has gone well */ }

  // Called from AFP_TDSensitiveDetectorTool::SetupEvent
  void StartOfAthenaEvent ();
  // Called from G4 at the start of each G4 event
  void Initialize(G4HCofThisEvent *) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  // Called from AFP_TDSensitiveDetectorTool::Gather
  void EndOfAthenaEvent();

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

private:
  int m_nHitID;
  int m_nEventNumber;
  int m_nNumberOfTDSimHits;
  int m_nNOfTDSimHits[4][32];
  // The hits collection
  SG::WriteHandle<AFP_TDSimHitCollection> m_HitColl;
};

#endif //AFP_G4_SD_AFP_TDSensitiveDetector_h
