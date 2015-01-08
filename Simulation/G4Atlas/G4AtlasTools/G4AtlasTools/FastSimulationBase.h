/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_FASTSIMULATIONBASE_H
#define G4ATLASINTERFACES_FASTSIMULATIONBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IFastSimulation.h"
#include "G4VFastSimulationModel.hh"

// STL library
#include <string>
#include <vector>

class FastSimulationBase : virtual public IFastSimulation , public G4VFastSimulationModel , public AthAlgTool {
 public:
  FastSimulationBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~FastSimulationBase() {}

  /** Athena method, used to get out the G4 geometry and set up the fast simulations
    This is horrible, but the G4 method called Initialize is called at the beginning
    of every event.  This method is the one that is called ONLY by athena at the
    beginning of the job */
  StatusCode initialize() override;

  // Dummy implementations of all virtual methods from G4
  /** IsApplicable function, called once per particle.  If the model is applicable, then
    the model trigger will be called at every step.  If it is not applicable, then it will
    not be called, and there is no major overhead for the fast simulation.  Only get
    generic information about the particle type here, no kinematic information, to make
    a decision with (only called in the appropriate part of the geometry to begin with). 
    This is basically a trick to speed things up when you don't want to apply the fast
    simulation to all particles in the simulation.  */
  virtual G4bool IsApplicable(const G4ParticleDefinition&) override { return false; }

  /** ModelTrigger.  Fires if the fast simulation should be triggered in this step. 
    Here you have full kinematic information from the track, as well as precise position
    information.  This is where the real decision is made for all ATLAS fast simulations. */
  virtual G4bool ModelTrigger(const G4FastTrack &) override { return false; }

  /** DoIt.  Actually apply the fast simulation.  This is separate from the above, 
    because Geant4 queries all processes, and "in principle" one can fire at higher
    priority than this fast simulation. You get both a track and the step information, 
    which you are free to update here.  In practice, all our fast simulations kill off
    the track in DoIt and make sure that it is removed from the stack. */
  virtual void DoIt(const G4FastTrack&, G4FastStep&) override { }

  /** Version of the ModelTrigger for particles that have come to rest in the detector. */
  virtual G4bool AtRestModelTrigger(const G4FastTrack&) override { return false; }

  /** Version of DoIt for particles that have come to rest in the detector */
  virtual void   AtRestDoIt  (const G4FastTrack&, G4FastStep&) override { }

 protected:
  std::vector<std::string> m_regionNames; //!< All the volumes to which this fast sim is assigned

};

#endif
