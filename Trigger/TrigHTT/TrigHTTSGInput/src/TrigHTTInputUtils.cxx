/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTSGInput/TrigHTTInputUtils.h"
#include "HepMC/GenVertex.h"

namespace TrigHTTInputUtils {  

  const ParentBitmask  construct_truth_bitmap( const HepMC::GenParticle* particle ) 
  {
    ParentBitmask result;
    result.reset();
    typedef std::pair<const HepMC::GenParticle*,unsigned int> Parent;
    std::vector<Parent> parents;
    parents.push_back( Parent(particle,0) );
    while( !parents.empty() ) {
      const HepMC::GenParticle* p = parents.back().first;
      const unsigned int level = parents.back().second;
      if( std::abs(p->pdg_id())==15 ) { result.set( TAU_PARENT_BIT , 1 ); }
      if( std::abs(p->pdg_id())==5 ) { result.set( B_PARENT_BIT , 1 ); }
      if( std::abs(p->pdg_id())==211 ) { result.set( PION_PARENT_BIT , 1 ); }
      if( std::abs(p->pdg_id())==211 && level<=1 ) { result.set( PION_IMMEDIATE_PARENT_BIT , 1 ); }
      if( result.count()==NBITS ) { break; }
      parents.pop_back();
      if( !(p->production_vertex()) ) { continue; }
      for( HepMC::GenVertex::particle_iterator i=p->production_vertex()->particles_begin(HepMC::parents), f=p->production_vertex()->particles_end(HepMC::parents); i!=f; ++i ) {
	parents.push_back( Parent(*i,level+1) );
      }
    }
    return result;
  }

  
}
