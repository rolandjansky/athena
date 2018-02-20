/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleClipboard.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_Event/ParticleUserInformation.h"

/** constructor */
ISF::ParticleUserInformation::ParticleUserInformation() 
  : m_process(0)
  , m_generation(0)
  , m_matInfo(nullptr)
{
}

bool ISF::ParticleUserInformation::operator==(const ISF::ParticleUserInformation& rhs) const
{
  bool pass = true;
  pass &= m_process == rhs.process();
  pass &= m_generation == rhs.generation();

  {
    const auto rhsMatPtr = rhs.materialLimit();
    if (m_matInfo && rhsMatPtr) {
      pass &= *m_matInfo == *rhsMatPtr;
    } else {
      pass &= m_matInfo == rhsMatPtr; // must be both nullptr to pass
    }
  }
  return pass;
}
