// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODParticleEventDict.h 649922 2015-02-26 12:20:50Z kkoeneke $
#ifndef XAODPARTICLEEVENT_XAODPARTICLEEVENTDICT_H
#define XAODPARTICLEEVENT_XAODPARTICLEEVENTDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__


// STL
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleAuxContainer.h"


namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODPARTICLEEVENT {
    xAOD::IParticleLinkContainer_v1                                              ipl_c1;
    DataLink< xAOD::IParticleLinkContainer_v1 >                                  ipl_l1;
    ElementLink< xAOD::IParticleLinkContainer_v1 >                               ipl_l2;
    std::vector< DataLink< xAOD::IParticleLinkContainer_v1 > >                   ipl_l3;
    std::vector< ElementLink< xAOD::IParticleLinkContainer_v1 > >                ipl_l4;
    std::vector< std::vector< ElementLink< xAOD::IParticleLinkContainer_v1 > > > ipl_l5;

    xAOD::ParticleContainer_v1                                              p_c1;
    DataLink< xAOD::ParticleContainer_v1 >                                  p_l1;
    ElementLink< xAOD::ParticleContainer_v1 >                               p_l2;
    std::vector< DataLink< xAOD::ParticleContainer_v1 > >                   p_l3;
    std::vector< ElementLink< xAOD::ParticleContainer_v1 > >                p_l4;
    std::vector< std::vector< ElementLink< xAOD::ParticleContainer_v1 > > > p_l5;

    xAOD::CompositeParticleContainer_v1                                              cp_c1;
    DataLink< xAOD::CompositeParticleContainer_v1 >                                  cp_l1;
    ElementLink< xAOD::CompositeParticleContainer_v1 >                               cp_l2;
    std::vector< DataLink< xAOD::CompositeParticleContainer_v1 > >                   cp_l3;
    std::vector< ElementLink< xAOD::CompositeParticleContainer_v1 > >                cp_l4;
    std::vector< std::vector< ElementLink< xAOD::CompositeParticleContainer_v1 > > > cp_l5;

    // // Smart pointers needed for the correct generation of the auxiliary
    // // class dictionaries:
    // std::vector< xAOD::IParticleLinkContainer > el1;
    // ElementLink< xAOD::IParticleContainer > el7;
    // std::vector< ElementLink< xAOD::IParticleContainer > > el8;
    // std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > el9;

  };
}

#endif // XAODPARTICLEEVENT_XAODPARTICLEEVENTDICT_H
