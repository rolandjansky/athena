/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENPDG_PROPERTY_H
#define GENPDG_PROPERTY_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Algorithm.h"

#include <utility>

namespace HepPDT { class ParticleDataTable; }
namespace CLHEP { class HepRandomEngine; }

/// An "admixture" class handling particle type configuration.
/// 
/// @author Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2008
/// 

class GenPDG_Property {
public:
  template<class ParentAlg> GenPDG_Property(ParentAlg *);
  
  StatusCode initialize(const HepPDT::ParticleDataTable*);
  
  /// returns  (PDG ID, mass) pair
  typedef std::pair<int,double> ParticleDescription;
  ParticleDescription getParticle(CLHEP::HepRandomEngine* ) const;

  
private:

  Algorithm* m_parent;

  /// PDG ID, or absolute value of it, of generated particles. See also m_generateAntiParticles.
  int m_pdg_id;
  
  /// False means use exactly the given PDG ID, true means generate both
  /// particles and antiparticles.
  bool m_generateAntiParticles;
  
  //----------------------------------------------------------------
  /// cached value
  double m_mass; 

};


//--------------------------------------------------------------------------


template<class ParentAlg>
GenPDG_Property::GenPDG_Property(ParentAlg *parent)
  : m_parent(parent)
{
  parent->declareProperty("pdg_id", m_pdg_id = 13);
  parent->declareProperty("generateAntiParticles", m_generateAntiParticles = true);
}

#endif /*GENPDG_PROPERTY_H*/
