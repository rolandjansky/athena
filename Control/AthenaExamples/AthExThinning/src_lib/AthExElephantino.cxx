///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExElephantino.cxx 
// Implementation file for class AthExElephantino
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthExThinning includes
#include "AthExThinning/AthExElephantino.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

AthExElephantino::AthExElephantino() :
  m_legs(),
  m_ears()
{}

AthExElephantino::AthExElephantino( const AthExElephantino& rhs ) :
  m_legs( rhs.m_legs ),
  m_ears( rhs.m_ears )
{}

AthExElephantino& AthExElephantino::operator=( const AthExElephantino& rhs )
{
  if ( this != &rhs ) {
    m_legs = rhs.m_legs;
    m_ears = rhs.m_ears;
  }
  return *this;
}

// Destructor
///////////////
AthExElephantino::~AthExElephantino()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const AthExIParticle* AthExElephantino::leg1() const
{
  assert( m_legs.size() == 4 );
  if ( m_legs[0].isValid() ) { return *m_legs[0];
  } else {                     return 0;
  }
}

const AthExIParticle* AthExElephantino::leg2() const
{
  assert( m_legs.size() == 4 );
  if ( m_legs[1].isValid() ) { return *m_legs[1];
  } else {                     return 0;
  }
}

const AthExIParticle* AthExElephantino::leg3() const
{
  assert( m_legs.size() == 4 );
  if ( m_legs[2].isValid() ) { return *m_legs[2];
  } else {                     return 0;
  }
}

const AthExIParticle* AthExElephantino::leg4() const
{
  assert( m_legs.size() == 4 );
  if ( m_legs[3].isValid() ) { return *m_legs[3];
  } else {                     return 0;
  }
}

const AthExIParticle* AthExElephantino::ear1() const
{
  assert (m_ears.size() == 2);
  if (m_ears[0].isValid()) { return *m_ears[0];
  } else {                   return 0;
  }
}

const AthExIParticle* AthExElephantino::ear2() const
{
  assert (m_ears.size() == 2);
  if (m_ears[1].isValid()) { return *m_ears[1];
  } else {                   return 0;
  }
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void AthExElephantino::setLegs( const ElementLink<AthExIParticles>& leg1,
				const ElementLink<AthExIParticles>& leg2,
				const ElementLink<AthExIParticles>& leg3,
				const ElementLink<AthExIParticles>& leg4 )
{
  m_legs.clear();
  m_legs.reserve( 4 );
  m_legs.push_back( leg1 );
  m_legs.push_back( leg2 );
  m_legs.push_back( leg3 );
  m_legs.push_back( leg4 );
}

void AthExElephantino::setEars( const ElementLink<AthExIParticles>& ear1,
				const ElementLink<AthExIParticles>& ear2 )
{
  m_ears.clear();
  m_ears.reserve( 2 );
  m_ears.push_back( ear1 );
  m_ears.push_back( ear2 );
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
