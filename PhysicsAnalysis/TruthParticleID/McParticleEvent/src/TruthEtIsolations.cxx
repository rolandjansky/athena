/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthEtIsolations.cxx 
// Implementation file for class TruthEtIsolations
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <sstream>
#include <ostream>
#include <iomanip>
#include <stdexcept>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"

// McParticleEvent includes
#include "McParticleEvent/TruthEtIsolations.h"

typedef TruthEtIsolations::GenEventLink_t GenEventLink_t;
typedef TruthEtIsolations::EtIsol_t       EtIsol_t;

EtIsol_t TruthEtIsolations::m_dfltEtIsols = {
  {
    -999.*CLHEP::GeV, -999.*CLHEP::GeV, -999.*CLHEP::GeV, -999.*CLHEP::GeV,
    -999.*CLHEP::GeV, -999.*CLHEP::GeV, -999.*CLHEP::GeV, -999.*CLHEP::GeV
  }
};
  

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TruthEtIsolations::TruthEtIsolations() :
  m_genEvtLink   ( ),
  m_etIsolations ( )
{}

TruthEtIsolations::TruthEtIsolations( const TruthEtIsolations& rhs ) :
  m_genEvtLink   ( rhs.m_genEvtLink ),
  m_etIsolations ( rhs.m_etIsolations )
{}


TruthEtIsolations::TruthEtIsolations( const GenEventLink_t& genEvtLink ) :
  m_genEvtLink   ( genEvtLink ),
  m_etIsolations ( )
{}

TruthEtIsolations::TruthEtIsolations( const std::string& genEvtName,
				      const std::size_t  genEvtIdx ) :
  m_genEvtLink  ( genEvtName, genEvtIdx ),
  m_etIsolations( )
{}

TruthEtIsolations& 
TruthEtIsolations::operator=( const TruthEtIsolations& rhs )
{
  if ( this != &rhs ) {
    m_genEvtLink   = rhs.m_genEvtLink;
    m_etIsolations = rhs.m_etIsolations;
  }
  return *this;
}

// Destructor
///////////////

TruthEtIsolations::~TruthEtIsolations()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const std::string& TruthEtIsolations::genEventName() const
{
  return m_genEvtLink.dataID();
}

std::size_t TruthEtIsolations::genEventIdx() const
{
  return m_genEvtLink.index();
}

bool 
TruthEtIsolations::hasEtIsol( HepMC::ConstGenParticlePtr p ) const
{
  return ( 0 == p ) 
    ? false
    : hasEtIsol( HepMC::barcode(p) );
}

const TruthEtIsolations::EtIsol_t* 
TruthEtIsolations::etIsolations( HepMC::ConstGenParticlePtr p ) const
{
  return ( 0 == p ) 
    ? 0
    : etIsolations( HepMC::barcode(p) );
}
  
double 
TruthEtIsolations::etIsol( HepMC::ConstGenParticlePtr p,
			   const TruthParticleParameters::ConeSize idx ) const
{
  return ( 0 == p ) 
    ? m_dfltEtIsols[idx]
    : etIsol( HepMC::barcode(p), idx );
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void 
TruthEtIsolations::setEtIsol( HepMC::ConstGenParticlePtr genParticle,
			      const TruthParticleParameters::ConeSize coneIdx,
			      const double etIsol )
{
  m_etIsolations[HepMC::barcode(genParticle)][coneIdx] = etIsol;
}

void 
TruthEtIsolations::setEtIsol( HepMC::ConstGenParticlePtr genParticle,
			      const std::vector<double>& etIsols )
{
  EtIsol_t& et = m_etIsolations[HepMC::barcode(genParticle)];
  for ( std::size_t 
	  i    = 0, 
	  iMax = std::min<std::size_t>( etIsols.size(), 
					TruthParticleParameters::NbrOfCones ); 
	i != iMax;
	++i ) {
    et[i] = etIsols[i];
  }

  return;
}
  
void 
TruthEtIsolations::setEtIsol( HepMC::ConstGenParticlePtr genParticle,
			      const EtIsol_t& etIsols )
{
  m_etIsolations[HepMC::barcode(genParticle)] = etIsols;
}
  
/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// I/O functions
/////////////////////////////////////////////////////////////////// 
std::ostream& 
operator<<( std::ostream& out, const TruthEtIsolations& etIsols )
{
  const std::size_t nEtIsols = etIsols.size();
  out << "nbr of EtIsols: " << nEtIsols;
  std::ostringstream buf; 
  buf << std::right << std::scientific << std::setprecision(8);
  for ( TruthEtIsolations::const_iterator
	  i    = etIsols.begin(),
	  iEnd = etIsols.end();
	i != iEnd;
	++i ) {
    buf.str("");
    buf << "\nbc: " << i->first << " [ ";
    std::copy( i->second.begin(), i->second.end(),
	       std::ostream_iterator<double>( buf, " " ) );
    out << buf.str() << "]";
  }
  return out;
}

