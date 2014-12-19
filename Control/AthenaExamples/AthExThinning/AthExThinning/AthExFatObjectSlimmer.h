///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObjectSlimmer.h 
// Header file for class AthExFatObjectSlimmer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXFATOBJECTSLIMMER_H
#define ATHEXTHINNING_ATHEXFATOBJECTSLIMMER_H 1

// STL includes
#include <iosfwd>

// AthenaKernel includes
#include "AthenaKernel/ISlimmingHdlr.h"

#include "AthLinks/ElementLink.h"
#include "AthExThinning/AthExParticles.h"

// Forward declaration
class AthExFatObject;

class AthExFatObjectSlimmer : public Athena::ISlimmingHdlr
{ 
  typedef ElementLink<AthExParticles> AthExParticleLink_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Copy constructor: 
  AthExFatObjectSlimmer( const AthExFatObjectSlimmer& rhs );

  /// Assignment operator: 
  AthExFatObjectSlimmer& operator=( const AthExFatObjectSlimmer& rhs ); 

  /// Constructor with parameters: 
  AthExFatObjectSlimmer (AthExFatObject* obj,
			 const ::INamedInterface* requester);

  /// Destructor: 
  virtual ~AthExFatObjectSlimmer(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** @{ @c Athena::Interface implementation
   */

  /** @brief returns a pointer to the object being slimmed
   */
  virtual void *object();

  /** @brief returns the type-id of the object being slimmed
   *         (mostly for debugging purposes)
   */
  virtual 
  const std::type_info& type_id();

  /** @brief returns the component who requested the registration of that
   *         slimming handler
   *         (mostly for debugging purposes)
   */
  virtual 
  const ::INamedInterface* requester() { return m_requester; }

  /** @brief apply the slimming: remove parts of the object
   */
  virtual void commit();

  /** @brief restore object's state as before slimming was applied
   */
  virtual void rollback();

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthExFatObjectSlimmer(); // not implemented.

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /** pointer to object being slimmed
   */
  AthExFatObject *m_object;

  /** pointer to the component requesting slimming of the above object
   */
  const INamedInterface *m_requester;

  /** backup of data being slimmed
   */
  const AthExParticle* m_particle;

  /** backup of another piece being slimmed
   */
  AthExParticleLink_t m_particleLink;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const AthExFatObjectSlimmer& o );



#endif //> !ATHEXTHINNING_ATHEXFATOBJECTSLIMMER_H
