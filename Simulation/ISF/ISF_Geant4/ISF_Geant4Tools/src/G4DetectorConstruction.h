/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4DetectorConstruction.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_G4DETECTORCONSTRUCTION_H
#define ISF_GEANT4TOOLS_G4DETECTORCONSTRUCTION_H 1

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;

// Geant4
#include "G4VUserDetectorConstruction.hh"

/** @class G4DetectorConstruction

    @author: sarka.todorova@cern.ch
*/
class G4DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  G4DetectorConstruction();

  ~G4DetectorConstruction();

  void dummyDetector();

  G4VPhysicalVolume* Construct();

private:

  // Logical volumes
  //
  G4LogicalVolume*          m_worldLog;

  // Physical volumes
  //
  G4VPhysicalVolume*        m_worldPhys;
};

#endif
