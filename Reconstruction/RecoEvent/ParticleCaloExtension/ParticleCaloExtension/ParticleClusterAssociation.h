/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_PARTICLECLUSTERASSOCIATION_H
#define REC_PARTICLECLUSTERASSOCIATION_H

#include <vector>
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "ParticleCaloExtension/ParticleCaloAssociation.h"

namespace Trk {
  class CaloExtension;
}

namespace Rec {
  
  /** class storing calorimeter cell association with IParticle objects */
  class ParticleClusterAssociation  : public ParticleCaloAssociation< const xAOD::CaloCluster* > {
  public:
    /** constructor taking CaloExtension, a vector of cells and a cone size as arguments */
    ParticleClusterAssociation( const Trk::CaloExtension& caloExtension, Data&& clusters, float coneSize, const xAOD::CaloClusterContainer* container = 0);

    /** access to container */
    const xAOD::CaloClusterContainer* container() const;

  private:
    /** no default constructor */
    ParticleClusterAssociation() = delete;
    
    /** no copy constructor */
    ParticleClusterAssociation(const ParticleClusterAssociation&) = delete;

    /** no assignment operator */
    ParticleClusterAssociation& operator=(const ParticleClusterAssociation&) = delete;
    
    /** container */
    const xAOD::CaloClusterContainer* m_container;
  };

  inline const xAOD::CaloClusterContainer* ParticleClusterAssociation::container() const {
    return m_container;
  }

}

#endif
