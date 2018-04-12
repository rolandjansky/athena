/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloClusterAssociationTool.h"
// forward declares

#include "ParticleCaloExtension/ParticleClusterAssociationCollection.h"
#include "ParticlesInConeTools/ICaloClustersInConeTool.h"

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "FourMomUtils/P4Helpers.h"

#include "TrackCaloClusterRecTools/IParticleToCaloExtensionMap.h"
#include "xAODTracking/VertexContainer.h"


namespace Rec {

  ParticleCaloClusterAssociationTool::ParticleCaloClusterAssociationTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
      m_clustersInConeTool("xAOD::CaloClustersInConeTool/CaloClustersInConeTool"),
      m_caloClusters("CaloCalTopoClusters"),
      m_caloEntryMapName("ParticleToCaloExtensionMap")
  {
    declareInterface<IParticleCaloClusterAssociationTool>(this);
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
    declareProperty("ClustersInConeTool",          m_clustersInConeTool);
    declareProperty("CaloClusterLocation",         m_caloClusters);
    declareProperty("AssociationCollectionName",   m_assCollection);
    declareProperty("ParticleCaloEntryMapName",    m_caloEntryMapName);
    //coneSize for including calo cells around track
    declareProperty("ConeSize",                    m_coneSize = 0.1);
    declareProperty("UseCovariance",               m_useCovariance = true);
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

  bool ParticleCaloClusterAssociationTool::particleClusterAssociation( const xAOD::IParticle& particle,  std::vector< ElementLink< xAOD::CaloClusterContainer > >& association, float dr, 
                                                                       const xAOD::CaloClusterContainer* container) const {


    ATH_MSG_DEBUG(" particleClusterAssociation: ptr " << &particle << " dr " << dr );

    // get the extrapolation into the calo
    const Trk::CaloExtension* caloExtension = 0;
    if( !m_caloExtensionTool->particleToCaloExtrapolate(particle,caloExtension) ) {
      ATH_MSG_DEBUG("Failed to get calo extension");      
      return false;
    }
    
    if( caloExtension->caloLayerIntersections().empty()){
      ATH_MSG_DEBUG( "Received a caloExtension object without track extrapolation");
      return false;
    }
    
    //retrieve the cluster container if not provided, return false it retrieval failed
    if( !container && !(container = getClusterContainer()) ) {
      ATH_MSG_DEBUG("Failed to get calo cluster container");      
      return false;
    }
    
    // update cone size in case it is smaller than the default
    if( dr < m_coneSize ) dr = m_coneSize;
    ParticleClusterAssociation::Data clusters;
    associateClusters(container,*caloExtension,dr,clusters,particle);    
    
    // now add particle and CaloExtension to the container
    IParticleToCaloExtensionMap * caloExtensionMap = 0;
    if(not evtStore()->contains<IParticleToCaloExtensionMap>(m_caloEntryMapName)) {
      caloExtensionMap = new IParticleToCaloExtensionMap();
      if (evtStore()->record(caloExtensionMap, m_caloEntryMapName).isFailure()) {
	ATH_MSG_WARNING( "Failed to record output collection, will leak the ParticleCaloExtension");
	delete caloExtensionMap;
	caloExtensionMap = 0;
      }
    } else {
      if(evtStore()->retrieve(caloExtensionMap,m_caloEntryMapName).isFailure()) {
	ATH_MSG_WARNING( "Unable to retrieve ParticleToCaloExtensionMap will leak the ParticleCaloExtension" );
      }
    }
    if (caloExtensionMap && caloExtension->caloEntryLayerIntersection()) {
        const Trk::CurvilinearParameters trkPar(caloExtension->caloEntryLayerIntersection()->position(),caloExtension->caloEntryLayerIntersection()->momentum(),caloExtension->caloEntryLayerIntersection()->charge());
        caloExtensionMap->addEntry(particle,trkPar);
    }
    
    delete caloExtension;
    
    for(auto cluster : clusters)
    {
        ElementLink< xAOD::CaloClusterContainer >   clusterLink(m_caloClusters,cluster->index());
        // if valid create TrackParticleClusterAssociation
        if( clusterLink.isValid() ){
            association.push_back( clusterLink );
        }
        ATH_MSG_DEBUG(" New cluster: eta " << cluster->eta() << " phi " << cluster->phi() );
    }

    return true;
  }
  
  void ParticleCaloClusterAssociationTool::associateClusters( const xAOD::CaloClusterContainer* container, 
                                                              const Trk::CaloExtension& caloExtension,
                                                              float dr,
                                                              ParticleClusterAssociation::Data& clusters, 
                                                              const xAOD::IParticle& particle) const {    

    particle.auxdecor<int>("ParticleCaloClusterAssociationTool")    = 1;

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
        
        if(m_useCovariance) {
            double uncertEta = 0.;
            double uncertPhi = 0.;
            if(pars->covariance()) {
              uncertEta = -2.*sin(pars->position().theta()) / (cos(2.*pars->position().theta())-1.) * sqrt((*pars->covariance())(Trk::theta,Trk::theta));
              uncertPhi = sqrt((*pars->covariance())(Trk::phi,Trk::phi));
            } 
            
            double rad;
            (*container)[i]->retrieveMoment(xAOD::CaloCluster::SECOND_R,rad);
            double cent;
            (*container)[i]->retrieveMoment(xAOD::CaloCluster::CENTER_MAG,cent);
            double sigmaWidth = atan(sqrt(rad)/cent)*cosh((*container)[i]->eta());
            double uncertExtrp = uncertEta*uncertEta + uncertPhi*uncertPhi;
            double uncertClus  = 2.*sigmaWidth*sigmaWidth;
	    	    
            if(uncertExtrp>uncertClus){
              ATH_MSG_DEBUG("Extrapolation uncertainty larger than cluster width! Returning without association.");
	      continue;
            }
            
            double dr2CutTmp = (sigmaWidth+uncertEta)*(sigmaWidth+uncertEta)+(sigmaWidth+uncertPhi)*(sigmaWidth+uncertPhi);
            
            if(sqrt(dr2)<sqrt(dr2Cut) && dr2 < dr2CutTmp) ATH_MSG_DEBUG("1. selections match! dR " << sqrt(dr2) << " new cut value " << sqrt(dr2CutTmp) << " pt trk " << pars->pT() << " sigma(phi) trk " << uncertPhi << " sigma(eta) trk " << uncertEta << " energy cluster " << (*container)[i]->e() << " sigma width " << sigmaWidth);
	    if(sqrt(dr2)<sqrt(dr2Cut) && dr2 > dr2CutTmp) ATH_MSG_DEBUG("2. only  dR matches! dR " << sqrt(dr2) << " new cut value " << sqrt(dr2CutTmp) << " pt trk " << pars->pT() << " sigma(phi) trk " << uncertPhi << " sigma(eta) trk " << uncertEta << " energy cluster " << (*container)[i]->e() << " sigma width " << sigmaWidth);
	    if(sqrt(dr2)>sqrt(dr2Cut) && dr2 < dr2CutTmp) ATH_MSG_DEBUG("3. only new matches! dR " << sqrt(dr2) << " new cut value " << sqrt(dr2CutTmp) << " pt trk " << pars->pT() << " sigma(phi) trk " << uncertPhi << " sigma(eta) trk " << uncertEta << " energy cluster " << (*container)[i]->e() << " sigma width " << sigmaWidth );           
            
	    dr2Cut = dr2CutTmp;
        }
        
        if( dr2 < dr2Cut )
	  clusters.push_back( (*container)[i]);
      }
    } else {
      ATH_MSG_DEBUG("We're at the clustersinconetool." );
      if( !m_clustersInConeTool->particlesInCone(eta,phi,dr,clusters) ) {
        ATH_MSG_WARNING("Failed to get clusters");
      }
    }
  }
  
  const xAOD::CaloClusterContainer* ParticleCaloClusterAssociationTool::getClusterContainer() const {

    const xAOD::CaloClusterContainer* container = 0;
    //retrieve the cell container
    if( evtStore()->retrieve(container, m_caloClusters).isFailure() || !container ) {
      ATH_MSG_WARNING( "Unable to retrieve the cluster container  " << m_caloClusters << " container ptr " << container );
      return 0;
    }
    if( container ) ATH_MSG_DEBUG("Retrieved cluster container " << container->size());
    return container;
  }

} // end of namespace Rec