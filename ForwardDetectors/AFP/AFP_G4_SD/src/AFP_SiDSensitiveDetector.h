/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_G4_SD_AFP_SiDSensitiveDetector_h
#define AFP_G4_SD_AFP_SiDSensitiveDetector_h

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "StoreGate/WriteHandle.h"
#include <gtest/gtest_prod.h>

// STL header
#include <string>

class G4Step;
class G4TouchableHistory;

class AFP_SiDSensitiveDetector : public G4VSensitiveDetector
{
 FRIEND_TEST( AFP_SiDSensitiveDetectortest, Initialize );
 FRIEND_TEST( AFP_SiDSensitiveDetectortest, ProcessHits );
 FRIEND_TEST( AFP_SiDSensitiveDetectortest, StartOfAthenaEvent );
 FRIEND_TEST( AFP_SiDSensitiveDetectortest, EndOfAthenaEvent );
 FRIEND_TEST( AFP_SiDSensitiveDetectortest, AddHit );

public:
  // Constructor
  AFP_SiDSensitiveDetector(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~AFP_SiDSensitiveDetector() { /* I don't own myHitColl if all has gone well */ }

  // Called from AFP_SiDSensitiveDetectorTool::SetupEvent
  void StartOfAthenaEvent ();
  // Called from G4 at the start of each G4 event
  void Initialize(G4HCofThisEvent *) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  // Called from AFP_SiDSensitiveDetectorTool::Gather
  void EndOfAthenaEvent();

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }
  
  static constexpr int SiDMaxCnt = 1000;

private:
  int m_nHitID;
  int m_nEventNumber;
  int m_nNumberOfSIDSimHits;

  int m_nNOfSIDSimHits[4];

  float m_delta_pixel_x, m_delta_pixel_y;
  float m_death_edge[4][10];
  float m_lower_edge[4][10];

  // The hits collections
  SG::WriteHandle<AFP_SIDSimHitCollection> m_HitColl;

};

#endif //AFP_G4_SD_AFP_SiDSensitiveDetector_h
