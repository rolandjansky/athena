/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IPHYSICSLISTTOOL_H
#define G4ATLASINTERFACES_IPHYSICSLISTTOOL_H

#include "GaudiKernel/IAlgTool.h"

class G4VUserPhysicsList;

/** @class IPhysicsListTool IPhysicsListTool.h "G4AtlasInterfaces/IPhysicsListTool.h"
 *
 *  Abstract interface to Geant4 Physics list classes
 *
 *  @author Andrea Dell'Acqua
 *  @date   2014-10-01
 */

class IPhysicsListTool : virtual public IAlgTool
{
public:
  IPhysicsListTool() {}
  virtual ~IPhysicsListTool() {}
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID IID_IPhysicsListTool( "IPhysicsListTool" , 1 , 0 ) ;
    return IID_IPhysicsListTool ;
  }

  /** Build the physics list.  This method should be called when G4 is ready
      for the initialization of the physics list */
  virtual void CreatePhysicsList () = 0 ;

  /** Simple function to get the physics list pointer, in case it is needed */
  virtual G4VUserPhysicsList* GetPhysicsList() = 0 ;

  /** Get the run manager out and assign the physics list to it.  This should
      be called once the run manager exists, before the physics list initialization
      needs to happen. */
  virtual void SetPhysicsList() = 0 ;

  /** Set the options to be used by the physics lists.  Things like EM physics
      options and binning, neutron cuts, etc. */
  virtual void SetPhysicsOptions() = 0 ;

};
#endif
