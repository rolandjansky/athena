/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_FASTSIMULATIONBASE_H
#define G4ATLASINTERFACES_FASTSIMULATIONBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IFastSimulation.h"
#include "G4VFastSimulationModel.hh"

// Members
#include "G4VFastSimulationModel.hh"
#ifdef ATHENAHIVE
#  include "tbb/concurrent_unordered_map.h"
#endif

// STL library
#include <string>
#include <vector>
#include <thread>

class FastSimulationBase : virtual public IFastSimulation, public AthAlgTool {
 public:
  FastSimulationBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~FastSimulationBase() {}

  /** Athena method, used to get out the G4 geometry and set up the fast simulations
    This is horrible, but the G4 method called Initialize is called at the beginning
    of every event.  This method is the one that is called ONLY by athena at the
    beginning of the job */
  StatusCode initializeFastSim() override;

  /** End of an athena event - do any tidying up required at the end of each *athena* event. */
  virtual StatusCode EndOfAthenaEvent() override { return StatusCode::SUCCESS; }

  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**);

 protected:
  /// Retrieve the current Fast Simulation Model. In hive, this means the
  /// thread-local Fast Simulation Model. Otherwise, it is simply the single
  // Fast Simulation Model.
  G4VFastSimulationModel* getFastSimModel();

  std::vector<std::string> m_regionNames; //!< All the regions to which this fast sim is assigned
  bool m_noRegions; //!< This Fast Simulation has no regions associated with it.

 private:

  /// Set the current model. In hive, this gets assigned as the thread-local model
  void setFastSimModel(G4VFastSimulationModel*);

#ifdef ATHENAHIVE
  /// Thread-to-FastSimModel concurrent map type
  typedef tbb::concurrent_unordered_map < std::thread::id,
                                          G4VFastSimulationModel*,
                                          std::hash<std::thread::id> > FastSimModelThreadMap_t;
  /// Concurrent map of Fast Sim Models, one for each thread
  FastSimModelThreadMap_t m_fastsimmodelThreadMap;
#else
  G4VFastSimulationModel* m_FastSimModel;             ///!< The Fast Simulation Model to which this thing corresponds
#endif
};

#endif
