///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExDecay.cxx 
// Implementation file for class AthExDecay
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthExThinning includes
#include "AthExThinning/AthExDecay.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

AthExDecay::AthExDecay() :
  m_p1(),
  m_p2(),
  m_l1(),
  m_l2()
{}

AthExDecay::AthExDecay( const AthExDecay& rhs ) :
  m_p1( rhs.m_p1 ),
  m_p2( rhs.m_p2 ),
  m_l1( rhs.m_l1 ),
  m_l2( rhs.m_l2 )
{}

AthExDecay& AthExDecay::operator=( const AthExDecay& rhs )
{
  if ( this != &rhs ) {
    m_p1 = rhs.m_p1;
    m_p2 = rhs.m_p2;
    m_l1 = rhs.m_l1;
    m_l2 = rhs.m_l2;
  }
  return *this;
}

// Destructor
///////////////
AthExDecay::~AthExDecay()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const AthExParticle* AthExDecay::p1() const
{
  if ( m_p1.isValid() ) { return *m_p1;
  } else {                return 0;
  }
}

const AthExParticle* AthExDecay::p2() const
{
  if ( m_p2.isValid() ) { return *m_p2;
  } else {                return 0;
  }
}

const AthExParticle* AthExDecay::l1() const
{
  if ( m_l1.isValid() ) { return *m_l1;
  } else {                return 0;
  }
}

const AthExParticle* AthExDecay::l2() const
{
  if ( m_l2.isValid() ) { return *m_l2;
  } else {                return 0;
  }
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void AthExDecay::setDecay( const ElementLink<AthExParticles>& p1,
			   const ElementLink<AthExParticles>& p2,
			   const ElementLink<AthExParticles>& l1,
			   const ElementLink<AthExParticles>& l2 )
{
  m_p1 = p1;
  m_p2 = p2;
  m_l1 = l1;
  m_l2 = l2;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
