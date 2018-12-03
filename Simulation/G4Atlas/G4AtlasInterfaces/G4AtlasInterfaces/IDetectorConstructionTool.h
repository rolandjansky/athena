/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasInterfaces_IDetectorConstructionTool_H
#define G4AtlasInterfaces_IDetectorConstructionTool_H

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

/** @class IDetectorConstructionTool IDetectorConstructionTool.h "G4AtlasInterfaces/IDetectorConstructionTool.h"
 *
 *  Abstract interface to a detector construction tool.
 *
 *  @author ADA
 *  @date   2015-02-20
 */

class IDetectorConstructionTool : public G4VUserDetectorConstruction, virtual public IAlgTool {
 public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IDetectorConstructionTool, 1, 0);

// from G4VUserDetectorConstruction

  virtual G4VPhysicalVolume* Construct() = 0;

// set method to define the World volume from outside

  virtual void SetWorld(G4VPhysicalVolume*) = 0;

  virtual G4VUserDetectorConstruction* GetDetectorConstruction() = 0;

};
#endif
