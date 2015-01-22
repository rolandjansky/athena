// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleLinkContainer.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_IPARTICLELINKCONTAINER_H
#define XAODPARTICLEEVENT_IPARTICLELINKCONTAINER_H

// Local include(s):
//#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/versions/IParticleLinkContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the IParticleLinkContainer class
   typedef IParticleLinkContainer_v1 IParticleLinkContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::IParticleLinkContainer, 1339976047, 1 )

#endif // XAODPARTICLEEVENT_IPARTICLELINKCONTAINER_H
