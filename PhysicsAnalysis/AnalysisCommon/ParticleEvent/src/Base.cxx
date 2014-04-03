///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base.cxx 
// Implementation file for class Base
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <ostream>
#include <sstream>
#include <iomanip>

// ParticleEvent includes
#include "ParticleEvent/Lib/Base.h"

namespace ParticleEvent {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const ElementLink<VxContainer>&
Base::originLink() const
{
  return m_origin;
}

std::ostream& 
Base::dump( std::ostream& out ) const
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

      << "[origin  ] = ";
  if ( this->origin() ) { this->origin()->dump( buf );
  } else {                buf << " --Null ptr-- ";
  }

  this->getAthenaBarCodeImpl().dump(buf);

  return out << buf.str();
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
std::ostream& operator<<( std::ostream& out, const Base& o )
{
  return o.dump( out );
}

} //> end namespace ParticleEvent
