/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthResetAlg.h"
//
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
//
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <climits>

TruthResetAlg::TruthResetAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
 declareProperty("InputMcEventCollection" , m_inputMcEventCollection  = "TruthEvent");
 declareProperty("OutputMcEventCollection" , m_outputMcEventCollection  = "NewTruthEvent");
}


//----------------------------------------------------
StatusCode TruthResetAlg::initialize() {
//----------------------------------------------------

  ATH_CHECK(m_inputMcEventCollection.initialize());
  ATH_CHECK(m_outputMcEventCollection.initialize());
  return StatusCode::SUCCESS;

}

//-------------------------------------------------
StatusCode TruthResetAlg::execute() {
//-------------------------------------------------

  ATH_MSG_DEBUG( " execute..... " );
  SG::ReadHandle<McEventCollection> inputMcEventCollection(m_inputMcEventCollection);
  if (!inputMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find input McEventCollection called " << inputMcEventCollection.name() << " in store " << inputMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  const HepMC::GenEvent& inputEvent(**(inputMcEventCollection->begin()));

  //Sanity check
  HepMC::GenEvent::particle_const_iterator inputPartIter = inputEvent.particles_begin();
  const HepMC::GenEvent::particle_const_iterator inputPartEnd = inputEvent.particles_end();
  bool inputProblem(false);
  while (inputPartIter!=inputPartEnd) {
    const HepMC::GenParticle& particle(**inputPartIter);
    if (particle.status()==1) {
      if (!particle.production_vertex()) {
        ATH_MSG_ERROR("Status 1 particle without a production vertex!! " << particle);
        inputProblem = true;
      }
    }
    else if (particle.status()==2) {
      if (!particle.production_vertex()) {
        ATH_MSG_ERROR("Status 2 particle without a production vertex!! " << particle);
        inputProblem = true;
      }
      if (!particle.end_vertex()) {
        ATH_MSG_ERROR("Status 2 particle without an end vertex!! " << particle);
        inputProblem = true;
      }
    }
    ++inputPartIter;
  }
  if (inputProblem) {
    ATH_MSG_FATAL("Problems in input GenEvent - bailing out.");
    return StatusCode::FAILURE;
  }

  std::unique_ptr<HepMC::GenEvent> outputEvent = std::make_unique<HepMC::GenEvent>(inputEvent.signal_process_id(),
                                                                                   inputEvent.event_number());

  outputEvent->set_mpi                  ( inputEvent.mpi() );
  outputEvent->set_event_scale          ( inputEvent.event_scale() );
  outputEvent->set_alphaQCD             ( inputEvent.alphaQCD() );
  outputEvent->set_alphaQED             ( inputEvent.alphaQED() );
  if ( inputEvent.cross_section() ) {
    outputEvent->set_cross_section        ( *inputEvent.cross_section());
  }
  if (inputEvent.heavy_ion()) {
    outputEvent->set_heavy_ion(*(inputEvent.heavy_ion()));
  }
  if (inputEvent.pdf_info()) {
    outputEvent->set_pdf_info(*(inputEvent.pdf_info()));
  }
  outputEvent->define_units( inputEvent.momentum_unit(), inputEvent.length_unit() );

  // 1. create a NEW copy of all vertices from inputEvent
  //    taking care to map new vertices onto the vertices being copied
  //    and add these new vertices to this event.
  //    We do not use GenVertex::operator= because that would copy
  //    the attached particles as well.
  std::map<const HepMC::GenVertex*,HepMC::GenVertex*> inputEvtVtxToOutputEvtVtx;
  HepMC::GenEvent::vertex_const_iterator currentVertexIter(inputEvent.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfCurrentListOfVertices(inputEvent.vertices_end());
  ATH_MSG_VERBOSE( "Starting a vertex loop ... " );
  for (; currentVertexIter != endOfCurrentListOfVertices; ++currentVertexIter) {
    const HepMC::GenVertex *pCurrentVertex(*currentVertexIter);
    if (pCurrentVertex->barcode()<-200000) { // FIXME configurable constant
      continue; // skip vertices created by the simulation
    }
    std::unique_ptr<HepMC::GenVertex> copyOfGenVertex =
      std::make_unique<HepMC::GenVertex>(pCurrentVertex->position(), pCurrentVertex->id(), pCurrentVertex->weights() );
    copyOfGenVertex->suggest_barcode( pCurrentVertex->barcode() );
    inputEvtVtxToOutputEvtVtx[pCurrentVertex] = copyOfGenVertex.get();
    outputEvent->add_vertex( copyOfGenVertex.release() );
  } //vertex loop

  // 2. copy the signal process vertex info.
  if ( inputEvent.signal_process_vertex() ) {
    outputEvent->set_signal_process_vertex( inputEvtVtxToOutputEvtVtx[inputEvent.signal_process_vertex()] );
  }
  else {
    outputEvent->set_signal_process_vertex( nullptr );
  }
  //
  // 3. create a NEW copy of all particles from inputEvent
  //    taking care to attach them to the appropriate vertex
  HepMC::GenParticle* beam1{};
  HepMC::GenParticle* beam2{};
  for ( HepMC::GenEvent::particle_const_iterator particleIter = inputEvent.particles_begin();
        particleIter != inputEvent.particles_end(); ++particleIter ) {
    const HepMC::GenParticle* currentParticle = *particleIter;
    if (currentParticle->barcode()>200000) { // FIXME configurable constant
      continue; // skip particles created by the simulation
    }
    std::unique_ptr<HepMC::GenParticle> copyOfGenParticle = std::make_unique<HepMC::GenParticle>(*currentParticle);
    if ( currentParticle == inputEvent.beam_particles().first ) beam1 = copyOfGenParticle.get();
    if ( currentParticle == inputEvent.beam_particles().second ) beam2 = copyOfGenParticle.get();
    // There may (will) be particles which had end vertices added by
    // the simulation (inputEvent). Those vertices will not be copied
    // to the outputEvent, so we should not try to use them here.
    const bool shouldAddProdVertex(currentParticle->production_vertex() && inputEvtVtxToOutputEvtVtx[ currentParticle->production_vertex() ]);
    const bool shouldAddEndVertex(currentParticle->end_vertex() && inputEvtVtxToOutputEvtVtx[ currentParticle->end_vertex() ]);
    if ( shouldAddProdVertex || shouldAddEndVertex ) {
      HepMC::GenParticle* particleCopy = copyOfGenParticle.release();
      if ( shouldAddEndVertex  ) {
        inputEvtVtxToOutputEvtVtx[ currentParticle->end_vertex() ]->
          add_particle_in(particleCopy);
      }
      if ( shouldAddProdVertex ) {
        inputEvtVtxToOutputEvtVtx[ currentParticle->production_vertex() ]->
          add_particle_out(particleCopy);
      }
    }
    else {
      ATH_MSG_WARNING ( "Found GenParticle with no production or end vertex! \n" << *currentParticle);
    }
  }
  outputEvent->set_beam_particles( beam1, beam2 );
  //
  // 4. now that vtx/particles are copied, copy weights and random states
  outputEvent->set_random_states( inputEvent.random_states() );
  outputEvent->weights() = inputEvent.weights();

  //Sanity check
  HepMC::GenEvent::particle_const_iterator partIter = outputEvent->particles_begin();
  const HepMC::GenEvent::particle_const_iterator partEnd = outputEvent->particles_end();
  bool outputProblem(false);
  while (partIter!=partEnd) {
    const HepMC::GenParticle& particle(**partIter);
    if (particle.status()==1) {
      if (!particle.production_vertex()) {
        ATH_MSG_ERROR("Status 1 particle without a production vertex!! " << particle);
        outputProblem = true;
      }
      if (particle.end_vertex()) {
        ATH_MSG_ERROR("Status 1 particle with an end vertex!! " << particle);
        outputProblem = true;
      }
    }
    else if (particle.status()==2) {
      if (!particle.production_vertex()) {
        ATH_MSG_ERROR("Status 2 particle without a production vertex!! " << particle);
        outputProblem = true;
      }
      if (!particle.end_vertex()) {
        ATH_MSG_ERROR("Status 2 particle without an end vertex!! " << particle);
        outputProblem = true;
      }
    }
    ++partIter;
  }
  if (outputProblem) {
    ATH_MSG_FATAL("Problems in output GenEvent - bailing out.");
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<McEventCollection> outputMcEventCollection(m_outputMcEventCollection);
  ATH_CHECK(outputMcEventCollection.record(std::make_unique<McEventCollection>()));
  outputMcEventCollection->push_back(outputEvent.release());
  if (!outputMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output McEventCollection called " << outputMcEventCollection.name() << " in store " << outputMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "succeded TruthResetAlg ..... " );

  return StatusCode::SUCCESS;

}
