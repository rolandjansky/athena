///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObject.cxx 
// Implementation file for class AthExFatObject
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExThinning includes
#include "AthExThinning/AthExFatObject.h"

// STL includes


typedef AthExFatObject::AthExParticleLink_t AthExParticleLink_t;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthExFatObject::AthExFatObject() :
  m_particle (0),
  m_particleLink()
{}

AthExFatObject::AthExFatObject (const AthExFatObject& rhs) :
  m_particle (rhs.m_particle),
  m_particleLink (rhs.m_particleLink)
{}

AthExFatObject& 
AthExFatObject::operator= (const AthExFatObject& rhs) 
{
  if (this != &rhs) {
    m_particle = rhs.m_particle;
    m_particleLink = rhs.m_particleLink;
  }
  return *this;
}

AthExFatObject::AthExFatObject (const AthExParticleLink_t& link) :
  m_particle (*link),
  m_particleLink (link)
{}

// Destructor
///////////////
AthExFatObject::~AthExFatObject()
{
  m_particle = 0;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void
AthExFatObject::setLink (const AthExParticleLink_t& link)
{
  m_particle = *link;
  m_particleLink = link;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


