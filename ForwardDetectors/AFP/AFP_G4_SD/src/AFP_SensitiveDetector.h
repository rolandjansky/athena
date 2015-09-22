/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_G4_SD_AFP_SensitiveDetector_h
#define AFP_G4_SD_AFP_SensitiveDetector_h

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

//FIXME Try to avoid using preprocessor constants
#define TDMAXQEFF 0.15
#define TDMAXCNT  4000
#define SIDMAXCNT 1000

class AFP_SensitiveDetector : public G4VSensitiveDetector
{

public:
  // Constructor
  AFP_SensitiveDetector(const std::string& name, const std::string& TDhitCollectionName, const std::string& SIDhitCollectionName);

  // Destructor
  ~AFP_SensitiveDetector() { /* I don't own myHitColl if all has gone well */ }

  // Called from AFP_SensitiveDetectorTool::SetupEvent
  void StartOfAthenaEvent ();
  // Called from G4 at the start of each G4 event
  void Initialize(G4HCofThisEvent *) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  // Called from AFP_SensitiveDetectorTool::Gather
  void EndOfAthenaEvent();

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  //template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

private:
  int m_nHitID;
  int m_nEventNumber;
  int m_nNumberOfTDSimHits;
  int m_nNumberOfSIDSimHits;

  int m_nNOfTDSimHits[4][32];
  int m_nNOfSIDSimHits[4];

  float m_delta_pixel_x, m_delta_pixel_y;
  float m_death_edge[4][10];
  float m_lower_edge[4][10];

  // The hits collections
  SG::WriteHandle<AFP_TDSimHitCollection> m_pTDSimHitCollection;
  SG::WriteHandle<AFP_SIDSimHitCollection> m_pSIDSimHitCollection;

};

#endif //AFP_G4_SD_AFP_SensitiveDetector_h
