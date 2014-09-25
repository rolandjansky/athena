// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleLink_v1.h 615672 2014-09-08 20:16:15Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H
#define XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H


// xAOD include(s):
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"


namespace xAOD {

  /// Define the IParticleLink as a simple typedef
  typedef ElementLink< xAOD::IParticleContainer > IParticleLink_v1;

} // namespace xAOD

// // Set up a CLID for the object:
// #include "xAODCore/CLASS_DEF.h"
// CLASS_DEF( xAOD::IParticleLink_v1, 108878087, 1 )

#endif // XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H
