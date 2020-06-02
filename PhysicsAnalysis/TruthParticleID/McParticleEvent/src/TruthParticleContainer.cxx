/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticleContainer.cxx 
// Implementation file for class TruthParticleContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <iostream>

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

#include "GeneratorObjects/HepMcParticleLink.h"
/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////

TruthParticleContainer::TruthParticleContainer() :
  DataVector<TruthParticle>( SG::OWN_ELEMENTS ),
  m_genEvent    ( ),
  m_etIsolations( ),
  m_particles   ( )
{}

/*
TruthParticleContainer::TruthParticleContainer( const TruthParticleContainer& rhs ) :
  DataVector<I4Momentum>   ( rhs ),
  DataVector<INavigable4Momentum>( rhs ),
  DataVector<TruthParticle>( rhs ),
  m_genEvent    ( rhs.m_genEvent  ),
  m_etIsolations( rhs.m_etIsolations ),
  m_particles   ( rhs.m_particles )
{}

TruthParticleContainer& TruthParticleContainer::operator=( const TruthParticleContainer& rhs ) 
{
  if ( this != &rhs ) {
    DataVector<TruthParticle>::operator=(rhs);
    m_genEvent     = rhs.m_genEvent;
    m_etIsolations = rhs.m_etIsolations;
    m_particles    = rhs.m_particles;
  }

  return *this;
}
*/

TruthParticleContainer::TruthParticleContainer( const SG::OwnershipPolicy own ) :
  DataVector<TruthParticle>(own),
  m_genEvent    ( ),
  m_etIsolations( ),
  m_particles   ( )
{}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

const TruthParticle* 
TruthParticleContainer::truthParticle( const int barcode,std::size_t genEventIdx ) const
{
  HepMcParticleLink mcLink(barcode, genEventIdx, EBC_MAINEVCOLL, HepMcParticleLink::IS_POSITION);
  Map_t::const_iterator itr = m_particles.find(mcLink.compress());
  if ( itr != m_particles.end() ) {
    return itr->second;
  }
  return 0;
}

std::size_t
TruthParticleContainer::truthParticleIdx( const int barcode, std::size_t genEventIdx ) const
{
  HepMcParticleLink mcLink(barcode, genEventIdx, EBC_MAINEVCOLL, HepMcParticleLink::IS_POSITION);
  Map_t::const_iterator itr = m_particles.find(mcLink.compress());
  if ( itr != m_particles.end() ) {
    for ( std::size_t idx = 0; idx != this->size(); ++idx ) {
      if ( this->operator[](idx) == itr->second ) {
	return idx;
      }
    }
  }
  return NoIdx;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

