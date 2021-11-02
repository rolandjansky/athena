/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthClosureCheck.h"
//
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"

#include "StoreGate/ReadHandle.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
//
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <climits>

TruthClosureCheck::TruthClosureCheck(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("OriginalMcEventCollection" , m_originalMcEventCollection  = "BeamTruthEvent");
  declareProperty("ResetMcEventCollection" , m_resetMcEventCollection  = "NewTruthEvent");
  declareProperty("CompareMomenta", m_compareMomenta);
  declareProperty("PostSimulation", m_postSimulation);
}


//----------------------------------------------------
StatusCode TruthClosureCheck::initialize() {
  //----------------------------------------------------

  ATH_CHECK(m_originalMcEventCollection.initialize());
  ATH_CHECK(m_resetMcEventCollection.initialize());
  return StatusCode::SUCCESS;

}


StatusCode TruthClosureCheck::sanityCheck(const HepMC::GenEvent& event) const {
  //Sanity check
  bool resetProblem(false);
  for (auto particle: event) {
    if (particle->status() == 1) {
      if (!particle->production_vertex()) {
        ATH_MSG_ERROR("Status 1 particle without a production vertex!! " << particle);
        resetProblem = true;
      }
      if (!m_postSimulation && particle->end_vertex()) {
        ATH_MSG_ERROR("Status 1 particle with an end vertex!! " << particle);
        resetProblem = true;
      }
    }
    else if (particle->status() == 2) {
      if (!particle->production_vertex()) {
        ATH_MSG_ERROR("Status 2 particle without a production vertex!! " << particle);
        resetProblem = true;
      }
      if (!particle->end_vertex()) {
        ATH_MSG_ERROR("Status 2 particle without an end vertex!! " << particle);
        resetProblem = true;
      }
    }
  }
  if (resetProblem) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

#ifdef HEPMC3
void TruthClosureCheck::printGenVertex(HepMC::ConstGenVertexPtr origVertex,
                                       HepMC::ConstGenVertexPtr resetVertex) const
{
  ATH_MSG_INFO("----------------------------------");
  ATH_MSG_INFO("Original Vertex:");
  ATH_MSG_INFO( origVertex );
  ATH_MSG_INFO("Particles In:");
  for (auto originalPartIn: origVertex->particles_in()) ATH_MSG_INFO( originalPartIn );
  ATH_MSG_INFO("Particles Out:");
  for (auto originalPartOut: origVertex->particles_out()) ATH_MSG_INFO( originalPartOut );
  ATH_MSG_INFO("----------------------------------");
  ATH_MSG_INFO("Reset Vertex:");
  ATH_MSG_INFO( resetVertex );
  ATH_MSG_INFO("Particles In:");
  for (auto resetPartIn: resetVertex->particles_in()) ATH_MSG_INFO( resetPartIn );
  ATH_MSG_INFO("Particles Out:");
  for (auto resetPartOut: resetVertex->particles_out()) ATH_MSG_INFO( resetPartOut );
  ATH_MSG_INFO("----------------------------------");
  return;
}
#else

void TruthClosureCheck::printGenVertex(const HepMC::GenVertex& origVertex,
                                       const HepMC::GenVertex& resetVertex) const
{
  ATH_MSG_INFO("----------------------------------");
  ATH_MSG_INFO("Original Vertex:");
  ATH_MSG_INFO( origVertex );
  ATH_MSG_INFO("Particles In:");
  HepMC::GenVertex::particles_in_const_iterator originalPartInIter(origVertex.particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfOriginalListOfParticlesIn(origVertex.particles_in_const_end());
  while( originalPartInIter!=endOfOriginalListOfParticlesIn ) {
    ATH_MSG_INFO( **originalPartInIter );
    ++originalPartInIter;
  }
  ATH_MSG_INFO("Particles Out:");
  HepMC::GenVertex::particles_out_const_iterator originalPartOutIter(origVertex.particles_out_const_begin());
  const HepMC::GenVertex::particles_out_const_iterator endOfOriginalListOfParticlesOut(origVertex.particles_out_const_end());
  while( originalPartOutIter!=endOfOriginalListOfParticlesOut) {
    ATH_MSG_INFO( **originalPartOutIter );
    ++originalPartOutIter;
  }
  ATH_MSG_INFO("----------------------------------");
  ATH_MSG_INFO("Reset Vertex:");
  ATH_MSG_INFO( resetVertex );
  ATH_MSG_INFO("Particles In:");
  HepMC::GenVertex::particles_in_const_iterator resetPartInIter(resetVertex.particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfResetListOfParticlesIn(resetVertex.particles_in_const_end());
  while( resetPartInIter!=endOfResetListOfParticlesIn ) {
    ATH_MSG_INFO( **resetPartInIter );
    ++resetPartInIter;
  }
  ATH_MSG_INFO("Particles Out:");
  HepMC::GenVertex::particles_out_const_iterator resetPartOutIter(resetVertex.particles_out_const_begin());
  const HepMC::GenVertex::particles_out_const_iterator endOfResetListOfParticlesOut(resetVertex.particles_out_const_end());
  while( resetPartOutIter!=endOfResetListOfParticlesOut) {
    ATH_MSG_INFO( **resetPartOutIter );
    ++resetPartOutIter;
  }
  ATH_MSG_INFO("----------------------------------");
  return;
}
#endif

#ifdef HEPMC3
StatusCode TruthClosureCheck::compareGenVertex(HepMC::ConstGenVertexPtr origVertex,
                                               HepMC::ConstGenVertexPtr resetVertex) const
{
  if (!origVertex && !resetVertex) return StatusCode::SUCCESS;
  if (!origVertex || !resetVertex) return StatusCode::FAILURE;
  bool pass{true};

  if (HepMC::barcode(origVertex) != HepMC::barcode(resetVertex)) {
    ATH_MSG_ERROR ("vertex barcode differs! Original: "<<HepMC::barcode(origVertex)<<", Reset: "<<HepMC::barcode(resetVertex));
    pass = false;
  }
  if (origVertex->position() != resetVertex->position()) {
    const HepMC::FourVector& oP = origVertex->position();
    const HepMC::FourVector& rP = resetVertex->position();
    ATH_MSG_ERROR("vertex position differs! Original: ("<<oP.x()<<","<<oP.y()<<","<<oP.z()<<","<<oP.t()<<"), Reset: ("<<rP.x()<<","<<rP.y()<<","<<rP.z()<<","<<rP.t()<<")");
    pass = false;
  }
  if (origVertex->particles_in().size() != resetVertex->particles_in().size() ) {
    ATH_MSG_ERROR ("particles_in_size differs! Original: "<<origVertex->particles_in().size()<<", Reset: "<<resetVertex->particles_in().size());
    pass = false;
  }
  if (origVertex->particles_out().size() != resetVertex->particles_out().size() ) {
    ATH_MSG_ERROR ("particles_out_size differs! Original: "<<origVertex->particles_out().size()<<", Reset: "<<resetVertex->particles_out().size());
    pass = false;
  }
  std::vector<HepMC::ConstGenParticlePtr> OriginalListOfParticlesIn = origVertex->particles_in();
  std::vector<HepMC::ConstGenParticlePtr> ResetListOfParticlesIn = resetVertex->particles_in();
  for ( std::vector<HepMC::ConstGenParticlePtr>::iterator originalPartInIter(OriginalListOfParticlesIn.begin()),resetPartInIter(ResetListOfParticlesIn.begin()); 
        originalPartInIter != OriginalListOfParticlesIn.end() && resetPartInIter != ResetListOfParticlesIn.end(); 
        ++originalPartInIter, ++resetPartInIter
       ) {
    if (compareGenParticle(*originalPartInIter,*resetPartInIter).isFailure()) {
      ATH_MSG_ERROR ( "input particle properties differ!" );
      pass &= false;
    }
    ATH_MSG_VERBOSE ( "particles match!" );
  }


  std::vector<HepMC::ConstGenParticlePtr> OriginalListOfParticlesOut = origVertex->particles_out();
  std::vector<HepMC::ConstGenParticlePtr> ResetListOfParticlesOut = resetVertex->particles_out();
  std::sort(OriginalListOfParticlesOut.begin(), OriginalListOfParticlesOut.end(), [](auto& a, auto& b) -> bool{return HepMC::barcode(a) > HepMC::barcode(b); });
  std::sort(ResetListOfParticlesOut.begin(), ResetListOfParticlesOut.end(), [](auto& a, auto& b) -> bool{return HepMC::barcode(a) > HepMC::barcode(b); });

  for ( std::vector<HepMC::ConstGenParticlePtr>::iterator originalPartOutIter(OriginalListOfParticlesOut.begin()), resetPartOutIter(ResetListOfParticlesOut.begin()); 
        originalPartOutIter != OriginalListOfParticlesOut.end() && resetPartOutIter != ResetListOfParticlesOut.end(); 
        ++originalPartOutIter, ++resetPartOutIter
       ) {
    if (compareGenParticle(*originalPartOutIter,*resetPartOutIter).isFailure()) {
      ATH_MSG_ERROR ( "output particle properties differ!" );
      pass &= false;
    }
    ATH_MSG_VERBOSE ( "particles match!" );
  }
  if(!pass) { return StatusCode::FAILURE; }
  return StatusCode::SUCCESS;
}


#else
StatusCode TruthClosureCheck::compareGenVertex(const HepMC::GenVertex& origVertex,
                                               const HepMC::GenVertex& resetVertex) const
{
  bool pass{true};

  if (origVertex.barcode() != resetVertex.barcode()) {
    ATH_MSG_ERROR ("vertex barcode differs! Original: "<<origVertex.barcode()<<", Reset: "<<resetVertex.barcode());
    pass = false;
  }
  if (origVertex.position() != resetVertex.position()) {
    const HepMC::FourVector& oP = origVertex.position();
    const HepMC::FourVector& rP = resetVertex.position();
    ATH_MSG_ERROR("vertex position differs! Original: ("<<oP.x()<<","<<oP.y()<<","<<oP.z()<<","<<oP.t()<<"), Reset: ("<<rP.x()<<","<<rP.y()<<","<<rP.z()<<","<<rP.t()<<")");
    pass = false;
  }
  if (origVertex.particles_in_size() != resetVertex.particles_in_size() ) {
    ATH_MSG_ERROR ("particles_in_size differs! Original: "<<origVertex.particles_in_size()<<", Reset: "<<resetVertex.particles_in_size());
    pass = false;
  }
  if (origVertex.particles_out_size() != resetVertex.particles_out_size() ) {
    ATH_MSG_ERROR ("particles_out_size differs! Original: "<<origVertex.particles_out_size()<<", Reset: "<<resetVertex.particles_out_size());
    pass = false;
  }
  HepMC::GenVertex::particles_in_const_iterator originalPartInIter(origVertex.particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfOriginalListOfParticlesIn(origVertex.particles_in_const_end());
  HepMC::GenVertex::particles_in_const_iterator resetPartInIter(resetVertex.particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfResetListOfParticlesIn(resetVertex.particles_in_const_end());
  while( originalPartInIter!=endOfOriginalListOfParticlesIn &&
         resetPartInIter!=endOfResetListOfParticlesIn ) {
    if (compareGenParticle(**originalPartInIter,**resetPartInIter).isFailure()) {
      ATH_MSG_ERROR ( "input particle properties differ!" );
      pass &= false;
    }
    ATH_MSG_VERBOSE ( "particles match!" );
    ++resetPartInIter;
    ++originalPartInIter;
  }

  HepMC::GenVertex::particles_out_const_iterator originalPartOutIter(origVertex.particles_out_const_begin());
  const HepMC::GenVertex::particles_out_const_iterator endOfOriginalListOfParticlesOut(origVertex.particles_out_const_end());
  // ordering of particles may differ in each case - sigh..
  const HepMC::GenVertex::particles_out_const_iterator endOfResetListOfParticlesOut(resetVertex.particles_out_const_end());
  while( originalPartOutIter!=endOfOriginalListOfParticlesOut) {
    const int barcodeOrig{(*originalPartOutIter)->barcode()};
    HepMC::GenVertex::particles_out_const_iterator resetPartOutIter(resetVertex.particles_out_const_begin());
    HepMC::GenVertex::particles_in_const_iterator matchingResetParticleIter{endOfResetListOfParticlesOut};
    while(resetPartOutIter!=endOfResetListOfParticlesOut) {
      if ( barcodeOrig == (*resetPartOutIter)->barcode() ) {
        matchingResetParticleIter = resetPartOutIter;
        break;
      }
      ++resetPartOutIter;
    }
    if (matchingResetParticleIter==endOfResetListOfParticlesOut ||
        compareGenParticle(**originalPartOutIter,**matchingResetParticleIter).isFailure()) {
      ATH_MSG_ERROR ( "output particle properties differ!" );
      pass &= false;
    }
    ATH_MSG_VERBOSE ( "particles match!" );
    ++originalPartOutIter;
  }

  if(!pass) { return StatusCode::FAILURE; }
  return StatusCode::SUCCESS;
}
#endif


StatusCode TruthClosureCheck::compareMomenta(const HepMC::FourVector& origMomenta,
                                             const HepMC::FourVector& resetMomenta) const
{
  bool pass{true};
  if (m_compareMomenta) {
    if (m_momentaLimit<std::abs(origMomenta.px()-resetMomenta.px())) {
      pass &= false;
    }
    if (m_momentaLimit<std::abs(origMomenta.py()-resetMomenta.py())) {
      pass &= false;
    }
    if (m_momentaLimit<std::abs(origMomenta.pz()-resetMomenta.pz())) {
      pass &= false;
    }
    if (m_momentaLimit<std::abs(origMomenta.e()-resetMomenta.e())) {
      pass &= false;
    }
    if(origMomenta != resetMomenta) {
      ATH_MSG_ERROR ("Exact momenta agreement check failed.");
      pass &= false;
    }
  }
  if(!pass) { return StatusCode::FAILURE; }
  return StatusCode::SUCCESS;
}

#ifdef HEPMC3
StatusCode TruthClosureCheck::compareGenParticle(HepMC::ConstGenParticlePtr origParticle,
                                                 HepMC::ConstGenParticlePtr resetParticle) const
{
  if (!origParticle && !resetParticle) return StatusCode::SUCCESS;
  if (!origParticle || !resetParticle) return StatusCode::FAILURE;
  
  bool pass{true};
  if (HepMC::barcode(origParticle) != HepMC::barcode(resetParticle)) {
    ATH_MSG_ERROR ("particle barcode differs! Original: "<<HepMC::barcode(origParticle)<<", Reset: "<<HepMC::barcode(resetParticle));
    pass &= false;
  }
  if (origParticle->status() != resetParticle->status()) {
    ATH_MSG_ERROR ("particle status differs! Original: "<<origParticle->status()<<", Reset: "<<resetParticle->status());
    pass &= false;
  }
  if (origParticle->pdg_id() != resetParticle->pdg_id()) {
    ATH_MSG_ERROR ("particle pdg_id differs! Original: "<<origParticle->pdg_id()<<", Reset: "<<resetParticle->pdg_id());
    pass &= false;
  }
  if (compareMomenta(origParticle->momentum(), resetParticle->momentum()).isFailure()) {
    const HepMC::FourVector& oM = origParticle->momentum();
    const HepMC::FourVector& rM = resetParticle->momentum();
    ATH_MSG_DEBUG("particle momentum differs! Original: ("<<oM.x()<<","<<oM.y()<<","<<oM.z()<<","<<oM.t()<<"), Reset: ("<<rM.x()<<","<<rM.y()<<","<<rM.z()<<","<<rM.t()<<")");
    pass &= false;
  }
  if(!pass) { return StatusCode::FAILURE; }
  return StatusCode::SUCCESS;
}

#else
StatusCode TruthClosureCheck::compareGenParticle(const HepMC::GenParticle& origParticle,
                                                 const HepMC::GenParticle& resetParticle) const
{
  bool pass{true};
  if (origParticle.barcode() != resetParticle.barcode()) {
    ATH_MSG_ERROR ("particle barcode differs! Original: "<<origParticle.barcode()<<", Reset: "<<resetParticle.barcode());
    pass &= false;
  }
  if (origParticle.status() != resetParticle.status()) {
    ATH_MSG_ERROR ("particle status differs! Original: "<<origParticle.status()<<", Reset: "<<resetParticle.status());
    pass &= false;
  }
  if (origParticle.pdg_id() != resetParticle.pdg_id()) {
    ATH_MSG_ERROR ("particle pdg_id differs! Original: "<<origParticle.pdg_id()<<", Reset: "<<resetParticle.pdg_id());
    pass &= false;
  }
  if (compareMomenta(origParticle.momentum(), resetParticle.momentum()).isFailure()) {
    const HepMC::FourVector& oM = origParticle.momentum();
    const HepMC::FourVector& rM = resetParticle.momentum();
    ATH_MSG_DEBUG("particle momentum differs! Original: ("<<oM.x()<<","<<oM.y()<<","<<oM.z()<<","<<oM.t()<<"), Reset: ("<<rM.x()<<","<<rM.y()<<","<<rM.z()<<","<<rM.t()<<")");
    pass &= false;
  }
  if(!pass) { return StatusCode::FAILURE; }
  return StatusCode::SUCCESS;
}
#endif

#ifdef HEPMC3
//-------------------------------------------------
StatusCode TruthClosureCheck::execute() {
//-------------------------------------------------

  ATH_MSG_DEBUG( " execute..... " );
  SG::ReadHandle<McEventCollection> originalMcEventCollection(m_originalMcEventCollection);
  if (!originalMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find original McEventCollection called " << originalMcEventCollection.name() << " in store " << originalMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  const HepMC::GenEvent& originalEvent(**(originalMcEventCollection->begin()));

  if (sanityCheck(originalEvent).isFailure()) {
    ATH_MSG_FATAL("Problems in original GenEvent - bailing out.");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<McEventCollection> resetMcEventCollection(m_resetMcEventCollection);
  if (!resetMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find reset McEventCollection called " << resetMcEventCollection.name() << " in store " << resetMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  const HepMC::GenEvent& resetEvent(**(resetMcEventCollection->begin()));

  if (sanityCheck(resetEvent).isFailure()) {
    ATH_MSG_FATAL("Problems in reset GenEvent - bailing out.");
    return StatusCode::FAILURE;
  }

  if (originalEvent.event_number() != resetEvent.event_number() ) {
    ATH_MSG_ERROR ("event_number differs! Original: "<<originalEvent.event_number()<<", Reset: "<<resetEvent.event_number());
  }

  if (HepMC::signal_process_id(originalEvent) != HepMC::signal_process_id(resetEvent) ) {
    ATH_MSG_ERROR ("signal_process_id differs! Original: "<<HepMC::signal_process_id(originalEvent)<<", Reset: "<<HepMC::signal_process_id(resetEvent));
  }
///->
  std::vector<HepMC::ConstGenParticlePtr> OriginalBeams = originalEvent.beams();
  std::vector<HepMC::ConstGenParticlePtr> ResetBeams = resetEvent.beams();

  for ( std::vector<HepMC::ConstGenParticlePtr>::iterator originalBeamIter(OriginalBeams.begin()), resetBeamIter(ResetBeams.begin()); 
        originalBeamIter != OriginalBeams.end() && resetBeamIter != ResetBeams.end(); 
        ++originalBeamIter, ++resetBeamIter
       ) {
    if (compareGenParticle(*originalBeamIter,*resetBeamIter).isFailure()) {
      ATH_MSG_ERROR ( "Beam particle properties differ!" );
    }
///<=
    if (originalEvent.particles().size() != resetEvent.particles().size() ) {
      ATH_MSG_ERROR ("particles_size differs! Original: "<<originalEvent.particles().size()<<", Reset: "<<resetEvent.particles().size());
    }

    if (originalEvent.vertices().size() != resetEvent.vertices().size() ) {
      ATH_MSG_ERROR ("vertices_size differs! Original: "<<originalEvent.vertices().size()<<", Reset: "<<resetEvent.vertices().size());
    }
  }

  //loop over vertices in Background GenEvent
  std::vector<HepMC::ConstGenVertexPtr> OriginalListOfVertices = originalEvent.vertices();
  std::vector<HepMC::ConstGenVertexPtr> ResetListOfVertices = resetEvent.vertices();

  for( std::vector<HepMC::ConstGenVertexPtr>::iterator origVertexIter(OriginalListOfVertices.begin()),resetVertexIter(ResetListOfVertices.begin()); 
       origVertexIter != OriginalListOfVertices.end() && resetVertexIter != ResetListOfVertices.end(); 
       ++origVertexIter, ++resetVertexIter
       ) {
    if (compareGenVertex(*origVertexIter,*resetVertexIter).isFailure()) {
      ATH_MSG_ERROR ( "vertices differ!" );
      printGenVertex(*origVertexIter,*resetVertexIter);
    }
  }
  ATH_MSG_INFO( "Completed Truth reset closure check ..... " );

  return StatusCode::SUCCESS;

}

#else


//-------------------------------------------------
StatusCode TruthClosureCheck::execute() {
  //-------------------------------------------------

  ATH_MSG_DEBUG( " execute..... " );
  SG::ReadHandle<McEventCollection> originalMcEventCollection(m_originalMcEventCollection);
  if (!originalMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find original McEventCollection called " << originalMcEventCollection.name() << " in store " << originalMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  const HepMC::GenEvent& originalEvent(**(originalMcEventCollection->begin()));

  if (sanityCheck(originalEvent).isFailure()) {
    ATH_MSG_FATAL("Problems in original GenEvent - bailing out.");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<McEventCollection> resetMcEventCollection(m_resetMcEventCollection);
  if (!resetMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find reset McEventCollection called " << resetMcEventCollection.name() << " in store " << resetMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  const HepMC::GenEvent& resetEvent(**(resetMcEventCollection->begin()));

  if (sanityCheck(resetEvent).isFailure()) {
    ATH_MSG_FATAL("Problems in reset GenEvent - bailing out.");
    return StatusCode::FAILURE;
  }

  if (originalEvent.event_number() != resetEvent.event_number() ) {
    ATH_MSG_ERROR ("event_number differs! Original: "<<originalEvent.event_number()<<", Reset: "<<resetEvent.event_number());
  }

  if (originalEvent.signal_process_id() != resetEvent.signal_process_id() ) {
    ATH_MSG_ERROR ("signal_process_id differs! Original: "<<originalEvent.signal_process_id()<<", Reset: "<<resetEvent.signal_process_id());
  }

  if (originalEvent.valid_beam_particles() != resetEvent.valid_beam_particles() ) {
    ATH_MSG_ERROR ("valid_beam_particles differs! Original: "<<originalEvent.valid_beam_particles()<<", Reset: "<<resetEvent.valid_beam_particles());
  }
  else if (originalEvent.valid_beam_particles() && resetEvent.valid_beam_particles()) {
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> originalBP = originalEvent.beam_particles();
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> resetBP = resetEvent.beam_particles();
    if ( ( !originalBP.first && resetBP.first ) ||
         ( originalBP.first && !resetBP.first ) ||
         ( originalBP.first && resetBP.first && compareGenParticle(*(originalBP.first), *(resetBP.first)).isFailure() ) ) {
      ATH_MSG_ERROR ( "First beam particle does not match" );
    }
    if ( ( !originalBP.second && resetBP.second ) ||
         ( originalBP.second && !resetBP.second ) ||
         ( originalBP.second && resetBP.second && compareGenParticle(*(originalBP.second), *(resetBP.second)).isFailure() ) ) {
      ATH_MSG_ERROR ( "Second beam particle does not match" );
    }

    if (originalEvent.particles_size() != resetEvent.particles_size() ) {
      ATH_MSG_ERROR ("particles_size differs! Original: "<<originalEvent.particles_size()<<", Reset: "<<resetEvent.particles_size());
    }

    if (originalEvent.vertices_size() != resetEvent.vertices_size() ) {
      ATH_MSG_ERROR ("vertices_size differs! Original: "<<originalEvent.vertices_size()<<", Reset: "<<resetEvent.vertices_size());
    }
  }

  //loop over vertices in Background GenEvent
  HepMC::GenEvent::vertex_const_iterator origVertexIter(originalEvent.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfOriginalListOfVertices(originalEvent.vertices_end());
  HepMC::GenEvent::vertex_const_iterator resetVertexIter(resetEvent.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfResetListOfVertices(resetEvent.vertices_end());
  while( origVertexIter!=endOfOriginalListOfVertices &&
         resetVertexIter!=endOfResetListOfVertices) {
    if (compareGenVertex(**origVertexIter,**resetVertexIter).isFailure()) {
      ATH_MSG_ERROR ( "vertices differ!" );
      printGenVertex(**origVertexIter,**resetVertexIter);
    }
    ++origVertexIter;
    ++resetVertexIter;
  }
  ATH_MSG_INFO( "Completed Truth reset closure check ..... " );

  return StatusCode::SUCCESS;

}
#endif
