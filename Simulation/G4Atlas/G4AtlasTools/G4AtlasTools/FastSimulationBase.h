/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_FASTSIMULATIONBASE_H
#define G4ATLASTOOLS_FASTSIMULATIONBASE_H

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

/// @class FastSimulationBase
/// @todo TODO needs class documentation
class FastSimulationBase : virtual public IFastSimulation, public AthAlgTool {
 public:
  FastSimulationBase(const std::string& type, const std::string& name,
                     const IInterface *parent);
  virtual ~FastSimulationBase() {}

  /// @brief Construct and setup the fast simulation model.
  ///
  /// This method invokes the makeFastSimModel of the derived concrete tool type
  /// and assigns the configured regions. Errors are reported if regions are
  /// missing. In multi-threading jobs, this method is called once per worker
  /// thread.
  StatusCode initializeFastSim() override;

  /** End of an athena event - do any tidying up required at the end of each *athena* event. */
  virtual StatusCode EndOfAthenaEvent() override { return StatusCode::SUCCESS; }

  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 protected:
  /// Retrieve the current Fast Simulation Model. In MT, this means the
  /// thread-local Fast Simulation Model. Otherwise, it is simply the single
  /// Fast Simulation Model.
  G4VFastSimulationModel* getFastSimModel();

  /// All the regions to which this fast sim is assigned
  std::vector<std::string> m_regionNames;
  /// This Fast Simulation has no regions associated with it.
  bool m_noRegions;

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
  /// The Fast Simulation Model to which this thing corresponds
  G4VFastSimulationModel* m_FastSimModel;
#endif
};

#endif
