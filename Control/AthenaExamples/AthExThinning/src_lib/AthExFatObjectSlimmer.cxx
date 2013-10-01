///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObjectSlimmer.cxx 
// Implementation file for class AthExFatObjectSlimmer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExThinning includes
#include "AthExThinning/AthExFatObject.h"
#include "AthExThinning/AthExFatObjectSlimmer.h"

// STL includes

#if 0
# include <iostream>
# define ATHEX_DEBUG(x) std::cerr << x << "\n"
#else
# define ATHEX_DEBUG(x) do {} while(0)
#endif

typedef ElementLink<AthExParticles> AthExParticleLink_t;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

/// Copy constructor: 
AthExFatObjectSlimmer::AthExFatObjectSlimmer (const AthExFatObjectSlimmer& rhs):
  Athena::ISlimmingHdlr (rhs),
  m_object       (rhs.m_object),
  m_requester    (rhs.m_requester),
  m_particle     (rhs.m_particle),
  m_particleLink (rhs.m_particleLink)
{}

/// Assignment operator: 
AthExFatObjectSlimmer& 
AthExFatObjectSlimmer::operator=( const AthExFatObjectSlimmer& rhs )
{
  if (this != &rhs) {
    m_object       = rhs.m_object;
    m_requester    = rhs.m_requester;
    m_particle     = rhs.m_particle;
    m_particleLink = rhs.m_particleLink;
  }
  return *this;
}

/// Constructor with parameters: 
AthExFatObjectSlimmer::AthExFatObjectSlimmer (AthExFatObject* obj,
					      const ::INamedInterface* req) :
  m_object       (obj),
  m_requester    (req),
  m_particle     (obj->m_particle),
  m_particleLink (obj->m_particleLink)
{}

/// Destructor: 
AthExFatObjectSlimmer::~AthExFatObjectSlimmer()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/** @brief returns a pointer to the object being slimmed
 */
void*
AthExFatObjectSlimmer::object()
{
  return static_cast<void*> (m_object);
}

/** @brief returns the type-id of the object being slimmed
 *         (mostly for debugging purposes)
 */
const std::type_info&
AthExFatObjectSlimmer::type_id()
{
  return typeid(AthExFatObject);
}

/** @brief apply the slimming: remove parts of the object
 */
void
AthExFatObjectSlimmer::commit()
{
  ATHEX_DEBUG ("slimmer::commit...");
  // as we captured the state of the object durint our c-tor, we don't have
  // to do anything there, except for zapping out the object's content.

  m_object->m_particle = 0;
  // moral equivalent of a null pointer
  m_object->m_particleLink = AthExParticleLink_t(); 
  ATHEX_DEBUG ("slimmer::commit...[done]");
}

/** @brief restore object's state as before slimming was applied
 */
void
AthExFatObjectSlimmer::rollback()
{
  ATHEX_DEBUG ("slimmer::rollback...");
  // put back the original content...
  m_object->m_particle     = m_particle;     // ptr assignment...
  m_object->m_particleLink = m_particleLink; // invokes assignement operator.
  ATHEX_DEBUG ("slimmer::rollback...[done]");
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


