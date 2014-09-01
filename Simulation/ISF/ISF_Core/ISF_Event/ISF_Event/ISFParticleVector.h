/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFParticleVector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
#ifndef ISF_EVENT_ISFPARTICLEVECTOR_H
#define ISF_EVENT_ISFPARTICLEVECTOR_H

// STL includes
#include <vector>

// forward declarations
namespace ISF {
  class ISFParticle;
}

namespace ISF {
  /** ISFParticle vector */
  typedef std::vector<ISF::ISFParticle *>             ISFParticleVector;
  typedef std::vector<const ISF::ISFParticle *>       ConstISFParticleVector;
}

#endif // ISF_EVENT_ISFPARTICLEVECTOR_H

