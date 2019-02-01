/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/IEventObjects.h>

#include <QuickAna/xAODHelpers.h>

//
// method implementations
//

namespace ana
{
  xAOD::IParticleContainer *IEventObjects ::
  getParticles (ObjectType type)
  {
    return xAODHelpers::getParticles (getVoid (type), type);
  }
}
