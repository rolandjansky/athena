/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventVertexPositioner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenEventVertexPositioner.h"

// HepMC includes
#include "HepMC/GenEvent.h"

// Framework includes
#include "HepMC_Interfaces/ILorentzVectorGenerator.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

/** Constructor **/
Simulation::GenEventVertexPositioner::GenEventVertexPositioner( const std::string& t,
                                                                const std::string& n,
                                                                const IInterface* p )
  : base_class(t,n,p)
  , m_vertexShifters(this)
{
  declareProperty("VertexShifters"   ,       m_vertexShifters    );
}


/** Athena algtool's Hooks */
StatusCode  Simulation::GenEventVertexPositioner::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");

  // retrieve Vertex Shifters
  if ( !m_vertexShifters.empty() ) {
    ATH_CHECK(m_vertexShifters.retrieve());
  }

  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  Simulation::GenEventVertexPositioner::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}


/** modifies (displaces) the given GenEvent */
StatusCode Simulation::GenEventVertexPositioner::manipulate(HepMC::GenEvent& ge) const
{
  // Grab signal_process_vertex pointer
  const HepMC::GenVertex *signalProcVtx(ge.signal_process_vertex());
  if(!signalProcVtx) {
    ATH_MSG_ERROR("Expected GenEvent::signal_process_vertex() to already have been set at this point!");
    return StatusCode::FAILURE;
  }

  // loop over all given ILorentzVectorGenerator AlgTools
  for (const auto& vertexShifter : m_vertexShifters) {

    // call VertexShifter and let it compute the current shift
    CLHEP::HepLorentzVector *curShift = vertexShifter->generate();
    if (!curShift) {
      ATH_MSG_ERROR("Vertex Shifter AthenaTool returned zero-pointer! Ignore.");
      continue;
    }

    ATH_MSG_VERBOSE("Retrieved Vertex shift of: " << *curShift);

    // As signal process vertex is a pointer, there is some risk
    // that the pointer points to a vertex somewhere else in the
    // event, rather than a unique / new vertex, in which case we
    // will modify its position in the loop below and will not need
    // to treat it separately.
    bool modifySigVtx(true);

    // loop over the vertices in the event, they are in respect with another
    //   (code from Simulation/Fatras/FatrasAlgs/McEventPreProcessing.cxx)
    HepMC::GenEvent::vertex_iterator vtxIt    = ge.vertices_begin();
    HepMC::GenEvent::vertex_iterator vtxItEnd = ge.vertices_end();
    for(  ; vtxIt != vtxItEnd; ++vtxIt) {
      // quick access:
      HepMC::GenVertex *curVtx = (*vtxIt);
      const HepMC::FourVector &curPos = curVtx->position();

      // get a copy of the current vertex position
      CLHEP::HepLorentzVector newPos( curPos.x(), curPos.y(), curPos.z(), curPos.t() );
      // and update it with the given smearing
      newPos += (*curShift);

      ATH_MSG_VERBOSE( "Original vtx  position = " << curPos.x() << ", " << curPos.y() << ", " << curPos.z() );
      ATH_MSG_VERBOSE( "Updated  vtx  position = " << newPos );

      // store the updated position in the vertex
      curVtx->set_position( newPos);
      if(modifySigVtx && signalProcVtx==curVtx) {
        modifySigVtx=false;
      }
    }

    // Do the same for the signal process vertex if still required.
    if (modifySigVtx) {
      const HepMC::FourVector &curPos = ge.signal_process_vertex()->position();
      CLHEP::HepLorentzVector newPos( curPos.x(), curPos.y(), curPos.z(), curPos.t() );
      newPos += (*curShift);
      ge.signal_process_vertex()->set_position( newPos);
    }

    // memory cleanup
    delete curShift;
  }

  return StatusCode::SUCCESS;
}
