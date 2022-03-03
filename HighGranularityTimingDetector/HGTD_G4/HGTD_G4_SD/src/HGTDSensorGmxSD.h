/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// HGTDSensorSD.h, (c) ATLAS Detector software
//////////////////////////////////////////////////////////

#ifndef HGTD_G4_Gmx_SD_HGTDSENSORSD_H
#define HGTD_G4_Gmx_SD_HGTDSENSORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "StoreGate/WriteHandle.h"

//new LAr stuff
#include "HGTD_Identifier/HGTD_ID.h"
#include "InDetSimEvent/SiHitCollection.h"

// Geant4 headers
#include "G4ThreeVector.hh"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;

class HGTDSensorGmxSD : public G4VSensitiveDetector
{

public:

    // Constructor
    HGTDSensorGmxSD(const std::string& name, const std::string& hitCollectionName);

    // Destructor
    virtual ~HGTDSensorGmxSD() {}

    // Process the hits from G4
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

    // Initialize from G4
    void Initialize(G4HCofThisEvent *) override final;

private:

    SG::WriteHandle<SiHitCollection> m_HitColl;

};

#endif // HGTD_G4_Gmx_SD_HGTDSENSORSD_H
