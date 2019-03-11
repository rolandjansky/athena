/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "BeamEffectsAlg.h"

// Athena headers
#include "CxxUtils/make_unique.h"
#include "GeneratorObjects/McEventCollection.h"

// HepMC includes
#include "HepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

Simulation::BeamEffectsAlg::BeamEffectsAlg( const std::string& name, ISvcLocator* pSvcLocator ):
  AthAlgorithm( name, pSvcLocator ),
  m_inputMcEventCollection("GEN_EVENT"),
  m_outputMcEventCollection("BeamTruthEvent"),
  m_genEventManipulators(this), //private ToolHandleArray
  m_ISFRun(false)

{
  declareProperty("InputMcEventCollection", m_inputMcEventCollection,  "The name of the input McEventCollection");
  declareProperty("OutputMcEventCollection", m_outputMcEventCollection, "The name of the output McEventCollection");
  declareProperty("GenEventManipulators",   m_genEventManipulators,    "BeamEffectsAlg will send the read-in GenEvent to each individual IGenEventManipulator.");
  declareProperty("ISFRun",                 m_ISFRun , "temporary property so that we don't change the output in the initial switch to this code");
}

/** Destructor */
Simulation::BeamEffectsAlg::~BeamEffectsAlg()
{

}

/** Athena algorithm's interface method initialize() */
StatusCode Simulation::BeamEffectsAlg::initialize()
{
  // the Tools for event manipulation
  if (!m_genEventManipulators.empty())
    {
      ATH_CHECK(m_genEventManipulators.retrieve());
    }
  return StatusCode::SUCCESS;
}

StatusCode Simulation::BeamEffectsAlg::execute()
{
  if(!m_inputMcEventCollection.isValid())
    {
      ATH_MSG_FATAL("No input McEventCollection called " << m_inputMcEventCollection.name() << " in StoreGate.");
      return StatusCode::FAILURE;
    }
  if (!m_outputMcEventCollection.isValid()) m_outputMcEventCollection = CxxUtils::make_unique<McEventCollection>(*m_inputMcEventCollection);
  // loop over the event in the mc collection
  for (const auto& currentGenEvent : *m_outputMcEventCollection)
    {
      // skip empty events
      if ( !currentGenEvent ) continue;
      ATH_CHECK(this->patchSignalProcessVertex(*currentGenEvent));
      // manipulate the GenEvent if needed : validating, filtering, smearing, dispersion, ...
      for (const auto& manipulator : m_genEventManipulators)
        {
          // call manipulate(..) in the current GenEventManipulator
          ATH_CHECK(manipulator->manipulate(*currentGenEvent));
        }
    }

  return StatusCode::SUCCESS;
}

/** Athena algorithm's interface method finalize() */
StatusCode Simulation::BeamEffectsAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode Simulation::BeamEffectsAlg::patchSignalProcessVertex(HepMC::GenEvent& ge) const
{
  //Ensure that we have a valid signal_process_vertex
  if( !ge.signal_process_vertex() )
    {
      if(m_ISFRun)
        {
          ATH_MSG_DEBUG("No signal_process_vertex found - creating a dummy GenVertex.");
          CLHEP::HepLorentzVector signalPos( 0.0, 0.0, 0.0, 0.0);
          HepMC::GenVertex *signalVertex = new HepMC::GenVertex( signalPos );
          // ge will now take ownership of the signal process vertex
          ge.set_signal_process_vertex( signalVertex );
        }
      else
        {
          if (!ge.vertices_empty())
            {
              ATH_MSG_DEBUG("No signal_process_vertex found - using the first GenVertex in the event.");
              HepMC::GenVertex *signalVertex = *(ge.vertices_begin());
              ge.set_signal_process_vertex( signalVertex );
            }
        }
      if( !ge.signal_process_vertex() ) // Insanity check
        {
          if (!ge.vertices_empty())
            {
              ATH_MSG_ERROR("Failed to set signal_process_vertex for GenEvent!!");
              return StatusCode::FAILURE;
            }
          ATH_MSG_WARNING("No signal_process_vertex found. Empty GenEvent!");
        }
    }
  else
    {
      ATH_MSG_DEBUG("signal_process_vertex set by Generator.");
    }
  return StatusCode::SUCCESS;
}
