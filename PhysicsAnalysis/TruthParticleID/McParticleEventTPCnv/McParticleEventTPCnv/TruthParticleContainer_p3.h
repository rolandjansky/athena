///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainer_p3.h 
// Header file for class TruthParticleContainer_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P3_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P3_H 

// STL includes
#include <vector>
#include <map>

// Persistent ElementLink
#include "DataModelAthenaPool/ElementLink_p1.h"

// HepMC / CLHEP includes
//#include "GeneratorObjects/McEventCollection.h"

// Gaudi includes

// Forward declaration
class TruthParticleContainerCnv_p3;

class TruthParticleContainer_p3
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TruthParticleContainerCnv_p3;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainer_p3();

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

inline TruthParticleContainer_p3::TruthParticleContainer_p3() :
  m_genEvent ( ),
  m_particles( )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P3_H
