/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleClusterAssociationAlg.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODAssociations/TrackParticleClusterAssociation.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationAuxContainer.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "RecoToolInterfaces/IParticleCaloClusterAssociationTool.h"
#include "TrackToCalo/CrossedCaloCellHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

TrackParticleClusterAssociationAlg::TrackParticleClusterAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_caloClusterAssociationTool("Rec::ParticleCaloClusterAssociationTool/ParticleCaloClusterAssociationTool"),
  m_trackSelector("InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool") ,
  m_trackvertexassoTool()
{  

  declareProperty("ParticleCaloClusterAssociationTool"  ,   m_caloClusterAssociationTool);
  declareProperty("TrackParticleContainerName"          ,   m_trackParticleCollectionName = "InDetTrackParticles" );
  declareProperty("PtCut"                               ,   m_ptCut                       = 25000. );
  declareProperty("OutputCollectionPostFix"             ,   m_outputPostFix               = "" );
  declareProperty("CaloClusterLocation"                 ,   m_caloClusters                = "CaloCalTopoClusters"   );

  declareProperty("TrackVertexAssoTool" , m_trackvertexassoTool);
  declareProperty("VertexContainerName", m_vertexContname );
  declareProperty("UseDetectorEta",      m_useDetectorEta = false);
}

TrackParticleClusterAssociationAlg::~TrackParticleClusterAssociationAlg()
{

}

StatusCode TrackParticleClusterAssociationAlg::initialize()
{
  ATH_CHECK(m_caloClusterAssociationTool.retrieve());
  // ATH_CHECK(m_trackSelector.retrieve());

  if(m_trackvertexassoTool.typeAndName()!="") ATH_CHECK(m_trackvertexassoTool.retrieve() );
  
  return StatusCode::SUCCESS; 
}

StatusCode TrackParticleClusterAssociationAlg::execute()
{

  // get track particles
  const xAOD::TrackParticleContainer* trackParticles = 0;
  ATH_CHECK(evtStore()->retrieve(trackParticles, m_trackParticleCollectionName));


  // pre-calculate a width of clusters which will be re-used a lot :
  const xAOD::CaloClusterContainer* clustContainer = 0;  
  ATH_CHECK(evtStore()->retrieve(clustContainer, m_caloClusters));
  static SG::AuxElement::Decorator<float> sig_dec("sigmaWidth");
  for(const xAOD::CaloCluster *cl : *clustContainer){
    double rad;
    cl->retrieveMoment(xAOD::CaloCluster::SECOND_R,rad);
    double cent;
    cl->retrieveMoment(xAOD::CaloCluster::CENTER_MAG,cent);
    double cleta = cl->eta();
    if (m_useDetectorEta)
    {
        static SG::AuxElement::ConstAccessor<float> acc_det_eta ("DetectorEta");
        if (acc_det_eta.isAvailable(*cl))
            cleta = acc_det_eta(*cl);
    }
    double sigmaWidth = atan(sqrt(rad)/cent)*cosh(cleta);
    sig_dec(*cl) = sigmaWidth;
  }
  
  // create strings for locations based on input track collection
  std::string associationContainerName = m_trackParticleCollectionName + "ClusterAssociations" + m_outputPostFix;

  // Create the xAOD container and its auxiliary store:
  xAOD::TrackParticleClusterAssociationContainer* xaoda = new xAOD::TrackParticleClusterAssociationContainer();
  ATH_CHECK( evtStore()->record( xaoda, associationContainerName ) );

  xAOD::TrackParticleClusterAssociationAuxContainer* auxa = new xAOD::TrackParticleClusterAssociationAuxContainer();
  ATH_CHECK( evtStore()->record( auxa, associationContainerName + "Aux." ) );
  xaoda->setStore( auxa );
  ATH_MSG_DEBUG( "Recorded TrackParticleClusterAssociationContainer with key: " << associationContainerName );    


  const xAOD::Vertex * pv0 = nullptr;
  if(m_trackvertexassoTool.typeAndName()!=""){
    const xAOD::VertexContainer *vxCont=nullptr;
    ATH_CHECK( evtStore()->retrieve(vxCont, m_vertexContname) ); 
    if(!vxCont->empty()) pv0=(*vxCont)[0]; // Hard code HS vertex as PV0
  }
  
  unsigned int ntracks = 0;
  for( unsigned int i=0;i<trackParticles->size();++i ){

    // slect track
    const xAOD::TrackParticle* tp = (*trackParticles)[i];
    // if( !m_trackSelector->decision(*tp) || tp->pt() < m_ptCut ) continue;
    if( tp->pt() < m_ptCut ) continue;

    if( pv0 != nullptr) if(! m_trackvertexassoTool->isCompatible(*tp, *pv0 )) continue;
    
    // get ParticleCellAssociation
    ATH_MSG_DEBUG(" Selected track: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() );
    std::vector< ElementLink< xAOD::CaloClusterContainer > > caloClusterLinks;
    if( !m_caloClusterAssociationTool->particleClusterAssociation(*tp,caloClusterLinks,0.1) ){
      ATH_MSG_DEBUG("failed to obtain the ParticleClusterAssociation");
      continue;
    }
   
    // create element links
    //ElementLink< xAOD::TrackParticleContainer > trackLink(m_trackParticleCollectionName,i);
    ElementLink< xAOD::TrackParticleContainer > trackLink(*trackParticles,i);
    
    if( trackLink.isValid() && caloClusterLinks.size()!=0){
        xAOD::TrackParticleClusterAssociation* trackAssociation = new xAOD::TrackParticleClusterAssociation();
        xaoda->push_back(trackAssociation);
        trackAssociation->setTrackParticleLink( trackLink );
        trackAssociation->setCaloClusterLinks(caloClusterLinks);
        ATH_MSG_DEBUG("added association");
        ++ntracks;
    }
    else{
        if( !trackLink.isValid() )   ATH_MSG_WARNING("Failed to create track ElementLink ");
        if( caloClusterLinks.size()==0 ) ATH_MSG_VERBOSE("Failed to create cluster ElementLink - this is probably a trackonly TCC ");
    }
   
  }

  ATH_MSG_DEBUG(" Total number of selected tracks: " << ntracks );

  // if (CaloClusterStoreHelper::finalizeClusters(&(*evtStore()), xaod,clusterContainerName,msg()).isFailure() ) 
    // ATH_MSG_WARNING("finalizeClusters failed");

  return StatusCode::SUCCESS;
}


StatusCode TrackParticleClusterAssociationAlg::finalize()
{
  return StatusCode::SUCCESS;
}
