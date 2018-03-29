/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasInterfaces_IParallelWorldTool_H
#define G4AtlasInterfaces_IParallelWorldTool_H

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "G4VUserParallelWorld.hh"

class G4VPhysicalVolume;

/** @class IParallelWorldTool IParallelWorldTool.h "G4AtlasInterfaces/IParallelWorldTool.h"
 *  
 *  Abstract interface to a detector construction tool. 
 * 
 *  @author ADA
 *  @date   2015-03-20
 */

class IParallelWorldTool : public G4VUserParallelWorld, virtual public IAlgTool {
 public:
  IParallelWorldTool(std::string n):G4VUserParallelWorld(n) {;}
 
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IParallelWorldTool, 1, 0);
  
// from G4VUserParallelWorld 

  virtual void Construct() = 0;
  
// set method to define the World volume from outside

  virtual void SetWorld() = 0;
  
  virtual G4VUserParallelWorld* GetParallelWorld() = 0;
 
};
#endif
