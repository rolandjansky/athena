/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IUserLimitsSvc_H
#define G4ATLASINTERFACES_IUserLimitsSvc_H

#include "GaudiKernel/IService.h"

/** @class IUserLimitsSvc IUserLimitsSvc.h "G4AtlasInterfaces/IUserLimitsSvc.h"
 *
 *
 *
 */

class IUserLimitsSvc : virtual public IService {
 public:

  virtual ~IUserLimitsSvc() {}

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IUserLimitsSvc, 1, 0);

};

#endif
