// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef BEAMEFFECTS_BEAMEFFECTSALG_H
#define BEAMEFFECTS_BEAMEFFECTSALG_H 1

// base class header
#include "AthenaBaseComps/AthAlgorithm.h"

// Athena includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "HepMC_Interfaces/IGenEventManipulator.h"

class McEventCollection;
namespace HepMC {
  class GenEvent;
}

namespace SimTesting {
  class BeamEffectsAlg_test;
}

namespace Simulation {
  /** @class BeamEffectsAlg

      This AthAlgorithm reads in the McEventCollection produced by the
      generator and writes out an updated McEventCollection for use by
      simulation.  The IGenEventManipulators are used to apply the
      effects of beam properties to the GenEvents.
     */
  class BeamEffectsAlg : public AthAlgorithm {
    friend class SimTesting::BeamEffectsAlg_test;
  public:
    //** Constructor with parameters */
    BeamEffectsAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~BeamEffectsAlg();

    /** Athena algorithm's interface method initialize() */
    StatusCode  initialize() override final; /** Athena algorithm's interface method execute() */
    StatusCode  execute() override final;

    /** Athena algorithm's interface method finalize() */
    StatusCode  finalize() override final;
  private:
    /** Ensure that the GenEvent::signal_process_vertex has been set */
    StatusCode patchSignalProcessVertex(HepMC::GenEvent& ge) const;
    SG::ReadHandle<McEventCollection> m_inputMcEventCollection;
    SG::WriteHandle<McEventCollection> m_outputMcEventCollection;
    ToolHandleArray<IGenEventManipulator> m_genEventManipulators;     //!< event manipulators (executed in given order)
    bool m_ISFRun; /** Temporary property so that we don't change the output in the initial switch to this code. */
  };
}
#endif //BEAMEFFECTS_BEAMEFFECTSALG_H
