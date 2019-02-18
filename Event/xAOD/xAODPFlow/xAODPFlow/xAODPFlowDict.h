/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Class to set what goes in the dictionary  - authors M. Hodgkinson amd M. Janus */

#ifndef XAODPFLOW_XAODPFODICT_H
#define XAODPFLOW_XAODPFODICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/versions/PFOContainer_v1.h"
#include "xAODPFlow/versions/PFOAuxContainer_v1.h"
#include "xAODPFlow/versions/TrackCaloClusterContainer_v1.h"
#include "xAODPFlow/versions/TrackCaloClusterAuxContainer_v1.h"
#include "xAODPFlow/PFODefs.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODPFLOW {
    xAOD::PFOContainer_v1                                                           c1;
    DataLink< xAOD::PFOContainer_v1 >                                               l1;
    ElementLink< xAOD::PFOContainer_v1 >                                            l2;
    std::vector< DataLink< xAOD::PFOContainer_v1 > >                                l4;
    std::vector< ElementLink< xAOD::PFOContainer_v1 > >                             l5;
    std::vector< std::vector< ElementLink< xAOD::PFOContainer_v1 > > >              l6;
                                                                                    
    xAOD::IParticleContainer                                                        c2;
    ElementLink< xAOD::IParticleContainer >                                         l8;
    std::vector< ElementLink<xAOD::IParticleContainer > >                           l9;
    std::vector< std::vector< ElementLink<xAOD::IParticleContainer > > >            l10;
    std::vector< xAOD::PFODetails::PFOLeptonType >                                  l11;
    ElementLink< xAOD::VertexContainer >                                            l12;
    
    xAOD::TrackCaloClusterContainer_v1                                              c3;
    DataLink< xAOD::TrackCaloClusterContainer_v1 >                                  l13;
    std::vector< DataLink< xAOD::TrackCaloClusterContainer_v1 > >                   l14;
    ElementLink< xAOD::TrackCaloClusterContainer_v1 >                               l15;
    std::vector< ElementLink< xAOD::TrackCaloClusterContainer_v1 > >                l16;
    std::vector< std::vector< ElementLink< xAOD::TrackCaloClusterContainer_v1 > > > l17;
    
   };
}

#endif // XAODPFLOW_XAODPFODICT_H
