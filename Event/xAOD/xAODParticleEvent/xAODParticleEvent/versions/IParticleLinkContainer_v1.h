// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleLinkContainer_v1.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_IPARTICLELINKCONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_IPARTICLELINKCONTAINER_V1_H

// STL includes
#include <vector>

// Local include(s):
#include "xAODParticleEvent/versions/IParticleLink_v1.h"

namespace xAOD {
  /// Define the container of IPartilceLinks
  // class IParticleLinkContainer_v1 : public std::vector< IParticleLink_v1 > {
  // }; // class IParticleLinkContainer_v1
  typedef std::vector< IParticleLink_v1 > IParticleLinkContainer_v1;
}

#endif // XAODPARTICLEEVENT_VERSIONS_IPARTICLELINKCONTAINER_V1_H
