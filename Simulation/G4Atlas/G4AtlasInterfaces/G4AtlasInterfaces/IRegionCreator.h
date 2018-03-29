/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IRegionCreator_H
#define G4ATLASINTERFACES_IRegionCreator_H

#include "GaudiKernel/IAlgTool.h"

// Geant4
#include "G4LogicalVolume.hh"

/** @class IRegionCreator IRegionCreator.h "G4AtlasInterfaces/IRegionCreator.h"
 *  
 *  Abstract interface to create G4 physics regions
 * 
 *  @author ADA
 *  @date   2015-03-23
 */

class IRegionCreator : virtual public IAlgTool {
 public:
  virtual ~IRegionCreator() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IRegionCreator, 1, 0);

  virtual void Dump() = 0;
  
};

#endif
