/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloClusterAssociationTool.h"
// forward declares
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "ParticleCaloExtension/ParticleClusterAssociationCollection.h"
#include "ParticlesInConeTools/ICaloClustersInConeTool.h"

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "FourMomUtils/P4Helpers.h"

namespace Rec {

  ParticleCaloClusterAssociationTool::ParticleCaloClusterAssociationTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
      m_clustersInConeTool("xAOD::CaloClustersInConeTool/CaloClustersInConeTool")
  {

    declareInterface<IParticleCaloClusterAssociationTool>(this);
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
    declareProperty("ClustersInConeTool",          m_clustersInConeTool);

    //coneSize for including calo cells around track
    declareProperty("ConeSize", m_coneSize = 0.1);

  }

  ParticleCaloClusterAssociationTool::~ParticleCaloClusterAssociationTool() {}

  StatusCode ParticleCaloClusterAssociationTool::initialize() {
    /* Retrieve track extrapolator from ToolService */
    ATH_CHECK( m_caloExtensionTool.retrieve() );

    if (!m_clustersInConeTool.empty()) ATH_CHECK(m_clustersInConeTool.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode ParticleCaloClusterAssociationTool::finalize() {
    return StatusCode::SUCCESS;
  }

  bool ParticleCaloClusterAssociationTool::particleClusterAssociation( const xAOD::IParticle& particle,  const ParticleClusterAssociation*& association, float dr, 
                                                                       const xAOD::CaloClusterContainer* container, bool useCaching ) const {


    ATH_MSG_DEBUG(" particleCellAssociation: ptr " << &particle << " dr " << dr << " useCaching " << useCaching);

    // reset pointer
    association = 0;
    // check if link is already there
    if( useCaching ){
      if( particle.isAvailable< ParticleClusterAssociation* >("clusterAssociation") ){
        ParticleClusterAssociation* theAssociation = particle.auxdata< ParticleClusterAssociation* >("clusterAssociation");
        if( theAssociation ){
          // check whether the cached association is from the same container
          if( container && theAssociation->container() != container ){
            ATH_MSG_WARNING("Calling cached version with different container pointer");
            return false;
          }
          // check if we need to resize the cone
          if( dr > theAssociation->associationConeSize() ){
            ATH_MSG_DEBUG(" dr larger then cached dr: " << dr << " cached dr " << theAssociation->associationConeSize());
            ParticleClusterAssociation::Data clusters;
            associateClusters(container,theAssociation->caloExtension(),dr,clusters);    
            theAssociation->updateData(std::move(clusters),dr);
          }
          association = theAssociation;
          ATH_MSG_DEBUG("Found existing calo extension");
          return true;
        }
      }
    }

    // get the extrapolation into the calo
    const Trk::CaloExtension* caloExtension = 0;
    if( !m_caloExtensionTool->caloExtension(particle,caloExtension) ) {
      ATH_MSG_DEBUG("Failed to get calo extension");      
      return false;
    }
    if( caloExtension->caloLayerIntersections().empty()){
      ATH_MSG_DEBUG( "Received a caloExtension object without track extrapolation");
      return false;
    }
    

    // update cone size in case it is smaller than the default
    if( dr < m_coneSize ) dr = m_coneSize;
    ParticleClusterAssociation::Data clusters;
    associateClusters(container,*caloExtension,dr,clusters);    
    
    ParticleClusterAssociation* theAssocation = new ParticleClusterAssociation( *caloExtension, std::move(clusters), dr, container );

    // now add the extension to the output collection so we are not causing any leaks
    ParticleClusterAssociationCollection* collection = 0;
    if( !evtStore()->contains<ParticleClusterAssociationCollection>(m_containerName) ){
      collection = new ParticleClusterAssociationCollection();
      if( evtStore()->record( collection, m_containerName).isFailure() ) {
        ATH_MSG_WARNING( "Failed to record output collection, will leak the ParticleCaloExtension");
        delete collection;
        collection = 0;
      }
    }else{
      if(evtStore()->retrieve(collection,m_containerName).isFailure()) {
        ATH_MSG_WARNING( "Unable to retrieve " << m_containerName << " will leak the ParticleCaloExtension" );
      }
    }
    if( collection ) collection->push_back(theAssocation);
    else{
      ATH_MSG_WARNING( "No ParticleCaloCellAssociationCollection, failing extension to avoid memory leak");
      delete theAssocation;
      theAssocation = 0;
    }

    association = theAssocation;
    if( useCaching ) particle.auxdecor< ParticleClusterAssociation* >("clusterAssociation") = theAssocation;


    return true;

  }

  void ParticleCaloClusterAssociationTool::associateClusters( const xAOD::CaloClusterContainer* container, 
                                                              const Trk::CaloExtension& caloExtension,
                                                              float dr,
                                                              ParticleClusterAssociation::Data& clusters ) const {    

    const Trk::TrackParameters*  pars = caloExtension.caloEntryLayerIntersection();
    if(!pars) {
      ATH_MSG_WARNING( " NO TrackParameters caloExtension.caloEntryLayerIntersection() ");
      return;
    } 

    float eta = pars->position().eta();
    float phi = pars->position().phi();
    if( container ){
      float dr2Cut = dr*dr;
      for( unsigned int i=0;i<container->size();++i ){
        
        float dPhi = P4Helpers::deltaPhi( (*container)[i]->phi(), phi);
        float dEta = (*container)[i]->eta()-eta;
        float dr2  = dPhi*dPhi+ dEta*dEta;
        if( dr2 < dr2Cut ) clusters.push_back( ElementLink<xAOD::CaloClusterContainer>(*container,i) );
      }
    }else{
      if( !m_clustersInConeTool->particlesInCone(eta,phi,dr,clusters) ) {
        ATH_MSG_WARNING("Failed to get clusters");
      }
    }
    ATH_MSG_DEBUG("associated cells " << clusters.size() << " using cone " << dr );
  }

} // end of namespace Trk
