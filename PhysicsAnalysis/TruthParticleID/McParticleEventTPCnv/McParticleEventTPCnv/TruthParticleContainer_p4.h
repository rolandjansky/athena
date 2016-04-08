///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainer_p4.h 
// Header file for class TruthParticleContainer_p4
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P4_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P4_H 

// STL includes
#include <vector>
#include <utility>

// Persistent ElementLink
#include "DataModelAthenaPool/ElementLink_p1.h"

// HepMC / CLHEP includes
//#include "GeneratorObjects/McEventCollection.h"

// Gaudi includes

// Forward declaration
class TruthParticleContainer;
class TruthParticleContainer_p4;
class TruthParticleContainerCnv_p4;
template <typename TRANS, typename PERS> class T_TPCnv;
template <>
class T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>;

class TruthParticleContainer_p4
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TruthParticleContainerCnv_p4;
  friend class T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainer_p4();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Retrieve the transverse energies (for different isolation cones) for a
   *  given particle (through its barcode)
   */
  const std::vector<float>& etIsol( const unsigned int barcode ) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** The persistent pointer toward the @c McEventCollection the (transient)
   *  @c TruthParticleContainer is proxying
   */
  ElementLinkInt_p1 m_genEvent;

  //typedef GaudiUtils::VectorMap< unsigned int, std::vector<float> > Particles_t;
  typedef std::vector< std::pair< unsigned int, std::vector<float> > > Particles_t;
  /** Persistent representation of a vector of @c TruthParticles
   *  we only store the vector of transverse energy isolation for
   *  different (pre-defined) isolation cones, this vector being
   *  labeled by the barcode of the @c GenParticle from which it has
   *  been computed.
   */
  Particles_t m_particles;

 protected:
  
  struct BarcodeFinder
  {
    BarcodeFinder( const std::size_t barcode ) : m_barcode(barcode) {}
    
    bool operator()( const Particles_t::value_type& p ) const {
      return p.first == m_barcode;
    }
    
    // the barcode we are looking for
    std::size_t m_barcode;
  };

  struct BarcodeSorter
  {
    bool operator()( const Particles_t::value_type& lhs,
		     const Particles_t::value_type& rhs ) const {
      return lhs.first < rhs.first;
    }
  };


}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthParticleContainer_p4::TruthParticleContainer_p4() :
  m_genEvent ( ),
  m_particles( )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P4_H
