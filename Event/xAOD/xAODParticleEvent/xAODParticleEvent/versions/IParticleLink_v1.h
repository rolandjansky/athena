// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleLink_v1.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H
#define XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H


// xAOD include(s):
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"


namespace xAOD {

  /// Define the IParticleLink as a simple typedef
  typedef ElementLink< xAOD::IParticleContainer > IParticleLink_v1;

} // namespace xAOD

#endif // XAODPARTICLEEVENT_VERSIONS_IPARTICLELINK_V1_H
