/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleClusterAssociation_v1.cxx 636824 2014-12-18 16:56:34Z htorres $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODAssociations/versions/TrackParticleClusterAssociation_v1.h"

namespace xAOD {

  TrackParticleClusterAssociation_v1::TrackParticleClusterAssociation_v1()
    : SG::AuxElement() {
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackParticleClusterAssociation_v1, ElementLink< TrackParticleContainer >, trackParticleLink, setTrackParticleLink)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackParticleClusterAssociation_v1, std::vector< ElementLink< CaloClusterContainer > >, caloClusterLinks, setCaloClusterLinks)
  
} // namespace xAOD
