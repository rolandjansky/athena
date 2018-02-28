/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "ISF_Interfaces/ISimulationSvc.h"
#include "ISF_Interfaces/ISimulationSelector.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// STL includes
#include <array>


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

private:
  /// Returns the simulator to use for the given particle
  ISimulationSvc& identifySimulator(const ISF::ISFParticle& particle) const;

  /// Input Generator Truth collection
  SG::ReadHandleKey<McEventCollection> m_inputEvgenKey;
  /// Output Simulation Truth collection
  SG::WriteHandleKey<McEventCollection> m_outputTruthKey;

  /// Input converter service (from Generator->ISF particle types)
  ServiceHandle<IInputConverter> m_inputConverter;

  /// The simulation selectors defining the "routing chain"
  std::array<ToolHandleArray<ISimulationSelector>, AtlasDetDescr::fNumAtlasRegions> m_simSelectors;

  /// When no appropriate simulator can be found for a given particle, the particle is sent to this "particle killer":
  ServiceHandle<ISimulationSvc> m_particleKillerSimulationSvc;
};

} // namespace ISF

#endif // ISF_SIMKERNELMT_H
