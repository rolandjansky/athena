/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleCaloClusterAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOCLUSTERASSOCIATIONTOOL_H
#define ITRKTRACKPARTICLECALOCLUSTERASSOCIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "ParticleCaloExtension/ParticleClusterAssociation.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


namespace Rec 
{
  

  static const InterfaceID IID_IParticleCaloClusterAssociationTool("Rec::IParticleCaloClusterAssociationTool", 1, 0);

  /** @brief Interface for extending Particles with calo cells */
  class IParticleCaloClusterAssociationTool : virtual public IAlgTool {
  public:

    /** Method to get the ParticleClusterAssociation of a given TrackParticle
        @param particle       input particle
        @param extension      reference to a pointer to a ParticleClusterAssociation, will be updated if call is successfull
                              NEVER delete the pointer, you will cause a crash! 
        @param dr             cone size used for the association
                              If caching is enabled, the cells associated to the association contain at least all cells
                              in dr but could contain more. Users ALWAYS have to recalculate the associated cells in their cone.
        @param container      cluster container to be used if provided 
        @param useCaching     configure whether the tool caches the result on the track particle
                              The default behavior is 'true' to ensure optimal performance
                              If caching is enabled, the code will perform a consistency check on the container pointer
                              If the function is called twice on the same particle with different containers, the call will fail.
                              The same is true if the function is called once without container and once with on the same particle.
        @return true if the call was successful
    */
    virtual bool particleClusterAssociation( const xAOD::IParticle& particle,  std::vector< ElementLink< xAOD::CaloClusterContainer > >& association, float dr, 
                                             const xAOD::CaloClusterContainer* container = 0) const = 0;

    static const InterfaceID& interfaceID( ) ;
  };

  inline const InterfaceID& IParticleCaloClusterAssociationTool::interfaceID() {
    return IID_IParticleCaloClusterAssociationTool; 
  }

} // end of namespace

#endif 
