/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SD_ZDC_STRIP_SD_H
#define ZDC_SD_ZDC_STRIP_SD_H

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"
#include "StoreGate/WriteHandle.h"

// STL header
#include <string>

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

class ZDC_StripSD : public G4VSensitiveDetector
{

 public:

  ZDC_StripSD(const std::string& name, const std::string& hitCollectionName);
  ~ZDC_StripSD() { /* I don't own myHitColl if all has gone well */ };

  // Called from ZDC_StripSDTool::SetupEvent
  void StartOfAthenaEvent ();
  // Initialize from G4
  void Initialize(G4HCofThisEvent *) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  // Called from ZDC_StripSDTool::Gather
  void EndOfAthenaEvent();

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

 private:
  SG::WriteHandle<ZDC_SimStripHit_Collection> m_HitColl;
  float    m_Edep_Cherenkov_Strip[2][4];
  int   m_NPhoton_Cherenkov_Strip[2][4];
};

#endif //ZDC_SD_ZDC_STRIP_SD_H
