///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Elephantino.h 
// Header file for class Elephantino
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXELEPHANTINO_H 
#define ATHEXTHINNING_ATHEXELEPHANTINO_H 

/// @class AthExElephantino is an object with 4 legs, so it is best modeled 
/// with a vector of @c ElementLink.

// STL includes
#include <vector>

// HepMC / CLHEP includes

#include "SGTools/CLASS_DEF.h"

// DataModel includes
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// AthExThinning includes
#include "AthExThinning/AthExIParticles.h"

// Forward declaration

class AthExElephantino
{ 

  // Make the AthenaPoolCnv class our friend
  friend class AthExElephantinoCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExElephantino();

  /** Copy constructor: 
   */
  AthExElephantino( const AthExElephantino& rhs );

  /** Assignment operator: 
   */
  AthExElephantino& operator=( const AthExElephantino& rhs ); 

  /** Constructor with parameters: 
   */

  /** Destructor: 
   */
  virtual ~AthExElephantino();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  const AthExIParticle* leg1() const;
  const AthExIParticle* leg2() const;
  const AthExIParticle* leg3() const;
  const AthExIParticle* leg4() const;

  const AthExIParticle* ear1() const;
  const AthExIParticle* ear2() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  void setLegs( const ElementLink<AthExIParticles>& p1,
		const ElementLink<AthExIParticles>& p2,
		const ElementLink<AthExIParticles>& p3,
		const ElementLink<AthExIParticles>& p4 );

  void setEars( const ElementLink<AthExIParticles>& e1,
		const ElementLink<AthExIParticles>& e4 );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  //std::vector< ElementLink<AthExIParticles> > m_legs;
  ElementLinkVector<AthExIParticles> m_legs;
  ElementLinkVector<AthExIParticles> m_ears;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

CLASS_DEF( AthExElephantino, 107042295, 1 )
#endif //> ATHEXTHINNING_ATHEXELEPHANTINO_H 
