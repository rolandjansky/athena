/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October 2016
 * @brief Core Athena algorithm for the Integrated Simulation Framework
 */

#ifndef ISF_SIMKERNELMT_H
#define ISF_SIMKERNELMT_H 1

// Interfaces to ISF Tools and Services
#include "ISF_Interfaces/IInputConverter.h"
#include "ISF_Interfaces/ISimulatorTool.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Interfaces/SimulationFlavor.h"
#include "ISF_Interfaces/IEntryLayerTool.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IParticleOrderingTool.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC_Interfaces/IZeroLifetimePatcher.h"

// TrackRecordCollection (and TrackRecord)
#include "TrackRecord/TrackRecordCollection.h"

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// STL includes
#include <array>
#include <map>

// forward declarations
namespace ISFTesting {
  class SimKernelMT_test;
}


namespace ISF {

/**
 * @class SimKernelMT
 * @headerfile SimKernelMT.h
 * @brief Core Athena algorithm for the Integrated Simulation Framework
 *
 * This Athena algorithm dispatches all selected particles from the input collection
 * to the attached simulators.
 */
class SimKernelMT final : public AthAlgorithm {

  /** Allow the test class access to all methods */
  friend class ISFTesting::SimKernelMT_test;

public:
  /// Implements standard AthAlgorithm constructor
  SimKernelMT( const std::string& name, ISvcLocator* pSvcLocator );

  /// Implements empty destructor
  virtual ~SimKernelMT();

  /// Check user configuration and configure the algorithm state accordingly
  StatusCode initialize() override;

  /** @brief Check the validity of the MC truth event in the current Athena event
   *  @retval StatusCode::SUCCESS all HepMC::GenEvents of the current Athena event are valid
   *  @retval StatusCode::FAILURE the MC truth collection could not be read
   *  @retval StatusCode::FAILURE the MC truth collection does not have a valid tree structure */
  StatusCode execute() override;

  /// Implements empty finalize (implementation required by AthAlgorithm)
  StatusCode finalize() override;

  bool isClonable() const override { return true; }

private:
  /// Returns the simulator to use for the given particle
  const ISimulatorTool& identifySimulator(const ISF::ISFParticle& particle) const;

  /// Input Generator Truth collection
  SG::ReadHandleKey<McEventCollection> m_inputEvgenKey{this, "InputEvgenCollection", "", "Input EVGEN collection."};
  /// Output Simulation Truth collection
  SG::WriteHandleKey<McEventCollection> m_outputTruthKey{this, "OutputTruthCollection", "", "Output Truth collection."};
  /// Output TrackRecordCollections
  SG::WriteHandleKey<TrackRecordCollection> m_caloEntryLayerKey{this, "CaloEntryLayerKey", "CaloEntryLayer", ""};
  SG::WriteHandleKey<TrackRecordCollection> m_muonEntryLayerKey{this, "MuonEntryLayerKey", "MuonEntryLayer", ""};
  SG::WriteHandleKey<TrackRecordCollection> m_muonExitLayerKey{this, "MuonExitLayerKey", "MuonExitLayer", ""};

  /// Force geoID recalculation for each particle
  Gaudi::Property<bool> m_forceGeoIDSvc{this, "AlwaysUseGeoIDSvc", false, "Force geoID recalculation for each particle" };

  /// Input converter service (from Generator->ISF particle types)
  ServiceHandle<IInputConverter> m_inputConverter{this, "InputConverter", "", "Input McEventCollection->ISFParticleContainer conversion service."};

  /// Central truth service
  ServiceHandle<ITruthSvc> m_truthRecordSvc{this,"TruthRecordService", "ISF_MC15aPlusTruthService", ""};

  /// Quasi-Stable Particle Simulation Patcher
  ServiceHandle<Simulation::IZeroLifetimePatcher> m_qspatcher{this, "QuasiStablePatcher", "", "Quasi-Stable Particle Simulation Patcher"};

  /// Simulation Tools
  ToolHandleArray<ISimulatorTool> m_simulationTools{this, "SimulationTools", {}, ""};

  /// When no appropriate simulator can be found for a given particle, the particle is sent to this "particle killer":
  PublicToolHandle<ISimulatorTool> m_particleKillerTool{this, "ParticleKillerTool", "", ""};

  /// AthenaTool responsible for writing Calo/Muon Entry/Exit Layer collection
  PublicToolHandle<IEntryLayerTool> m_entryLayerTool{this, "EntryLayerTool", "ISF_EntryLayerToolMT", ""};

  /// Service to set particle GeoIDs
  ServiceHandle<IGeoIDSvc> m_geoIDSvc{this, "GeoIDSvc", "", "Service to set particle GeoIDs"};

  /// Tool to set particle ordering
  ToolHandle<IParticleOrderingTool> m_orderingTool{this, "ParticleOrderingTool", "", "Tool to set order of particles"};

  /// The simulation selectors defining the "routing chain"
  std::array<ToolHandleArray<ISimulationSelector>, AtlasDetDescr::fNumAtlasRegions> m_simSelectors;

  /// Map of the simulation flavours used in this job to the corresponding Simulation Services
  std::map<ISF::SimulationFlavor, ISimulatorTool*> m_simToolMap;

  /// Number of particles simultaneously sent to simulator
  size_t m_maxParticleVectorSize{10240};
};

} // namespace ISF

#endif // ISF_SIMKERNELMT_H
