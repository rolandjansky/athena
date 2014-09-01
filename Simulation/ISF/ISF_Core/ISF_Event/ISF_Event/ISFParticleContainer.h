/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFParticleContainer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
#ifndef ISF_EVENT_ISFPARTICLECONTAINER_H
#define ISF_EVENT_ISFPARTICLECONTAINER_H

// STL includes
#include <list>

// forward declarations
namespace ISF {
  class ISFParticle;
}

namespace ISF {
  /** generic ISFParticle container (not necessarily a std::list!) */
  typedef std::list<ISF::ISFParticle*>                ISFParticleContainer;
  typedef std::list<const ISF::ISFParticle*>          ConstISFParticleContainer;
}

#endif // ISF_EVENT_ISFPARTICLECONTAINER_H

