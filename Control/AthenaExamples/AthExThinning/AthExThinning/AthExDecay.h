///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Decay.h 
// Header file for class Decay
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXDECAY_H 
#define ATHEXTHINNING_ATHEXDECAY_H 

// STL includes

// HepMC / CLHEP includes

#include "SGTools/CLASS_DEF.h"

// DataModel includes
#include "AthLinks/ElementLink.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"

// Forward declaration

class AthExDecay
{ 
  // Make the AthenaPoolCnv class our friend
  friend class AthExDecayCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExDecay();

  /** Copy constructor: 
   */
  AthExDecay( const AthExDecay& rhs );

  /** Assignment operator: 
   */
  AthExDecay& operator=( const AthExDecay& rhs ); 

  /** Constructor with parameters: 
   */

  /** Destructor: 
   */
  virtual ~AthExDecay();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  const AthExParticle* p1() const; // kept
  const AthExParticle* p2() const; // kept
  const AthExParticle* l1() const; // lost
  const AthExParticle* l2() const; // lost

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  void setDecay( const ElementLink<AthExParticles>& p1,
		 const ElementLink<AthExParticles>& p2,
		 const ElementLink<AthExParticles>& l1,
		 const ElementLink<AthExParticles>& l2 );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  ElementLink<AthExParticles> m_p1;
  ElementLink<AthExParticles> m_p2;
  ElementLink<AthExParticles> m_l1;
  ElementLink<AthExParticles> m_l2;
  
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

CLASS_DEF( AthExDecay , 21767618, 1 )
#endif //> ATHEXTHINNING_ATHEXDECAY_H 
