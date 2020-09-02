/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventGenParticleAssociationTool.cxx 452268 2011-08-04 20:27:55Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenEventGenParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Associate from a @c GenEvent to its contained particles.
 */


#include "GenEventGenParticleAssociationTool.h"
#include "AtlasHepMC/GenParticle.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GenEventGenParticleAssociationTool::GenEventGenParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_haveSeenAHadron(false),
      m_firstHadronBarcode(0)
{
  declareProperty ("WritePartons",     m_doPartons  = false);
  declareProperty ("WriteHadrons",     m_doHadrons  = false);
  declareProperty ("WriteGeant",       m_doGeant    = false);
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode
GenEventGenParticleAssociationTool::reset (const HepMC::GenEvent& p)
{
  m_it = p.particles_begin();
  m_end = p.particles_end();

  m_haveSeenAHadron = false;

  m_firstHadronBarcode = 0;

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const HepMC::GenParticle* GenEventGenParticleAssociationTool::next()
{
  if (m_it == m_end)
    return 0;
  
  const HepMC::GenParticle* out;

  // loop until we see what we want
  bool ok = false;
  while( !ok ) {

    int pdg_id = std::abs ((*m_it)->pdg_id());
    int status = (*m_it)->status();
    int barcode = (*m_it)->barcode();
    
    // are we at parton/hadron level?
    if ( status!=3 && pdg_id > PARTONPDGMAX && 
	 !m_haveSeenAHadron ) {
      m_haveSeenAHadron = true;
      m_firstHadronBarcode = barcode;
    }

    // OK if we select partons and are at beginning of event record
    if( m_doPartons && !m_haveSeenAHadron )
      ok = true;

    //  OK if we should select hadrons and are in hadron range 
    if( m_doHadrons && m_haveSeenAHadron && barcode < PHOTOSMIN )
      ok = true;
 
    // PHOTOS range: check whether photons come from parton range or 
    // hadron range
    int motherBarcode = 999999999;
    if( barcode > PHOTOSMIN && barcode < GEANTMIN &&
	(*m_it)->production_vertex() ) {
      const HepMC::GenVertex* vprod = (*m_it)->production_vertex();
      if (vprod->particles_in_size() > 0) {
	const HepMC::GenParticle* mother = *vprod->particles_in_const_begin();
	if (mother) 
	  motherBarcode = mother->barcode();
      }
      if( m_doPartons && motherBarcode < m_firstHadronBarcode )
	ok = true;
      if( m_doHadrons && motherBarcode >= m_firstHadronBarcode )
	ok = true;
    }

    // OK if we should select G4 particles and are in G4 range
    if( m_doGeant && barcode > GEANTMIN )
      ok = true;

    out = *m_it;
    ++m_it;
    
    if (m_it == m_end)
      return 0;
  
  }
  
  // exit if we are at geant level and not supposed to write this out
  if( out->barcode() > GEANTMIN && !m_doGeant )
    return 0;
  
  return out;
}


} // namespace D3PD
