/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleCellAssociationAlg.h"


#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "TrackToCalo/CrossedCaloCellHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

TrackParticleCellAssociationAlg::TrackParticleCellAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool", this) {

  declareProperty("ParticleCaloCellAssociationTool",m_caloCellAssociationTool);
  declareProperty("PtCut", m_ptCut = 25000. );
}

TrackParticleCellAssociationAlg::~TrackParticleCellAssociationAlg()
{

}

StatusCode TrackParticleCellAssociationAlg::initialize()
{
  ATH_CHECK(m_caloCellAssociationTool.retrieve());

  ATH_CHECK(m_trackParticleCollectionName.initialize());
  ATH_CHECK(m_clusterContainerName.initialize());
  ATH_CHECK(m_associationContainerName.initialize());
  ATH_CHECK(m_clusterCellLinkName.initialize());
  return StatusCode::SUCCESS; 
}

StatusCode TrackParticleCellAssociationAlg::execute()
{
  // get track particles
  SG::ReadHandle<xAOD::TrackParticleContainer> trackParticles{m_trackParticleCollectionName};

  // Create the xAOD container and its auxiliary store:
  SG::WriteHandle<xAOD::CaloClusterContainer> clusColl( m_clusterContainerName /*,ctx*/);
  ATH_CHECK(CaloClusterStoreHelper::AddContainerWriteHandle(clusColl));

  
  SG::WriteHandle<xAOD::TrackParticleClusterAssociationContainer> xaoda( m_associationContainerName/*,ctx*/);
  ATH_CHECK(xaoda.record (std::make_unique<xAOD::TrackParticleClusterAssociationContainer>(),
			  std::make_unique<xAOD::TrackParticleClusterAssociationAuxContainer>()));

  

  unsigned int ntracks = 0;
  for( unsigned int i=0;i<trackParticles->size();++i ){

    // slect track
    const xAOD::TrackParticle* tp = (*trackParticles)[i];
    if( tp->pt() < m_ptCut ) continue;

    // get ParticleCellAssociation
    ATH_MSG_DEBUG(" Selected track: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() );
    std::unique_ptr<const Rec::ParticleCellAssociation> association
      =m_caloCellAssociationTool->particleCellAssociation(*tp,0.1);
    if(!association){
      ATH_MSG_DEBUG("failed to obtain the ParticleCellAssociation");
      continue;
    }

    // require container as it should be there
    if( !association->container() ){
      ATH_MSG_WARNING("Failed to obtain CaloCellContainer from ParticleCellAssociation");
      continue;
    }

    // create cell from ParticleCellAssociation
    xAOD::CaloCluster* cluster = Rec::CrossedCaloCellHelper::crossedCells( *association,
                                                                           *association->container(), 
                                                                           *clusColl );
    if( !cluster ){
      ATH_MSG_WARNING("Failed to create cluster from ParticleCellAssociation");
      continue;
    }else{
      ATH_MSG_DEBUG(" New cluster: eta " << cluster->eta() << " phi " << cluster->phi() << " cells " << cluster->size() << " nclusters " << clusColl->size() );
    }

    // create element links
    ElementLink< xAOD::TrackParticleContainer > trackLink(m_trackParticleCollectionName.key(),i);
    ElementLink< xAOD::CaloClusterContainer >   clusterLink(m_clusterContainerName.key(),clusColl->size()-1);

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

  SG::WriteHandle<CaloClusterCellLinkContainer> cellLinks (m_clusterCellLinkName /*, ctx*/);
  ATH_CHECK(CaloClusterStoreHelper::finalizeClusters (cellLinks,
                                                      clusColl.ptr()));
  return StatusCode::SUCCESS;
}


StatusCode TrackParticleCellAssociationAlg::finalize()
{
  return StatusCode::SUCCESS;
}
