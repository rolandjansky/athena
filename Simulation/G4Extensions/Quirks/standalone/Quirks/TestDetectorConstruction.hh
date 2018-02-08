/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestDetectorConstruction_hh
#define TestDetectorConstruction_hh

class G4VPhysicalVolume;
class G4LogicalVolume;

#include "G4VUserDetectorConstruction.hh"

class TestDetectorConstruction : public G4VUserDetectorConstruction {
public:
    G4VPhysicalVolume* Construct();
private:
    G4LogicalVolume* hall_log;
    G4VPhysicalVolume* hall_phys;
    G4LogicalVolume* block_log;
    G4VPhysicalVolume* block_phys;
};

#endif
