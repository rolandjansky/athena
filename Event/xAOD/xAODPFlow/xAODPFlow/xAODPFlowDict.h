// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODPFLOW_XAODPFODICT_H
#define XAODPFLOW_XAODPFODICT_H

// Local include(s):
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/versions/PFO_v1.h"
#include "xAODPFlow/versions/PFOContainer_v1.h"
#include "xAODPFlow/versions/PFOAuxContainer_v1.h"

#include "xAODPFlow/TrackCaloCluster.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"
#include "xAODPFlow/versions/TrackCaloCluster_v1.h"
#include "xAODPFlow/versions/TrackCaloClusterContainer_v1.h"
#include "xAODPFlow/versions/TrackCaloClusterAuxContainer_v1.h"
#include "xAODPFlow/versions/FlowElementContainer_v1.h"
#include "xAODPFlow/versions/FlowElementAuxContainer_v1.h"

#include "xAODPFlow/PFODefs.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"
#include "AthLinks/ElementLink.h"

// System include(s).
#include <utility>
#include <vector>

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODFLOW {
     // Local type(s).
     XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, PFOContainer_v1 );
     XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackCaloClusterContainer_v1 );
     XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, FlowElementContainer_v1 );
     // Type(s) needed for the dictionary generation to succeed.
     XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, IParticleContainer );
     // Weird/bad types used by the PFO reconstruction as attributes on
     // xAOD::PFO objects. :-(
     std::pair< ElementLink< xAOD::CaloClusterContainer >, double > dummy1;
     std::vector< std::pair< ElementLink< xAOD::CaloClusterContainer >, double > > dummy2;
     std::vector< std::vector<ElementLink<xAOD::FlowElementContainer_v1>, double >> dummy3; // fixes issues in eflowRec
     std::vector< std::vector< std::pair< ElementLink< xAOD::CaloClusterContainer >, double > >  > dummy4;
     std::vector< xAOD::PFODetails::PFOLeptonType > dummy5;


   };
}

#endif // XAODPFLOW_XAODPFODICT_H
