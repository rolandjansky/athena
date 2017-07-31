/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleCellAssociationAlg.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODAssociations/TrackParticleClusterAssociation.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationAuxContainer.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "TrackToCalo/CrossedCaloCellHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

TrackParticleCellAssociationAlg::TrackParticleCellAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
  m_trackSelector("InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool") {  

  declareProperty("ParticleCaloCellAssociationTool",m_caloCellAssociationTool);
  declareProperty("TrackParticleContainerName",m_trackParticleCollectionName = "InDetTrackParticles" );
  declareProperty("PtCut", m_ptCut = 25000. );
  declareProperty("OutputCollectionPostFix", m_outputPostFix = "" );
}

TrackParticleCellAssociationAlg::~TrackParticleCellAssociationAlg()
{

}

StatusCode TrackParticleCellAssociationAlg::initialize()
{
  ATH_CHECK(m_caloCellAssociationTool.retrieve());
  // ATH_CHECK(m_trackSelector.retrieve());

  return StatusCode::SUCCESS; 
}

StatusCode TrackParticleCellAssociationAlg::execute()
{

  // get track particles
  const xAOD::TrackParticleContainer* trackParticles = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_trackParticleCollectionName)) {
    if(evtStore()->retrieve(trackParticles,m_trackParticleCollectionName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_trackParticleCollectionName );
      return StatusCode::FAILURE;
    }
  }else{
    // in case nothing is found return
    return StatusCode::SUCCESS;
  }

  // create strings for locations based on input track collection
  std::string clusterContainerName = m_trackParticleCollectionName + "AssociatedClusters" + m_outputPostFix;
  std::string associationContainerName = m_trackParticleCollectionName + "ClusterAssociations" + m_outputPostFix;

  // Create the xAOD container and its auxiliary store:
  xAOD::CaloClusterContainer* xaod = CaloClusterStoreHelper::makeContainer(&(*evtStore()),clusterContainerName,msg());
  if ( !xaod ) {
    ATH_MSG_WARNING ("makeContainer failed");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Recorded CaloClusterContainer with key: " << clusterContainerName );    

  // Create the xAOD container and its auxiliary store:
  xAOD::TrackParticleClusterAssociationContainer* xaoda = new xAOD::TrackParticleClusterAssociationContainer();
  ATH_CHECK( evtStore()->record( xaoda, associationContainerName ) );

  xAOD::TrackParticleClusterAssociationAuxContainer* auxa = new xAOD::TrackParticleClusterAssociationAuxContainer();
  ATH_CHECK( evtStore()->record( auxa, associationContainerName + "Aux." ) );
  xaoda->setStore( auxa );
  ATH_MSG_DEBUG( "Recorded TrackParticleClusterAssociationContainer with key: " << associationContainerName );    

  unsigned int ntracks = 0;
  for( unsigned int i=0;i<trackParticles->size();++i ){

    // slect track
    const xAOD::TrackParticle* tp = (*trackParticles)[i];
    // if( !m_trackSelector->decision(*tp) || tp->pt() < m_ptCut ) continue;
    if( tp->pt() < m_ptCut ) continue;

    // get ParticleCellAssociation
    ATH_MSG_DEBUG(" Selected track: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() );
    const Rec::ParticleCellAssociation* association = 0;
    if( !m_caloCellAssociationTool->particleCellAssociation(*tp,association,0.1) ){
      ATH_MSG_DEBUG("failed to obtain the ParticleCellAssociation");
      continue;
    }

    // require container as it should be there
    if( !association->container() ){
      ATH_MSG_WARNING("Failed to obtain CaloCellContainer from ParticleCellAssociation");
      continue;
    }

    // create cell from ParticleCellAssociation
    xAOD::CaloCluster* cluster = Rec::CrossedCaloCellHelper::crossedCells( *association,*association->container(), *xaod );
    if( !cluster ){
      ATH_MSG_WARNING("Failed to create cluster from ParticleCellAssociation");
      continue;
    }else{
      ATH_MSG_DEBUG(" New cluster: eta " << cluster->eta() << " phi " << cluster->phi() << " cells " << cluster->size() << " nclusters " << xaod->size() );
    }

    // create element links
    ElementLink< xAOD::TrackParticleContainer > trackLink(m_trackParticleCollectionName,i);
    ElementLink< xAOD::CaloClusterContainer >   clusterLink(clusterContainerName,xaod->size()-1);

    // if valid create TrackParticleClusterAssociation
    if( trackLink.isValid() && clusterLink.isValid() ){
      xAOD::TrackParticleClusterAssociation* trackAssociation = new xAOD::TrackParticleClusterAssociation();
      xaoda->push_back(trackAssociation);
      trackAssociation->setTrackParticleLink( trackLink );
      std::vector< ElementLink< xAOD::CaloClusterContainer > > caloClusterLinks;
      caloClusterLinks.push_back( clusterLink );
      trackAssociation->setCaloClusterLinks(caloClusterLinks);
      ATH_MSG_DEBUG("added association");
      ++ntracks;
    }else{
      if( !trackLink.isValid() )   ATH_MSG_WARNING("Failed to create track ElementLink ");
      if( !clusterLink.isValid() ) ATH_MSG_WARNING("Failed to create cluster ElementLink ");
    }
  }

  ATH_MSG_DEBUG(" Total number of selected tracks: " << ntracks );

  if (CaloClusterStoreHelper::finalizeClusters(&(*evtStore()), xaod,clusterContainerName,msg()).isFailure() ) 
    ATH_MSG_WARNING("finalizeClusters failed");

  return StatusCode::SUCCESS;
}


StatusCode TrackParticleCellAssociationAlg::finalize()
{
  return StatusCode::SUCCESS;
}
