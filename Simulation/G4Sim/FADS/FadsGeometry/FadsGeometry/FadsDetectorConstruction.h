/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsDetectorConstruction_H
#define FadsDetectorConstruction_H

#include "G4VUserDetectorConstruction.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace FADS {

  class FadsDetectorConstruction: public G4VUserDetectorConstruction
  {
  public:
    FadsDetectorConstruction();
    ~FadsDetectorConstruction();

    G4VPhysicalVolume* Construct();

    void SetEnvelope(G4LogicalVolume *);
    void SetPhysicalVolume(G4VPhysicalVolume *);
    G4LogicalVolume *GetEnvelope();
    void CheckGeometry();
    void SetGeometryCheckLevel(int i);
  private:
    int geometryCheckLevel;
    bool isGeometry;
    G4LogicalVolume* topEnvelope;
    G4VPhysicalVolume* topVolume;
  };

}

#endif
