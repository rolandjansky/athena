/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasInterfaces_IGeo2G4Svc_H
#define G4AtlasInterfaces_IGeo2G4Svc_H

#include "GaudiKernel/IService.h"
#include "Geo2G4SvcBase.h"

class IGeo2G4Svc: virtual public IService,
                  virtual public Geo2G4SvcBase {
public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IGeo2G4Svc, 1, 0);
};
#endif // G4AtlasInterfaces_IGeo2G4Svc_H
