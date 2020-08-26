/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticle.cxx 
// Implementation file for class TruthParticle
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <sstream>
#include <string>

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"

#include "VxVertex/RecVertex.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TruthParticle::TruthParticle() :
  INavigable          ( ),
  I4Momentum          ( ),
  INavigable4Momentum ( ),
  TruthParticleImpl_t ( ),
  m_container    ( 0 ),
  m_mothers      ( 0 ),
  m_children     ( 0 ),
  m_nGenEventIdx ( 0 )
{}

TruthParticle::TruthParticle( const TruthParticle& rhs ) :
  IAthenaBarCode      ( rhs ),
  INavigable          ( rhs ),
  I4Momentum          ( rhs ),
  INavigable4Momentum ( rhs ),
  TruthParticleImpl_t ( rhs ),
  m_container      ( rhs.m_container ),
  m_mothers        ( rhs.m_mothers ),
  m_children       ( rhs.m_children ),
  m_nGenEventIdx   ( rhs.m_nGenEventIdx )
{}

TruthParticle::TruthParticle( HepMC::ConstGenParticlePtr particle, 
                              const TruthParticleContainer * container ) :
  INavigable          ( ),
  I4Momentum          ( ),
  INavigable4Momentum ( ),
  TruthParticleImpl_t ( ),
  m_container      ( container ),
  m_mothers        ( 0 ),
  m_children       ( 0 ),
  m_nGenEventIdx   ( 0 )
{
  setGenParticle( particle );
  m_nGenEventIdx = container ? container->genEventIdx() : 0;
}

TruthParticle& TruthParticle::operator=( const TruthParticle& rhs )
{
  if ( this != &rhs ) {
    INavigable::operator=(rhs);
    I4Momentum::operator=(rhs);
    INavigable4Momentum::operator=(rhs);
    TruthParticleImpl_t::operator=(rhs);
    m_container = rhs.m_container;
    m_mothers   = rhs.m_mothers;
    m_children  = rhs.m_children;
    m_nGenEventIdx=rhs.m_nGenEventIdx;
  }
  return *this;
}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const TruthParticle * TruthParticle::mother(const std::size_t i) const
{
  if ( m_mothers.empty()     || 
       i >= m_mothers.size() || 
       0 == m_container      ) return 0;
  return m_container->truthParticle(m_mothers[i], m_nGenEventIdx);
}

const TruthParticle * TruthParticle::child(const std::size_t i) const
{
  if ( m_children.empty()     || 
       i >= m_children.size() ||
       0 == m_container       ) return 0;
  return m_container->truthParticle(m_children[i], m_nGenEventIdx);
}


HepMC::ConstGenParticlePtr TruthParticle::genMother(const std::size_t i) const
{
  if ( i < m_mothers.size() ) {
    auto mother = this->mother(i);
    return mother ? mother->genParticle() : nullptr;
  } else {
    std::string error = "WRONG index for TruthParticle::genMother(index)";
    throw std::out_of_range(error);
  }
}

HepMC::ConstGenParticlePtr TruthParticle::genChild(const std::size_t i) const
{
  if ( i < m_children.size() ) {
    auto child = this->child(i);
    return child ? child->genParticle() : nullptr;
  } else {
    std::string error = "WRONG index for TruthParticle::genChild(index)";
    throw std::out_of_range(error);
  }
}

bool TruthParticle::hasMother( const PDG::pidType pdgId ) const
{
  const std::size_t nMothers = m_mothers.size();
  for ( std::size_t iMother = 0; iMother != nMothers; ++iMother ) {
    auto mother = this->mother(iMother);
    if ( mother && ( pdgId == mother->pdgId() ) ) {
      return true;
    }
  }
  return false;
}

bool TruthParticle::hasChild( const PDG::pidType pdgId ) const
{
  const std::size_t nChildren = m_children.size();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    auto child = this->child(iChild);
    if ( child && ( pdgId == child->pdgId() ) ) {
      return true;
    }
  }
  return false;
}

bool TruthParticle::hasMother( const PDG::pidType pdgId, 
			       std::vector<unsigned int>& indices ) const
{
  bool found = false;
  const std::size_t nMothers = m_mothers.size();
  for ( std::size_t iMother = 0; iMother != nMothers; ++iMother ) {
    auto mother = this->mother(iMother);
    if ( mother && ( pdgId == mother->pdgId() ) ) {
      found = true;
      indices.push_back(iMother);
    }
  }
  return found;
}

bool TruthParticle::hasChild( const PDG::pidType pdgId, 
			      std::vector<unsigned int>& indices ) const
{
  bool found = false;
  const std::size_t nChildren = m_children.size();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    auto child = this->child(iChild);
    if ( child && ( pdgId == child->pdgId() ) ) {
      found = true;
      indices.push_back(iChild);
    }
  }
  return found;
}

bool TruthParticle::hasEtIsol() const
{
  return m_container->hasEtIsol( barcode() );
}

const TruthEtIsolations::EtIsol_t* TruthParticle::etIsol() const
{
  return m_container->etIsolations( barcode() );
}

double 
TruthParticle::etIsol( const TruthParticleParameters::ConeSize coneIdx ) const 
{ 
  if ( coneIdx >= 0 && // just to be sure... as it is a signed integer...
       coneIdx < TruthParticleParameters::NbrOfCones ) {
    return m_container->etIsol( barcode(), coneIdx );
  } else {
    std::string error = "WRONG index for TruthParticle::etIsol(coneIdx)";
    throw std::out_of_range(error);
  }
}

CLHEP::HepLorentzVector TruthParticle::pDecay( const std::size_t i ) const 
{ 
  if ( i < m_children.size() ) {
    auto child = this->child(i);
    if ( child ) {
      return child->hlv(); 
    } else {
      const std::string error = "Caught INVALID pointer to child in TruthParticle::pDecay call!";
      throw std::out_of_range(error);
    }
  } else {
    std::string error = "Caught INVALID pointer to child in TruthParticle::pDecay call!";
    throw std::out_of_range(error);
  }
}

PDG::pidType TruthParticle::pdgDecay( const std::size_t i ) const 
{ 
  if ( i < m_children.size() ) {
    auto child = this->child(i);
    if ( child ) {
      return child->pdgId(); 
    } else {
      const std::string error = "Caught INVALID pointer to child in TruthParticle::pdgDecay call!";
      throw std::out_of_range(error);
    }
  } else {
    const std::string error = "Caught INVALID pointer to child in TruthParticle::pdgDecay call!";
    throw std::out_of_range(error);
  }
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void TruthParticle::setGenParticle( HepMC::ConstGenParticlePtr particle )
{
  this->particleBase().setGenParticle( particle );

  if ( particle ) {
    this->set4Mom(particle->momentum());

    // children
    auto dcyVtx = particle->end_vertex();
#ifdef HEPMC3
    m_children.reserve( dcyVtx ? dcyVtx->particles_out().size() : 0 );

    if ( dcyVtx ) {
      for ( auto  itr: dcyVtx->particles_out()) {
        m_children.push_back( HepMC::barcode(itr) );
      }//> end loop over outgoing particles
    }//> decay vertex exists

    // parents
    auto prodVtx = particle->production_vertex();
    m_mothers.reserve( prodVtx ? prodVtx->particles_in().size() : 0 );
    if ( prodVtx ) {
      for ( auto itr:  prodVtx->particles_in()) {
        m_mothers.push_back( HepMC::barcode(itr) );
      }//> end loop over ingoing particles
    }//> production vertex exists
#else

    m_children.reserve( dcyVtx ? dcyVtx->particles_out_size() : 0 );

    if ( dcyVtx ) {
      for ( HepMC::GenVertex::particles_out_const_iterator itr = 
              dcyVtx->particles_out_const_begin();
            itr != dcyVtx->particles_out_const_end();
            ++itr ) {
        m_children.push_back( (*itr)->barcode() );
      }//> end loop over outgoing particles
    }//> decay vertex exists

    // parents
    auto prodVtx = particle->production_vertex();
    m_mothers.reserve( prodVtx ? prodVtx->particles_in_size() : 0 );
    if ( prodVtx ) {
      for ( HepMC::GenVertex::particles_in_const_iterator itr = 
              prodVtx->particles_in_const_begin();
            itr != prodVtx->particles_in_const_end();
            ++itr ) {
        m_mothers.push_back( (*itr)->barcode() );
      }//> end loop over ingoing particles
    }//> production vertex exists

#endif
  } else {
    m_mothers.resize( 0 );
    m_children.resize( 0 );
  }
}

