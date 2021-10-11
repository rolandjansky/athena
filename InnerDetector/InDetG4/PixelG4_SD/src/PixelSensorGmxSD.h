/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Pixel Sensitive Detector class for GeoModelXML
 ****************************************************************/

#ifndef PIXEL_G4_SD_PIXELSENSORGMXSD_H
#define PIXEL_G4_SD_PIXELSENSORGMXSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "InDetSimEvent/SiHitCollection.h"
#include "StoreGate/WriteHandle.h"
#include <gtest/gtest_prod.h>

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

class PixelSensorGmxSD : public G4VSensitiveDetector
{
 FRIEND_TEST( PixelSensorGmxSDtest, Initialize );
 FRIEND_TEST( PixelSensorGmxSDtest, ProcessHits );
 FRIEND_TEST( PixelSensorGmxSDtest, AddHit );
 public:
  // Constructor
  PixelSensorGmxSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  virtual ~PixelSensorGmxSD() { /* I don't own myHitColl if all has gone well */ }

  // Process the hits from G4
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  // Initialize from G4
  virtual void Initialize(G4HCofThisEvent *) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

 private:
  // The hits collection
  SG::WriteHandle<SiHitCollection> m_HitColl;
};

#endif //PIXEL_G4_SD_PIXELSENSORGMXSD_H
