/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleSimWhiteList.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenParticleSimWhiteList.h"

// HepMC includes
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// Helper function
#include "TruthUtils/PIDHelpers.h"

// For finding that file
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <cstdlib>

/** Constructor **/
ISF::GenParticleSimWhiteList::GenParticleSimWhiteList( const std::string& t,
                                                       const std::string& n,
                                                       const IInterface* p )
  : base_class(t,n,p)
{
    // different options
    declareProperty("WhiteList", m_whiteList="G4particle_whitelist.txt");
    declareProperty("QuasiStableSim", m_qs=true);
}

// Athena algtool's Hooks
StatusCode  ISF::GenParticleSimWhiteList::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    // Initialize the list
    m_pdgId.clear();

    // Get the appropriate file handle
    std::string resolvedFilename = PathResolver::find_file( m_whiteList , "DATAPATH" );
    std::ifstream white_list;
    white_list.open( resolvedFilename );
    if (!white_list.is_open()){
      ATH_MSG_ERROR("Could not find white list " << m_whiteList);
      return StatusCode::FAILURE;
    }

    // Parse the list into the vector
    std::string a_line;
    char * pEnd;
    while (!white_list.eof()){
      getline( white_list , a_line );
      long int pdg = strtol( a_line.c_str() , &pEnd , 10 );
      m_pdgId.push_back(pdg);
    }

    // Sort the list for use later
    std::sort( m_pdgId.begin() , m_pdgId.end() );

    // All done!
    return StatusCode::SUCCESS;
}

/** passes through to the private version of the filter */
bool ISF::GenParticleSimWhiteList::pass(const HepMC::GenParticle& particle) const
{
  static std::vector<int> vertices(500);
  vertices.clear();
  bool so_far_so_good = pass( particle , vertices );

  // Test all parent particles
  if (so_far_so_good && particle.production_vertex() && m_qs){
    for (HepMC::GenVertex::particle_iterator it = particle.production_vertex()->particles_begin(HepMC::parents);
                                             it != particle.production_vertex()->particles_end(HepMC::parents); ++it){
      // Loop breaker
      if ( (*it)->barcode() == particle.barcode() ) continue;
      // Check this particle
      vertices.clear();
      bool parent_all_clear = pass( **it , vertices );
      so_far_so_good = so_far_so_good && !parent_all_clear;
    } // Loop over parents
  } // particle had parents

  return so_far_so_good;
}

/** returns true if the the particle and all daughters are on the white list */
bool ISF::GenParticleSimWhiteList::pass(const HepMC::GenParticle& particle , std::vector<int> & used_vertices ) const
{
  // See if the particle is in the white list
  bool passFilter = std::binary_search( m_pdgId.begin() , m_pdgId.end() , particle.pdg_id() ) || MC::PID::isNucleus( particle.pdg_id() );
  // Remove documentation particles
  passFilter = passFilter && particle.status()!=3;

  // Test all daughter particles
  if (particle.end_vertex() && m_qs && passFilter){
    // Break loops
    if ( std::find( used_vertices.begin() , used_vertices.end() , particle.end_vertex()->barcode() )==used_vertices.end() ){
      used_vertices.push_back( particle.end_vertex()->barcode() );
      for (HepMC::GenVertex::particle_iterator it = particle.end_vertex()->particles_begin(HepMC::children);
                                               it != particle.end_vertex()->particles_end(HepMC::children); ++it){
        passFilter = passFilter && pass( **it , used_vertices );
        if (!passFilter) break;
      } // Loop over daughters
    } // Break loops
  } // particle had daughters
  else if (!particle.end_vertex() && !passFilter && particle.status()!=3) { // no daughters... No end vertex... Check if this isn't trouble
    ATH_MSG_ERROR( "Found a particle with no end vertex that does not appear in the white list." );
    ATH_MSG_ERROR( "This is VERY likely pointing to a problem with either the configuration you ");
    ATH_MSG_ERROR( "are using, or a bug in the generator.  Either way it should be fixed.  The");
    ATH_MSG_ERROR( "particle will come next, and then we will throw.");
    ATH_MSG_ERROR( particle );
    throw; 
  }

  return passFilter;
}

StatusCode  ISF::GenParticleSimWhiteList::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

