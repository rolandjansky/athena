///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleMomentum.cxx 
// Implementation file for class TruthParticleMomentum
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <ostream>
#include <sstream>
#include <iomanip>

// McParticleEvent includes
#include "McParticleEvent/TruthParticleMomentum.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

TruthParticleMomentum::TruthParticleMomentum() :
  m_hlv( 0 )
{}

TruthParticleMomentum::TruthParticleMomentum( const TruthParticleMomentum& rhs ) :
  m_hlv( rhs.m_hlv )
{}


TruthParticleMomentum& 
TruthParticleMomentum::operator=( const TruthParticleMomentum& rhs )
{
  if ( this != &rhs ) {
    m_hlv = rhs.m_hlv;
  }
  return *this;
}

TruthParticleMomentum::TruthParticleMomentum( const HepMC::FourVector& hlv ) :
  m_hlv( &hlv )
{}

TruthParticleMomentum::~TruthParticleMomentum()
{}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

std::ostream& TruthParticleMomentum::dump( std::ostream& out ) const
{
  std::stringstream buf;
  buf << "[px,py,pz,e] ="
      << std::right << std::scientific << std::setprecision(8) 
      << std::setw(16) << this->px()
      << std::setw(16) << this->py()
      << std::setw(16) << this->pz()
      << std::setw(16) << this->e();
  return out << buf.str();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

