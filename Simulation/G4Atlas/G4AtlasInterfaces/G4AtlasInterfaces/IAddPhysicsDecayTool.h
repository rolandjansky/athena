/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IAddPhysicsDecayTool_H
#define G4ATLASINTERFACES_IAddPhysicsDecayTool_H

#include "GaudiKernel/IAlgTool.h"
class G4String;


/** @class IAddPhysicsDecayTool IAddPhysicsDecayTool.h "G4AtlasInterfaces/IAddPhysicsDecayTool.h"
 *
 *  Abstract interface to Geant4 Physics list classes
 *
 *  @author Edoardo Farina
 *  @date   2015-05-15
 */

class IAddPhysicsDecayTool : virtual public IAlgTool
{
public:
  IAddPhysicsDecayTool() {}
  virtual ~IAddPhysicsDecayTool() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IAddPhysicsDecayTool, 1, 0);


  virtual void AddPhysicsDecay() = 0;

};
#endif
