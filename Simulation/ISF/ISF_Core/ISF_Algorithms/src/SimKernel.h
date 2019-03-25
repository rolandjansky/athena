/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGS_SIMKERNEL_H
#define ISF_ALGS_SIMKERNEL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// ISF includes
#include "ISF_Event/SimSvcID.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Interfaces/IInputConverter.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC_Interfaces/IZeroLifetimePatcher.h"

// forward declarations
namespace PMonUtils {
  class CustomBenchmark;
}

namespace ISF {

    class IParticleMgr;
    class ITruthSvc;
    class ISimulationSvc;
    class IMonitoringTool;
    class IEventFilterTool;


  /** @class SimKernel

      This is the purely sequential simulation kernel, executed once per event.
      The ISF::SimKernel is configured by providing:
        - one particle stack service (ISF::IParticleMgr )
        - one truth record service (ISF::ITruthSvc)
        - n simulation services for the subdetector geometries ( ISF::ISimulationSvc )

      The AthAlgorithm::initialize() framework call triggers:
        - retrieval of the given components, and subsequent inialize() calls on them
        - translation of the ServiceHandleArray<ISF::ISimulationSvc> into a std::vector<ISF::ISimulationSvc*>
          for fast access of the geometry service

      The AthAlgorithm::execute() framework call triggers the following sequence of calls :
        - (1) the stack filling call to the ParticleMgr
        - (2) the setupEvent() calls to the provided components
        - (3) the loop over the particle stack
        - (4) the releaseEvent() calls to the provided components

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch

     */
  class SimKernel : public AthAlgorithm {

  public:
    /** Constructor with parameters */
    SimKernel( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~SimKernel();

    /** Athena algorithm's interface method initialize() */
    StatusCode  initialize(); /** Athena algorithm's interface method execute() */
    StatusCode  execute();

    /** Athena algorithm's interface method finalize() */
    StatusCode  finalize();

  private:
    StatusCode initSimSvcs( SimSelectorToolArray &simSelectorTools);

    /** Convert input generator particles to ISFParticles and copy input
        generator truth collection into output simulation truth collection */
    StatusCode prepareInput(SG::ReadHandle<McEventCollection>& inputTruth,
                            SG::WriteHandle<McEventCollection>& outputTruth,
                            ISFParticleContainer& simParticles) const;


    /** Input/output truth collections and input conversion */
    SG::ReadHandle<McEventCollection>    m_inputHardScatterEvgen; //!< input hard scatter collection
    SG::ReadHandle<McEventCollection>    m_inputPileupEvgen;      //!< input pileup collection
    SG::WriteHandle<McEventCollection>   m_outputHardScatterTruth;//!< output hard scatter truth collection
    SG::WriteHandle<McEventCollection>   m_outputPileupTruth;     //!< output pileup truth collection
    ServiceHandle<IInputConverter>       m_inputConverter;        //!< input->ISFParticle converter

    /** Central particle broker service */
    ServiceHandle<IParticleBroker>       m_particleBroker;
    /** Central truth service */
    ServiceHandle<ITruthSvc>             m_truthRecordSvc;

    /** The Simulation Selector Chains */
    ToolHandleArray<ISimulationSelector> m_simSelectors[AtlasDetDescr::fNumAtlasRegions];

    /** Quasi-Stable Particle Simulation Patcher */
    ServiceHandle<Simulation::IZeroLifetimePatcher> m_qspatcher;

    /** The Event Filters */
    ToolHandleArray<IEventFilterTool>    m_eventFilters;

    /** The Memory Info Tool */
    bool                                 m_doMemMon;
    ToolHandle<IMonitoringTool>          m_memMon;
    unsigned int                         m_memUsageEvts;

    /** Simulators to be used */
    std::vector<ISimulationSvc*>         m_simSvcs;     //!< SimSvc handles
    std::vector<std::string>             m_simSvcNames; //!< SimSvc names
    ISF::SimSvcID                        m_numSimSvcs;  //!< total number of SimSvcs used

    /** keep track of the number of events processed */
    unsigned int                         m_numISFEvents;

    /** CPU Benchmarking */
    bool                                 m_doCPUMon;    //!< whether we use PMonUtils or not
    //PMonUtils::CustomBenchmark          *m_benchPDGCode; //TODO: implement this if feasible
    //PMonUtils::CustomBenchmark          *m_benchGeoID;   //TODO: implement this if feasible
    PMonUtils::CustomBenchmark          *m_benchSimID;

    /** Statistics */
    long int                             m_numParticles; //!< total number of particles handled by ISF

    /** tuning */
    size_t                               m_maxParticleVectorSize; //!< number of particles simultaneously sent to simulator
  };
}


#endif //> !ISF_ALGS_SIMKERNEL_H
