/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PhotonTruthParticleFilterTool.cxx
 * @author Mike Hance <mike.hance@cern.ch>
 * @date August 2010
 * @brief Filter truth particles for writing to D3PD.
 */


#include "PhotonTruthParticleFilterTool.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "boost/foreach.hpp"
#include <utility>


namespace {


const int PARTONPDGMAX = 43;
const int NPPDGMIN = 1000000;
const int NPPDGMAX = 8999999;
const int PHOTOSMIN = 10000;
const int GEANTMIN = 200000;


} // anonymous namespace


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PhotonTruthParticleFilterTool::PhotonTruthParticleFilterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : AthAlgTool (type, name, parent),
      m_resolver (name, evtStore(), m_mcEventsName),
      m_firstHadronBarcode(0)
{
  declareProperty( "McEvents",       
		   m_mcEventsName = "GEN_AOD",
		   "Name of the input McEventCollection we want to filter" );

  declareProperty( "McEventsOutput", 
		   m_mcEventsOutputName = "GEN_D3PD",
		   "Name of the output McEventCollection which has been filtered" );

  declareProperty( "DoPileup",
                   m_doPileup = false,
                   "If true, include particles from pileup/cavern.");

  declareProperty ("RemoveEmpty",
                   m_removeEmpty = true,
                   "If true, remove empty GenEvent structures.");

  declareProperty ("DoEtIsolations",
                   m_doEtIsolations = false,
                   "Unused, but required by configuration script.");

  declareProperty ("WritePartons",
                   m_writePartons = true,
                   "Keep partons?");

  declareProperty ("WriteHadrons",
                   m_writeHadrons = false,
                   "Keep hadrons?");

  declareProperty ("WriteGeant",
                   m_writeGeant = false,
                   "Keep geant particles?");

  declareProperty ("ParticleFilters",
		   m_particleFilters,
		   "Filters to keep certain types of particles");

  m_haveSeenAHadron = false;
}


/**
 * @brief Standard Gaudi @c queryInterface method.
 */
StatusCode
PhotonTruthParticleFilterTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITruthParticleFilterTool::interfaceID() )  {
    *ppvIf = static_cast<ITruthParticleFilterTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }

  return AlgTool::queryInterface( riid, ppvIf );
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode PhotonTruthParticleFilterTool::initialize()
{
  CHECK( m_resolver.initialize<McEventCollection>() );
  return AthAlgTool::initialize();
}


/**
 * @brief Standard Gaudi finalize method.
 */
StatusCode PhotonTruthParticleFilterTool::finalize()
{
  return AthAlgTool::finalize();
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode PhotonTruthParticleFilterTool::execute()
{
  m_haveSeenAHadron = false;
  m_firstHadronBarcode = 0;

  // Fetch input collection.
  const McEventCollection* mc_in;
  CHECK( evtStore()->retrieve (mc_in, m_resolver.key()) );

  // Create output collection.
  McEventCollection* mc_out = new McEventCollection;
  CHECK( evtStore()->record (mc_out, m_mcEventsOutputName) );
  CHECK( evtStore()->setConst (mc_out) );

  // Copy and filter.
  CHECK( buildMcAod (mc_in, mc_out) );
  return StatusCode::SUCCESS;
}


/**
 *  This method will check the validity of the input McEventCollection 
 *  and build a filtered one from the strategy implemented by the 
 *  concrete tool.
 *  It is not const to allow derived tools to build statistics during
 *  the filtering process.
 */
StatusCode
PhotonTruthParticleFilterTool::buildMcAod (const McEventCollection* mc_in,
                                     McEventCollection* mc_out)
{
  // Loop over GenEvent's.
  mc_out->reserve (mc_in->size());
  BOOST_FOREACH (const HepMC::GenEvent* ev_in, *mc_in) {
    if (!ev_in) continue;

    // Copy the GenEvent.
    HepMC::GenEvent* ev_out = new HepMC::GenEvent (ev_in->signal_process_id(),
                                                   ev_in->event_number());
    ev_out->set_event_scale (ev_in->event_scale());
    ev_out->set_alphaQCD (ev_in->alphaQCD());
    ev_out->set_alphaQED (ev_in->alphaQED());
    ev_out->weights() = ev_in->weights();
    ev_out->set_random_states (ev_in->random_states());
    if (ev_in->heavy_ion())
      ev_out->set_heavy_ion (*ev_in->heavy_ion());
    if (ev_in->pdf_info())
      ev_out->set_pdf_info (*ev_in->pdf_info());

    // Copy and filter the contents.
    CHECK( filterEvent (ev_in, ev_out) );

    // Maybe throw out empty GenEvent's.
    if (m_removeEmpty && ev_out->particles_empty())
      delete ev_out;
    else
      mc_out->push_back (ev_out);

    // If we don't want pileup, only do the first GenEvent.
    if (!m_doPileup)
      break;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Filter a single @c GenEvent.
 */
StatusCode
PhotonTruthParticleFilterTool::filterEvent (const HepMC::GenEvent* ev_in,
                                      HepMC::GenEvent* ev_out)
{
  // Loop over particles.
  // (FOREACH doesn't work here because particle_const_iterator
  // isn't consistent in the use of const...)
  for (HepMC::GenEvent::particle_const_iterator ip = ev_in->particles_begin();
       ip != ev_in->particles_end();
       ++ip)
  {
    // Copy the particle if we want to keep it.
    if (acceptParticle (*ip))
      CHECK( addParticle (*ip, ev_out) );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Add a @c GenParticle (and its production vertex) to a @c GenEvent.
 */
StatusCode
PhotonTruthParticleFilterTool::addParticle (const HepMC::GenParticle* p,
                                      HepMC::GenEvent* ev)
{
  // Add parent vertex if it exists.  Otherwise, add decay vertex.
  if (p->production_vertex())
    CHECK( addVertex (p->production_vertex(), ev) );
  else if (p->end_vertex())
    CHECK( addVertex (p->end_vertex(), ev) );
  else {
    REPORT_MESSAGE (MSG::ERROR) << "Encountered GenParticle with no vertices!";
    return StatusCode::FAILURE;
  }

  // Find the particle in the event.
  // If it doesn't exist yet, copy it.
  HepMC::GenParticle* pnew = ev->barcode_to_particle (p->barcode());
  if (!pnew)
    pnew = new HepMC::GenParticle (*p);

  // Add ourself to our vertices.
  if (p->production_vertex()) {
    HepMC::GenVertex* v =
      ev->barcode_to_vertex (p->production_vertex()->barcode());
    if (v)
      v->add_particle_out (pnew);
  }

  if (p->end_vertex()) {
    HepMC::GenVertex* v =
      ev->barcode_to_vertex (p->end_vertex()->barcode());
    if (v)
      v->add_particle_in (pnew);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Add a @c GenVertex to a @c GenEvent.
 */
StatusCode
PhotonTruthParticleFilterTool::addVertex (const HepMC::GenVertex* v,
                                    HepMC::GenEvent* ev)
{
  // See if this vertex has already been copied.
  HepMC::GenVertex* vnew = ev->barcode_to_vertex (v->barcode());
  if (!vnew) {
    // No ... make a new one.
    vnew = new HepMC::GenVertex;
    vnew->set_position (v->position());
    vnew->set_id (v->id());
    vnew->suggest_barcode (v->barcode());
    vnew->weights() = v->weights();
    ev->add_vertex (vnew);

    // Fill in the existing relations of the new vertex.
    BOOST_FOREACH(const HepMC::GenParticle* p,
                  std::make_pair (v->particles_in_const_begin(),
                                  v->particles_in_const_end()))
    {
      HepMC::GenParticle* pnew = ev->barcode_to_particle (p->barcode());
      if (pnew)
        vnew->add_particle_in (pnew);
    }

    BOOST_FOREACH(const HepMC::GenParticle* p,
                  std::make_pair (v->particles_out_const_begin(),
                                  v->particles_out_const_end()))
    {
      HepMC::GenParticle* pnew = ev->barcode_to_particle (p->barcode());
      if (pnew)
        vnew->add_particle_out (pnew);
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Test to see if we want to keep a particle.
 */
bool
PhotonTruthParticleFilterTool::acceptParticle (const HepMC::GenParticle* p)
{
  bool ok = false;

  int pdg_id = std::abs (p->pdg_id());
  int status = p->status();
  int barcode = p->barcode();

  if (p->barcode() > GEANTMIN && !m_writeGeant)
    return false;
  
  // are we at parton/hadron level?
  if ( status!=3 && pdg_id > PARTONPDGMAX && !m_haveSeenAHadron ) {
    m_haveSeenAHadron = true;
    m_firstHadronBarcode = barcode;
  }

  // OK if we select partons and are at beginning of event record
  if( m_writePartons /*&& !m_haveSeenAHadron */ &&
      (pdg_id <= PARTONPDGMAX || (pdg_id >= NPPDGMIN && pdg_id <= NPPDGMAX) ))
    ok = true;

  //  OK if we should select hadrons and are in hadron range 
  if( m_writeHadrons && m_haveSeenAHadron && barcode < PHOTOSMIN )
    ok = true;
 
  // PHOTOS range: check whether photons come from parton range or 
  // hadron range
  int motherBarcode = 999999999;
  if( barcode > PHOTOSMIN && barcode < GEANTMIN &&
      p->production_vertex() ) {
    const HepMC::GenVertex* vprod = p->production_vertex();
    if (vprod->particles_in_size() > 0) {
      const HepMC::GenParticle* mother = *vprod->particles_in_const_begin();
      if (mother) 
        motherBarcode = mother->barcode();
    }
    if( m_writePartons && motherBarcode < m_firstHadronBarcode )
      ok = true;
    if( m_writeHadrons && motherBarcode >= m_firstHadronBarcode )
      ok = true;
  }

  // OK if we should select G4 particles and are in G4 range
  if( m_writeGeant && barcode > GEANTMIN )
    ok = true;

  using namespace std;

  if(ok) return ok;


  // now, if we're still not sure if we should keep this particle, go through some checks
  ToolHandleArray<ITruthParticleFilterCutTool>::iterator cut_iter   ;
  ToolHandleArray<ITruthParticleFilterCutTool>::iterator cut_iter_E = m_particleFilters.end();
  for(cut_iter = m_particleFilters.begin(); cut_iter != cut_iter_E; ++cut_iter){
    if((*cut_iter)->passesCut(p)){
      ok = true;
      break;
    }
  }
  
  return ok;
}


} // namespace D3PD
