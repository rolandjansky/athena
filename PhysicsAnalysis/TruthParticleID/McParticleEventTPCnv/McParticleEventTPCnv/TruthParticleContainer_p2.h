///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainer_p2.h 
// Header file for class TruthParticleContainer_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P2_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P2_H 

// STL includes
#include <vector>
#include <string>
#include <map>

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class TruthParticleContainerCnv_p2;

class TruthParticleContainer_p2
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TruthParticleContainerCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainer_p2();

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

  /** The key of the @c McEventCollection from which the (transient)
   *  @c TruthParticleContainer has been made.
   */
  std::string m_parentKey;
  
  /** Persistent representation of a vector of @c TruthParticles
   *  we only store the vector of transverse energy isolation for
   *  different (pre-defined) isolation cones, this vector being
   *  labeled by the barcode of the @c GenParticle from which it has
   *  been computed.
   */
  std::map< unsigned int, std::vector<float> > m_particles;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthParticleContainer_p2::TruthParticleContainer_p2() :
  m_parentKey( "NO_KEY" ),
  m_particles( )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P2_H
