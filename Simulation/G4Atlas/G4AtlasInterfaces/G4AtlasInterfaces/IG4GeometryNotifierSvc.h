/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IG4GEOMETRYNOTIFIERSVC_H
#define G4ATLASINTERFACES_IG4GEOMETRYNOTIFIERSVC_H

#include "GaudiKernel/IService.h"

#include <vector>
#include <string>

/** @class IG4GeometryNotifierSvc IG4GeometryNotifierSvc.h "G4AtlasInterfaces/IG4GeometryNotifierSvc.h"
 *
 *  Abstract interface to service for Geant4 geometry notifier classes
 */

class IG4GeometryNotifierSvc : virtual public IService {
 public:
  IG4GeometryNotifierSvc() {}
  virtual ~IG4GeometryNotifierSvc() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IG4GeometryNotifierSvc, 1, 0);
  virtual void SetCurrentDetectorName(std::string) = 0;

  virtual const std::string GetCurrentDetectorName() const = 0;
};

#endif
