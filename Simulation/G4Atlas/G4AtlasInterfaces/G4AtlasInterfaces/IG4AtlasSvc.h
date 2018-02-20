/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IG4AtlasSvc_H
#define G4ATLASINTERFACES_IG4AtlasSvc_H

#include "GaudiKernel/IService.h"

/** @class IG4AtlasSvc IG4AtlasSvc.h "G4AtlasInterfaces/IG4AtlasSvc.h"
 *  
 *  Abstract interface to service for a Geant4 master service
 * 
 *  @author ADA
 *  @date   2015-04-02
 */

class IG4AtlasSvc : virtual public IService {
 public:
  IG4AtlasSvc() {}
  virtual ~IG4AtlasSvc() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IG4AtlasSvc, 1, 0);
  
};

#endif
