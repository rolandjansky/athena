/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IFASTSIMULATION_H
#define G4ATLASINTERFACES_IFASTSIMULATION_H

#include "GaudiKernel/IAlgTool.h"

/** @class IFastSimulation IFastSimulation.h "G4AtlasInterfaces/IFastSimulation.h"
 *  
 *  Abstract interface to service for Geant4 fast simulation classes
 * 
 *  @author Zach Marshall
 *  @date   2014-12-20
 */

static const InterfaceID IID_IFastSimulation( "IFastSimulation" , 1 , 0 ) ;

class IFastSimulation : virtual public IAlgTool {
 public:
  IFastSimulation() {}
  virtual ~IFastSimulation() {}
  static const InterfaceID& interfaceID() { return IID_IFastSimulation ; }
};

#endif
