///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleBase.cxx 
// Implementation file for class TruthParticleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <ostream>
#include <sstream>
#include <iomanip>

// McParticleEvent includes
#include "McParticleEvent/TruthParticleBase.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

TruthParticleBase::TruthParticleBase() :
  m_charge     ( -999. ),
  m_genParticle(    0  ),
  m_abc        ()
{}

TruthParticleBase::TruthParticleBase( const TruthParticleBase& rhs ) :
  m_charge     ( rhs.m_charge ),
  m_genParticle( rhs.m_genParticle ),
  m_abc        ( rhs.m_abc)
{}

TruthParticleBase& 
TruthParticleBase::operator=( const TruthParticleBase& rhs )
{
  if ( this != &rhs ) {
    m_charge      = rhs.m_charge;
    m_genParticle = rhs.m_genParticle;
    m_abc         = rhs.m_abc;
  }
  return *this;
}

TruthParticleBase::TruthParticleBase( const HepMC::GenParticlePtr mc ) :
  m_charge     ( -999. ),
  m_genParticle(   mc  ),
  m_abc        ()
{}


// Destructor
///////////////

TruthParticleBase::~TruthParticleBase()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const Trk::RecVertex*
TruthParticleBase::origin() const
{
  return 0;
}

/** Return the ElementLink to particle Origin  */
const ElementLink<VxContainer>& 
TruthParticleBase::originLink() const
{
  static ElementLink<VxContainer> origin;
  return origin;
}


std::ostream& TruthParticleBase::dump( std::ostream& out ) const
{
  std::stringstream buf;
  buf << "[dataType] = " << this->dataType() << "\n"

      << "[charge  ] = " 
      << std::setw(5) << std::boolalpha << this->hasCharge()
      << std::scientific << std::right << std::setw(16) << std::setprecision(8)
      << this->charge()  << "\n"

      << "[pdg-Id  ] = " << std::setw(5) << std::boolalpha << this->hasPdgId()
      << std::fixed << std::right << std::setw(16) << std::setprecision(8)
      << this->pdgId()   << "\n"

      << "[origin  ] =  --Null ptr-- ";

  out << buf.str();

  //Dump the AthenaBarCode Part
  m_abc.dump(out);

  return out;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// I/O functions
/////////////////////////////////////////////////////////////////// 
std::ostream& operator<<( std::ostream& out, const TruthParticleBase& o )
{
  return o.dump( out );
}
