/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SD_ZDC_PIXEL_SD_H
#define ZDC_SD_ZDC_PIXEL_SD_H

// Base class
#include "G4VSensitiveDetector.hh"

// Athena headers
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h"
#include "StoreGate/WriteHandle.h"
#include <gtest/gtest_prod.h>

// STL header
#include <string>

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

class ZDC_PixelSD : public G4VSensitiveDetector
{

 FRIEND_TEST( ZDC_PixelSDtest, ProcessHits );
 FRIEND_TEST( ZDC_PixelSDtest, Initialize );
 FRIEND_TEST( ZDC_PixelSDtest, StartOfAthenaEvent );
 FRIEND_TEST( ZDC_PixelSDtest, EndOfAthenaEvent );
 FRIEND_TEST( ZDC_PixelSDtest, AddHit );
 public:
  // Constructor
  ZDC_PixelSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~ZDC_PixelSD() { /* I don't own myHitColl if all has gone well */ };

  // Called from ZDC_PixelSDTool::SetupEvent
  void StartOfAthenaEvent ();

  // Initialize from G4
  void Initialize(G4HCofThisEvent *) override final;

  // Process the hits from G4
  G4bool ProcessHits(G4Step *aStep, G4TouchableHistory* ROhist) override final;

  // Called from ZDC_PixelSDTool::Gather
  void EndOfAthenaEvent();

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

private:
  // The hits collection
  SG::WriteHandle<ZDC_SimPixelHit_Collection> m_HitColl;

  double m_Edep_Cherenkov_Pixel   [2][2][80]{};
  int    m_NPhoton_Cherenkov_Pixel[2][2][80]{};

  // The first index is [A_side/C_side] second is [EMXY/HMXY] third is [0...79(63)] channel No
  // I keep 80 channels in the HMXY and add them up later during digitization
};

#endif //ZDC_SD_ZDC_PIXEL_SD_H
