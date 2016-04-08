///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObject.h 
// Header file for class AthExFatObject
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXFATOBJECT_H
#define ATHEXTHINNING_ATHEXFATOBJECT_H 1

// STL includes
#include <iosfwd>

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"

// datamodel
#include "SGTools/CLASS_DEF.h"
#include "AthLinks/ElementLink.h"

// Forward declaration
class AthExFatObjectSlimmer;


class AthExFatObject
{ 
  // for slimming
  friend class AthExFatObjectSlimmer;

  // Make the AthenaPoolCnv class our friend
  friend class AthExFatObjectCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
public: 
  typedef ElementLink<AthExParticles> AthExParticleLink_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Default constructor: 
  AthExFatObject();

  /// Copy constructor: 
  AthExFatObject( const AthExFatObject& rhs );

  /// Assignment operator: 
  AthExFatObject& operator=( const AthExFatObject& rhs ); 

  /// Constructor with parameters: 
  AthExFatObject (const ElementLink<AthExParticles>& particle);

  /// Destructor: 
  virtual ~AthExFatObject(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  
  /** raw pointer access
   */
  const AthExParticle* particle() const { return m_particle; }

  /** element link access
   */
  const AthExParticleLink_t& particleLink() const { return m_particleLink; }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** set the internals of the fat object
   */
  void setLink (const AthExParticleLink_t& link);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 
  
  /** a raw pointer to an @c AthExParticle
   */
  const AthExParticle* m_particle;

  /** a persistable pointer to an @c AthExParticle
   */
  AthExParticleLink_t m_particleLink;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const AthExFatObject& o );


// for storegate
CLASS_DEF( AthExFatObject , 166093780 , 1 )

#endif //> !ATHEXTHINNING_ATHEXFATOBJECT_H
