/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloExtension/ParticleClusterAssociation.h"


namespace Rec {

  ParticleClusterAssociation::ParticleClusterAssociation( const Trk::CaloExtension& caloExtension, Data&& clusters, 
                                                          float coneSize, const xAOD::CaloClusterContainer* container ) :
    ParticleCaloAssociation< const xAOD::CaloCluster* >(caloExtension,std::move(clusters),coneSize),
    m_container(container) {
  }

}
