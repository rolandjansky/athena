/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IPHYSICSOPTIONTOOL_H
#define G4ATLASINTERFACES_IPHYSICSOPTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
class G4String;
class G4VPhysicsConstructor;

/** @class IPhysicsListTool IPhysicsListTool.h "G4AtlasInterfaces/IPhysicsListTool.h"
 *
 *  Abstract interface to Geant4 Physics list classes
 *
 *  @author Edoardo Farina
 *  @date   2015-05-15
 */

class IPhysicsOptionTool : virtual public IAlgTool
{
public:
  IPhysicsOptionTool() {}
  virtual ~IPhysicsOptionTool() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IPhysicsOptionTool( "IPhysicsOptionTool" , 1 , 0 ) ;
    return IID_IPhysicsOptionTool ;
  }

  // Method needed to register G4VPhysicsConstructor into G4VmodularPhysicsList
  virtual G4VPhysicsConstructor* GetPhysicsOption() = 0 ;
};
#endif
