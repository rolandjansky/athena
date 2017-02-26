// Dear emacs, this is -*- c++ -*-
// $Id: xAODTrackCaloClusterDict.h $
#ifndef XAODTRACKCALOCLUSTER_XAODTRACKCALOCLUSTERDICT_H
#define XAODTRACKCALOCLUSTER_XAODTRACKCALOCLUSTERDICT_H

// System include(s):
#include <vector>
 
// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
 
// Local include(s):
#include "xAODTrackCaloCluster/versions/TrackCaloClusterContainer_v1.h"
#include "xAODTrackCaloCluster/versions/TrackCaloClusterAuxContainer_v1.h"
 
namespace {
  struct GCCXML_DUMMY_INSTANTIATION {
    
     xAOD::TrackCaloClusterContainer_v1                                              c1;
     DataLink< xAOD::TrackCaloClusterContainer_v1 >                                  l1;
     ElementLink< xAOD::TrackCaloClusterContainer_v1 >                               l2;
     ElementLinkVector< xAOD::TrackCaloClusterContainer_v1 >                         l3;
     std::vector< DataLink< xAOD::TrackCaloClusterContainer_v1 > >                   l4;
     std::vector< ElementLink< xAOD::TrackCaloClusterContainer_v1 > >                l5;
     std::vector< ElementLinkVector< xAOD::TrackCaloClusterContainer_v1 > >          l6;
     std::vector< std::vector< ElementLink< xAOD::TrackCaloClusterContainer_v1 > > > l7;
   
  };
}

#endif // XAODTRACKCALOCLUSTER_XAODTRACKCALOCLUSTERDICT_H