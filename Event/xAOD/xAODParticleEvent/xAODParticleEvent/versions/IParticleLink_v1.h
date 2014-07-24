// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleLink_v1.h 599909 2014-06-02 14:25:44Z kkoeneke $
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
// #ifndef XAOD_STANDALONE
// #include "SGTools/CLASS_DEF.h"
// CLASS_DEF( xAOD::IParticleLink_v1, 108878087, 1 )
// #endif // not XAOD_STANDALONE

#endif // XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H
