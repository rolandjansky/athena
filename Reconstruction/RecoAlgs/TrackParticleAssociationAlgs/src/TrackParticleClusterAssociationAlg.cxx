/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleClusterAssociationAlg.h"

#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/WriteDecorHandle.h"

#include "FourMomUtils/P4Helpers.h"

#include "AthContainers/AuxElement.h"


TrackParticleClusterAssociationAlg::TrackParticleClusterAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator), m_doDetEta{false} {  }

StatusCode TrackParticleClusterAssociationAlg::initialize()
{

  if (m_detectorEtaDecor.key().empty())
      m_doDetEta = false;
  else
  {
      m_doDetEta = true;
      m_detectorEtaDecor = m_caloClusters.key() + "." + m_detectorEtaDecor.key();
  }
  
  ATH_CHECK( m_caloExtKey.initialize() );
  ATH_CHECK( m_trackParticleCollectionHandle.initialize() );
  ATH_CHECK( m_caloClusters.initialize() );
  ATH_CHECK( m_detectorEtaDecor.initialize(m_doDetEta) );
  ATH_CHECK( m_assocClustersDecor.initialize() );

  ATH_CHECK(m_caloEntryParsDecor.initialize( !m_caloEntryParsDecor.empty() ) );

  ATH_CHECK( m_vertexContHandle.initialize( !m_vertexContHandle.empty() ) );
  if(!m_vertexContHandle.empty() ) {
    ATH_CHECK(m_trackvertexassoTool.retrieve() );
  }

  ATH_MSG_DEBUG(" cluster decoration = "<< m_assocClustersDecor.key() );
  return StatusCode::SUCCESS; 
}

StatusCode TrackParticleClusterAssociationAlg::execute()
{

  ATH_MSG_DEBUG("excute()");
  // get track particles
  SG::ReadHandle<xAOD::TrackParticleContainer> trackParticles(m_trackParticleCollectionHandle);  
  ATH_MSG_DEBUG("retrieved "<< m_trackParticleCollectionHandle.key()<< " size ="<< trackParticles->size() );
  
  // pre-calculate a width of clusters, set it as dynamica attribute so we don't have to recalculate it
  SG::ReadHandle<xAOD::CaloClusterContainer> clusterContainer(m_caloClusters);
  ATH_MSG_DEBUG("retrieved "<< m_caloClusters.key() << " size = "<< clusterContainer->size() );
  static SG::AuxElement::Decorator<float> sig_dec("sigmaWidth");
  //for(const xAOD::CaloCluster *cl : *clusterContainer){
  for(const xAOD::CaloCluster *cl : *clusterContainer){
    double rad;
    cl->retrieveMoment(xAOD::CaloCluster::SECOND_R,rad);
    double cent;
    cl->retrieveMoment(xAOD::CaloCluster::CENTER_MAG,cent);
    
    float cl_eta {99};
    if (m_doDetEta)
    {
        SG::ReadDecorHandle<xAOD::CaloClusterContainer, float> detEta(m_detectorEtaDecor);
        cl_eta = detEta(*cl);
    }
    else
        cl_eta = cl->eta();
    
    double sigmaWidth = atan(sqrt(rad)/cent)*cosh(cl_eta);
    sig_dec(*cl) = sigmaWidth;
  }


  // obtain the CaloExtension from the map in the event store
  SG::ReadHandle<CaloExtensionCollection> caloExts( m_caloExtKey );
  ATH_MSG_DEBUG("CaloExtensionCollection "<< m_caloExtKey.key() << "  : size="<< caloExts->size() );

  const xAOD::Vertex * pv0 = nullptr;
  if(!m_vertexContHandle.empty()){
    SG::ReadHandle<xAOD::VertexContainer> vxCont(m_vertexContHandle);
    if(!vxCont->empty()) pv0=(*vxCont)[0]; // Hard code HS vertex as PV0
  }

  SG::WriteDecorHandle<xAOD::TrackParticleContainer,
		       std::vector<ElementLink<xAOD::CaloClusterContainer>> > assoClustDecor(m_assocClustersDecor);
    
  ATH_MSG_DEBUG("will decorate with "<<assoClustDecor.key()<< " and adding trkParam : "<< m_caloEntryParsDecor.key()  );

  // ******************************************
  // main loop over tracks 
  unsigned int ntracks = 0;
  for( const xAOD::TrackParticle* tp : *trackParticles){


    if( tp->pt() < m_ptCut ) continue;

    if( pv0 != nullptr) if(! m_trackvertexassoTool->isCompatible(*tp, *pv0 )) continue;
    
    ATH_MSG_DEBUG(" Selected track " << tp->index() << "  pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() );


    // IMMPORTANT : this assumes a correspondance between the TrackParticleContainer and the CaloExtensionCollection !
    const Trk::CaloExtension * caloExtension = (*caloExts)[tp->index() ] ;
    if (caloExtension == nullptr ) {
      ATH_MSG_DEBUG(" Selected track "<< tp->index() << " has no caloExtension ");
      continue;
    }


    // build the associated clusters
    std::vector<const xAOD::CaloCluster*> assoClusters = associatedClusters( *caloExtension, *clusterContainer); 


    // retrieve the vector of links to cluster (and creating it )
    std::vector< ElementLink< xAOD::CaloClusterContainer > > & caloClusterLinks = assoClustDecor(*tp);
    // translate in ElementLink
    caloClusterLinks.reserve( assoClusters.size() );
    for(const xAOD::CaloCluster* cluster : assoClusters) caloClusterLinks.emplace_back( *clusterContainer,cluster->index() );
    ntracks++;     
  }// end loop over tracks

  // 2nd loop over track, only to decorate with Track parameter if requested.
  if (! m_caloEntryParsDecor.empty() ){
    // we can not do this in the above loop because declaring a WriteDecorHandle requires a non empty key
    //  (otherwise : run-time error).
    SG::WriteDecorHandle<xAOD::TrackParticleContainer,  const Trk::TrackParameters*> trkParamDecor( m_caloEntryParsDecor );
    for( const xAOD::TrackParticle* tp : *trackParticles){
      const Trk::CaloExtension * caloExtension = (*caloExts)[tp->index() ] ;
      if (caloExtension == nullptr ) trkParamDecor( *tp ) =  nullptr ;
      else trkParamDecor( *tp ) = caloExtension->caloEntryLayerIntersection();      
    }

  }
  
  ATH_MSG_DEBUG(" Total number of selected tracks: " << ntracks );

  return StatusCode::SUCCESS;
}


std::vector<const xAOD::CaloCluster* > TrackParticleClusterAssociationAlg::associatedClusters(const Trk::CaloExtension & caloExtension, const xAOD::CaloClusterContainer & allClusters ){

  std::vector<const xAOD::CaloCluster* > clusters;

  const Trk::TrackParameters*  pars = caloExtension.caloEntryLayerIntersection();
  if(!pars) {
    ATH_MSG_WARNING( " NO TrackParameters caloExtension.caloEntryLayerIntersection() ");
    return clusters;
  } 
    
  float eta = pars->position().eta();
  float phi = pars->position().phi();

  double uncertEta = 0.;
  double uncertPhi = 0.;
  if(pars->covariance()) {
    uncertEta = -2.*sin(pars->position().theta()) / (cos(2.*pars->position().theta())-1.) * sqrt((*pars->covariance())(Trk::theta,Trk::theta));
    uncertPhi = sqrt((*pars->covariance())(Trk::phi,Trk::phi));
  } 
  double uncertExtrp = uncertEta*uncertEta + uncertPhi*uncertPhi;

  float dr2Cut0 = m_dr*m_dr;
  // to access the pre-calculated width :
  static SG::AuxElement::ConstAccessor<float> sig_acc("sigmaWidth");

  for(const xAOD::CaloCluster * cl : allClusters){

    float dPhi = P4Helpers::deltaPhi( cl->phi(), phi);
    
    float cl_eta {99};
    if (m_doDetEta)
    {
        SG::ReadDecorHandle<xAOD::CaloClusterContainer, float> detEta(m_detectorEtaDecor);
        cl_eta = detEta(*cl);
    }
    else
        cl_eta = cl->eta();

    float dEta = cl_eta - eta;
    float dr2  = dPhi*dPhi+ dEta*dEta;
    float dr2Cut = dr2Cut0;
    
    if(m_useCovariance) {                        
      
      double sigmaWidth = sig_acc(*cl);          
      double uncertClus  = 2.*sigmaWidth*sigmaWidth;
      if(uncertExtrp>uncertClus){
	ATH_MSG_DEBUG("Extrapolation uncertainty larger than cluster width! Returning without association.");
	continue;
      }
      
      dr2Cut = (sigmaWidth+uncertEta)*(sigmaWidth+uncertEta)+(sigmaWidth+uncertPhi)*(sigmaWidth+uncertPhi);   
    }
    if( dr2 < dr2Cut ) clusters.push_back( cl );    
  }

  return clusters;
}
