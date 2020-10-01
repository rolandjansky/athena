/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
ParticleCaloClusterAssociationTool.h  -  Description
-------------------
begin   : Summer 2014
authors : Niels van Eldik (CERN PH-ATC)
***************************************************************************/
#ifndef TRACKTOCALO_PARTICLECALOCLUSTERASSOCIATION_H
#define TRACKTOCALO_PARTICLECALOCLUSTERASSOCIATION_H

#include "RecoToolInterfaces/IParticleCaloClusterAssociationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "ParticleCaloExtension/ParticleClusterAssociation.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "CxxUtils/checker_macros.h"
namespace Trk {
  class CaloExtension;
}
namespace xAOD {
  class ICaloClustersInConeTool;
}
namespace Rec {

  /* Mark as not ATLAS_NOT_THREAD_SAFE
   * as it still uses the old style caching
   * to be updated if needed
   */
  class  ParticleCaloClusterAssociationTool :
    virtual public IParticleCaloClusterAssociationTool, public AthAlgTool {
  public:
    
    ParticleCaloClusterAssociationTool(const std::string&,const std::string&,const IInterface*);

    virtual ~ParticleCaloClusterAssociationTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

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
    bool particleClusterAssociation( const xAOD::IParticle& particle,  const ParticleClusterAssociation*& association, float dr, 
                                     const xAOD::CaloClusterContainer* container = 0, bool useCaching = true ) const final;
    
  private:


    void associateClusters( const xAOD::CaloClusterContainer* container, 
                            const Trk::CaloExtension& caloExtension,
                            float dr,
                            ParticleClusterAssociation::Data& clusters,
                            const xAOD::IParticle& particle) const;
    
    const xAOD::CaloClusterContainer* getClusterContainer() const;

    ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
    // FIXME: mutable
    ToolHandle< xAOD::ICaloClustersInConeTool >     m_clustersInConeTool;
    std::string m_caloClusters;
    std::string m_assCollection;
    std::string m_caloEntryMapName;
    double      m_coneSize;
    bool        m_useCovariance;

  };


}

#endif
