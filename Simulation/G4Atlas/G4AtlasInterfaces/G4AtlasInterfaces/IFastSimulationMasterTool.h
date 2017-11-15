/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IFASTSIMULATIONMASTERTOOL_H
#define G4ATLASINTERFACES_IFASTSIMULATIONMASTERTOOL_H

#include "GaudiKernel/IAlgTool.h"

/** @class IFastSimulationMasterTool IFastSimulationMasterTool.h "G4AtlasInterfaces/IFastSimulationMasterTool.h"
 *
 *  Abstract interface to service for Geant4 fast simulation classes
 *
 *  @author Zach Marshall
 *  @date   2014-12-20
 */

class IFastSimulationMasterTool : virtual public IAlgTool {
 public:
  IFastSimulationMasterTool() {}
  virtual ~IFastSimulationMasterTool() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IFastSimulationMasterTool, 1, 0);
  /** Method for initializing the fast simulation models of each tool.
   Not done in the athena-level initialize() function, as this may
   need to happen once per thread in a multi-threaded environment. */
  virtual StatusCode initializeFastSims() = 0;

  /** End of athena event method.  This should get called once per athena event by the
   framework, somewhere.  Eventually it may be done via a Gaudi function.  This ensures
   a simple call when we have multiple G4 events per athena event.  The function calls
   the Gather() method for all the SD handles that it owns. */
  virtual StatusCode EndOfAthenaEvent() = 0;
};

#endif
