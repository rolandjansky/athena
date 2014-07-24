// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleLinkContainer_v1.h 599909 2014-06-02 14:25:44Z kkoeneke $
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

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::IParticleLinkContainer_v1, 1339976047, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODPARTICLEEVENT_VERSIONS_IPARTICLELINKCONTAINER_V1_H
