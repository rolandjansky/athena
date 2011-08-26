/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McUtils.cxx
// Implementation file for various Mc helpers and methods
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <string>

// boost includes
#include "boost/array.hpp"

// FrameWork includes
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// CLHEP/HepMC includes
#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

// McParticleUtils includes
#include "McParticleUtils/McUtils.h"

namespace {

/** 3*charge for basic pdgId codes -- used to parse unknown id's
    Fix from Frank for the charge of the MC Truth Particle */
static const boost::array<int, 100> qcharge =
  {{+0, -1, +2, -1, +2, -1, +2, -1, +2, +0,  //  0- 9
    +0, -3, +0, -3, +0, -3, +0, -3, +0, +0,  // 10-19
    +0, +0, +0, +3, +0, +0, +0, +0, +0, +0,  // 20-29
    +0, +0, +0, +3, +0, +0, +0, +3, +0, +0,  // 30-39
    +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
    +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
    +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
    +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
    +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
    +0, +0, +0, +0, +0, +0, +0, +0, +0, +0}};

}


double McUtils::chargeFromPdgId( const int pdgId, 
				 const HepPDT::ParticleDataTable* pdt )
{
  using std::abs;
  if ( !pdt ) {
    throw std::invalid_argument( "Null pointer to HepPDT::ParticleDataTable !" );
  }

  /** FIXME : should use HepPDT::ParticleDataTable to retrieve
      the charge of the MC particle
      Problem : how do we handle particles which are not in the
      PDGTABLE.MeV file ? */
  const HepPDT::ParticleData* ap = pdt->particle( abs( pdgId ) );
  if ( ap ) {
    return ( pdgId < 0 ) 
      ? -(ap->charge()) 
      :  ap->charge();
  } else if ( 0 == pdgId ) {
    // we are trying to compute a charge for a non existing particle...
    //throw std::invalid_argument( "Non existing PDG-id" );
    return -999.;
  } else {
    /** Set charge using PDG convention:
	id = nnnnijkl
	i == 0, j == 0:   see qcharge[100]
	i == 0:           meson, j kbar quarks    l = 2*spin+1
	i != 0:           baryon, i j k quarks    l = 2*spin+1
	Default is 0; */
    const int idmod = abs(pdgId) % 10000;
    const int q1 = (idmod/10) % 10;
    const int q2 = (idmod/100) % 10;
    const int q3 = (idmod/1000) % 10;
    double q = 0;

    if (abs(pdgId) >= 1000000000) {
      // Seems to be a nucleus: 100pppnnn0
      q = (abs(pdgId) / 10000) % 1000;
    }
    else if( idmod < 100 ) {
      q = qcharge[idmod]/3.;
    }
    else if ( idmod < 1000 ) {
      q = (qcharge[q1]-qcharge[q2])/3.;
      if ( qcharge[q2] == 2 ) {
	q *= -1.;
      }
    }
    else if( idmod < 10000 ) {
      q = (qcharge[q3]+qcharge[q2]+qcharge[q1])/3.;
    }
    if (q == 0) q = 0; // Change -0 -> 0.
    return (pdgId < 0) ? -q : q;
  }
}

