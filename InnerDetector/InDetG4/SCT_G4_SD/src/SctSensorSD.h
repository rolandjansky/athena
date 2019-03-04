/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   SCT Sensitive Detector class
****************************************************************/

#ifndef SCT_G4_SD_SCTSENSORSD_H
#define SCT_G4_SD_SCTSENSORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// For the hits
#include "InDetSimEvent/SiHitCollection.h"
#include "StoreGate/WriteHandle.h"

// G4 needed classes
class G4Step;
class G4TouchableHistory;

class SctSensorSD : public G4VSensitiveDetector
{
public:
  // Constructor
  SctSensorSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~SctSensorSD() { /* If all goes well we do not own myHitColl here */ }

  // Deal with each G4 hit
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

  // For setting up the hit collection
  void Initialize(G4HCofThisEvent*) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
      could get rather tricky, but the idea is to allow fast simulations to use the very
      same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

private:
  void indexMethod(const G4TouchableHistory *myTouch, double coord1z, int &brlEcap, int &layerDisk, int &etaMod, int &phiMod, int &side);
protected:
  // The hits collection
  SG::WriteHandle<SiHitCollection> m_HitColl;
};

#endif //SCT_G4_SD_SCTSENSORSD_H
