///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticle.cxx 
// Implementation file for class CompositeParticle
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <stdexcept>

// ParticleEvent includes
#include "ParticleEvent/CompositeParticle.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

/*
CompositeParticle::CompositeParticle() :
  Base_t()
{}

CompositeParticle::CompositeParticle( const CompositeParticle& rhs ) :
  Base_t( rhs )
{}

CompositeParticle& 
CompositeParticle::CompositeParticle( const CompositeParticle& rhs )
{
  if ( this != &rhs ) {
    Base_t::operator=(rhs);
  }
  return *this;
}
*/

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/** @brief check if a given particle is contained by this composite
 */
bool CompositeParticle::contains (const INavigable4Momentum* particle) const
{
  typedef Base_t::navigable_type navigable_type;
  typedef navigable_type::object_iter object_iter;
  for ( object_iter 
	  itr = this->navigableBase().begin(),
	  iEnd= this->navigableBase().end();
	itr != iEnd;
	++itr ) {
    const INavigable4Momentum* o = *itr;
    if ( o == particle ) {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void
CompositeParticle::add( const INav4MomLink& ipart )
{
  // update 4-momentum
  set4Mom( (*ipart)->hlv() + hlv() );
  // update navigation
  putElement( ipart );
}

void
CompositeParticle::add( const INav4MomLinkContainer* iparts )
{
  if (iparts) {
    for ( std::size_t i = 0, iEnd = iparts->size(); i != iEnd; ++i ) {
      add( (*iparts)[i] );
    }
  } else {
    throw std::runtime_error("Null pointer to std::vector<INav4MomLink> !");
  }
}

void
CompositeParticle::add( const INav4MomLinkContainer& iparts )
{
  for ( std::size_t i = 0, iEnd = iparts.size(); i != iEnd; ++i ) {
    add( iparts[i] );
  }
}

void
CompositeParticle::add( const INav4MomLink& p1,
                        const INav4MomLink& p2 )
{
  // update 4-momentum
  set4Mom( (*p1)->hlv() + (*p2)->hlv() + hlv());

  // update navigation
  putElement( p1 );
  putElement( p2 );
}

void
CompositeParticle::add( const INav4MomLink& p1,
                        const INav4MomLink& p2,
                        const INav4MomLink& p3 )
{
  // update 4-momentum
  set4Mom( (*p1)->hlv() + (*p2)->hlv() + (*p3)->hlv() + hlv());

  // update navigation
  putElement( p1 );
  putElement( p2 );
  putElement( p3 );
}

void
CompositeParticle::add( const INav4MomLink& p1,
                        const INav4MomLink& p2,
                        const INav4MomLink& p3,
                        const INav4MomLink& p4 )
{
  // update 4-momentum
  set4Mom( (*p1)->hlv() + (*p2)->hlv() + (*p3)->hlv() + (*p4)->hlv() + hlv());

  // update navigation
  putElement( p1 );
  putElement( p2 );
  putElement( p3 );
  putElement( p4 );
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// helper method to put elements into the underlying Navigable container
inline
void 
CompositeParticle::putElement( const INav4MomLink& ipart )
{
  INavigable4MomentumCollection::size_type objectIndex = ipart.index();
  const INavigable4MomentumCollection* ipcont = ipart.getStorableObjectPointer();

  navigableBase().putElement( ipcont, objectIndex );
}

