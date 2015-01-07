/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IFASTSIMULATIONSVC_H
#define G4ATLASINTERFACES_IFASTSIMULATIONSVC_H

#include "GaudiKernel/IService.h"

/** @class IFastSimulationSvc IFastSimulationSvc.h "G4AtlasInterfaces/IFastSimulationSvc.h"
 *  
 *  Abstract interface to service for Geant4 fast simulation classes
 * 
 *  @author Zach Marshall
 *  @date   2014-12-20
 */

static const InterfaceID IID_IFastSimulationSvc( "IFastSimulationSvc" , 1 , 0 ) ;

class IFastSimulationSvc : virtual public IService {
 public:
  IFastSimulationSvc() {}
  virtual ~IFastSimulationSvc() {}
  static const InterfaceID& interfaceID() { return IID_IFastSimulationSvc ; }
};

#endif
