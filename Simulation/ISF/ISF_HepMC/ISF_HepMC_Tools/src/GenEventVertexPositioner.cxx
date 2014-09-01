/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventVertexPositioner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_HepMC_Tools/GenEventVertexPositioner.h"

// HepMC includes
#include "HepMC/GenEvent.h"

// ISF_HepMC includes
#include "ISF_HepMC_Interfaces/ILorentzVectorGenerator.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

/** Constructor **/
ISF::GenEventVertexPositioner::GenEventVertexPositioner( const std::string& t,
                                                         const std::string& n,
                                                         const IInterface* p )
  : AthAlgTool(t,n,p),
    m_vertexShifters()
{
    declareInterface<ISF::IGenEventManipulator>(this);

    declareProperty("VertexShifters"   ,       m_vertexShifters    );
}


/** Athena algtool's Hooks */
StatusCode  ISF::GenEventVertexPositioner::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");

  // retrieve the BeamCondService
  if ( m_vertexShifters.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not retrieve VertexShifters '" << m_vertexShifters << "'. Abort.");
      return StatusCode::FAILURE;
  }

  // everything set up properly
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::GenEventVertexPositioner::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}


/** modifies (displaces) the given GenEvent */
StatusCode ISF::GenEventVertexPositioner::manipulate(HepMC::GenEvent& ge)
{
  // loop over all given VertexShifter AthenaTools
  VertexShifters::const_iterator shifterIt  = m_vertexShifters.begin();
  VertexShifters::const_iterator shifterEnd = m_vertexShifters.end();
  for ( ; shifterIt!=shifterEnd; ++shifterIt) {

    // call VertexShifter and let it compute the current shift
    CLHEP::HepLorentzVector *curShift = (*shifterIt)->generate();
    if (!curShift) {
      ATH_MSG_ERROR("Vertex Shifter AthenaTool returned zero-pointer! Ignore.");
      continue;
    }

    ATH_MSG_VERBOSE("Retrieved Vertex shift of: " << *curShift);

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
    }

    // memory cleanup
    delete curShift;
  }

  // set signal process vertex if currently unset
  if( !ge.signal_process_vertex() ) {
    HepMC::GenVertex *signalVertex = *(ge.vertices_begin());
    ge.set_signal_process_vertex( signalVertex );
  }

  return StatusCode::SUCCESS;
}
