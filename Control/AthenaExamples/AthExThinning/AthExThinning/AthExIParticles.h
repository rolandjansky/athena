///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExIParticles.h 
// Header file for class AthExIParticles
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXIPARTICLES_H 
#define ATHEXTHINNING_ATHEXIPARTICLES_H 

// STL includes

// Gaudi includes

#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"

// AthExThinning includes
#include "AthExThinning/AthExIParticle.h"

// Forward declaration


//typedef DataVector<AthExIParticle> AthExIParticles;
class AthExIParticles : public DataVector<AthExIParticle>
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExIParticles();

  /** Copy constructor: 
   */
  AthExIParticles( const AthExIParticles& rhs );

  /** Assignment operator: 
   */
  AthExIParticles& operator=( const AthExIParticles& rhs ); 

  /** Constructor with parameters: 
   */
  AthExIParticles( const SG::OwnershipPolicy own );

  /** Destructor: 
   */
  virtual ~AthExIParticles();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

CLASS_DEF( AthExIParticles , 141971559, 1 )

#endif //> ATHEXTHINNING_ATHEXIPARTICLES_H
