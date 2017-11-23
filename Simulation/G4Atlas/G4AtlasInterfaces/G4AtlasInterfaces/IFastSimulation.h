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
class G4VFastSimulationModel;

class IFastSimulation : virtual public IAlgTool {
 public:
  IFastSimulation() {}
  virtual ~IFastSimulation() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IFastSimulation, 1, 0);
  /** Method to initialize the Fast Simulation mdoel itself - this is our hook for initialization in G4,
   since in a multi-threaded setup the Fast Simulation models are local and they might need to be
   initialized in each thread.  Saves us using the AlgTool's initialize() for this. */
  virtual StatusCode initializeFastSim() = 0;

  /** Method to be called at the end of the *athena* event.  This
   ensures a simple call when we have multiple G4 events per athena
   event.  This is where any tidying that may need to be done at the
   end of the *athena* event rather that the G4 event can be done. */
  virtual StatusCode EndOfAthenaEvent() = 0;

 protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() = 0;
};

#endif
