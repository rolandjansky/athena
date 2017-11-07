/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IG4GEOMETRYCONFIGURATIONTOOL_H
#define G4ATLASINTERFACES_IG4GEOMETRYCONFIGURATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"

// Geant4

/** @class IG4GeometryConfigurationTool IG4GeometryConfigurationTool.h "G4AtlasInterfaces/IG4GeometryConfigurationTool.h"
 *
 *  Abstract interface for tools to work at the Geant4 geometry step
 *
 *  @author ADA
 *  @date   2016-08-04
 */

class IG4GeometryConfigurationTool : virtual public IAlgTool {
public:
  virtual ~IG4GeometryConfigurationTool() {}

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IG4GeometryConfigurationTool, 1, 0);

  /// Initialize
  virtual StatusCode preGeometryConfigure() const = 0;
  virtual StatusCode postGeometryConfigure() const = 0;

};

#endif
