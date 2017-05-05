#include "TrackCaloClusterRecTools/TrackCaloClusterCreatorTool.h"
#include "TrackCaloClusterRecTools/IParticleToCaloExtensionMap.h"
#include "TrkCaloExtension/CaloExtension.h"

#include "xAODTracking/VertexContainer.h"

#include "CxxUtils/sincos.h"
#include <cmath>

TrackCaloClusterCreatorTool::TrackCaloClusterCreatorTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
  m_loosetrackvertexassoTool("LooseTrackVertexAssociationTool"),
  m_caloEntryMapName("ParticleToCaloExtensionMap"),
  m_useEnergy(false),
  m_doOriginCorrection(true)
{
    declareProperty("VertexContainerName"          ,    m_vertexContname                  = "PrimaryVertices"   );
    declareProperty("ParticleCaloEntryMapName"     ,    m_caloEntryMapName                                      );
    declareProperty("LooseTrackVertexAssoTool"     ,    m_loosetrackvertexassoTool                              );
    declareProperty("UseEnergy"                    ,    m_useEnergy                                             );
    declareProperty("DoOriginCorrection"           ,    m_doOriginCorrection                                    );
}

TrackCaloClusterCreatorTool::~TrackCaloClusterCreatorTool() {}

StatusCode TrackCaloClusterCreatorTool::initialize() {
  ATH_CHECK(m_loosetrackvertexassoTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterCreatorTool::finalize() {
  return StatusCode::SUCCESS;
}

void TrackCaloClusterCreatorTool::createChargedTCCs(xAOD::TrackCaloClusterContainer* tccContainer, const xAOD::TrackParticleClusterAssociationContainer* assocContainer, std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt, std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap ) {
    
    const xAOD::VertexContainer *vxCont=0;
    StatusCode sc = evtStore()->retrieve(vxCont, m_vertexContname);
    if (sc.isFailure()) {
        ATH_MSG_WARNING ("Vertex container " << m_vertexContname << " not found! Can't perform TVA!");
    }

    for ( const auto* assocClusters : *assocContainer ) {
        ATH_MSG_VERBOSE ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
            bool isMatched = true;
            if (vxCont && vxCont->size()!=0) {
	      isMatched = m_loosetrackvertexassoTool->isCompatible(*trk, *(vxCont->at(0)) );
            } else {
	      ATH_MSG_WARNING ("Vertex container " << m_vertexContname << " is empty! Can't perform TVA!");
            }
            if (!isMatched) continue;
        } else {
	  ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
        FourMom_t tcc_4p(0.,0.,0.,0.);
        // follow the link to the calorimeter clusters
        ATH_MSG_VERBOSE ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                    double cluster_pt       = m_useEnergy ? cluster->e() : cluster->pt();
                    double totalcluster_pt  = m_useEnergy ? TrackTotalClusterPt->at(trk).E() : TrackTotalClusterPt->at(trk).Pt();
                    tcc_4p += cluster->p4()*(( trk->pt() * cluster_pt / totalcluster_pt) / ((clusterToTracksWeightMap->at(cluster)).Pt()));
                    ATH_MSG_VERBOSE ("cluster->pt() " << cluster_pt << " cluster->eta() " << cluster->eta() << " cluster->phi() " 
		    << cluster->phi() << " track pt " << trk->pt() << " (clusterToTracksWeightMap.at(cluster)).Pt() " << (clusterToTracksWeightMap->at(cluster)).Pt());
            } // for caloClusterLinks
        } // if caloClusterLinks().size
        
        // retrieve the caloExtensionContainer to get the track direction at the calo entrance
        IParticleToCaloExtensionMap * caloExtensionMap = 0;
	if(evtStore()->retrieve(caloExtensionMap,m_caloEntryMapName).isFailure())
	  ATH_MSG_WARNING( "Unable to retrieve " << m_caloEntryMapName << " will leak the ParticleCaloExtension" );
	
	const Trk::TrackParameters* pars = caloExtensionMap->readCaloEntry(trk);
	
	double eta = pars->position().eta();
	double phi = pars->position().phi();
	
	if (m_doOriginCorrection)
	  computeVertexCorr(eta, phi, (vxCont->at(0))->position(), pars->position().perp());
	
// 	std::cout << "Element = " << trk << " --- eta --> " << pars->position().eta() << "      " << trk->eta()  << "    --- delta = " << (pars->position().eta() - trk->eta())<< std::endl;
// 	std::cout << "Element = " << trk << " --- phi --> " << pars->position().phi() << "      " << trk->phi()  << "    --- delta = " << (pars->position().phi() - trk->phi())<< std::endl;
        
        tcc->setP4(tcc_4p.Pt(),eta,phi,tcc_4p.M());
        tcc->setTaste(xAOD::TrackCaloCluster::Taste::Charged);
        tccContainer->push_back(tcc);
        ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->getTaste());
    } // for assoc clusters
    
}

void TrackCaloClusterCreatorTool::createNeutralTCCs(xAOD::TrackCaloClusterContainer* tccContainer, 
						    const xAOD::CaloClusterContainer* assocContainer, 
						    std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap  ) {

    for ( const auto* cluster : *assocContainer ) {
        if(clusterToTracksWeightMap->find(cluster)==clusterToTracksWeightMap->end()){
            xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
            tccContainer->push_back(tcc);
            tcc->setP4(cluster->pt(),cluster->eta(),cluster->phi(),cluster->m());
            tcc->setTaste(xAOD::TrackCaloCluster::Taste::Neutral);
            ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->getTaste());
        }
    } // for all clusters
    
}

void TrackCaloClusterCreatorTool::createTrackOnlyTCCs(xAOD::TrackCaloClusterContainer* tccContainer, const xAOD::TrackParticleContainer* assocContainer, std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt  ) {
    
    const xAOD::VertexContainer *vxCont=0;
    StatusCode sc = evtStore()->retrieve(vxCont, m_vertexContname);
    if (sc.isFailure()) {
        ATH_MSG_WARNING ("Vertex container " << m_vertexContname << " not found! Can't perform TVA!");
    }

    for ( const auto* track : *assocContainer ) {
        if(TrackTotalClusterPt->find(track)==TrackTotalClusterPt->end()){
            bool isMatched = true;
            if (vxCont && vxCont->size()!=0) {
                isMatched = m_loosetrackvertexassoTool->isCompatible(*track, *(vxCont->at(0)) );
            }
            else{
                ATH_MSG_WARNING ("Vertex container " << m_vertexContname << " is empty! Can't perform TVA!");
            }
            if (!isMatched) continue;
            xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
            tccContainer->push_back(tcc);
            tcc->setP4(track->pt(),track->eta(),track->phi(),track->m());
            tcc->setTaste(xAOD::TrackCaloCluster::Taste::TrackOnly);
            ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->getTaste());
        }
    } // for all clusters
    
}

void TrackCaloClusterCreatorTool::computeVertexCorr(double& eta, double& phi, const Amg::Vector3D& vertex, double radius) {
  
  if (radius<1.) return;
  
  if (std::fabs(eta)>10. || std::fabs(phi)>10.) return;
  
  CxxUtils::sincos sc (phi);
  double iradius = 1 / radius;
  eta += (-vertex[2]/std::cosh(eta) + sc.apply (vertex[1], vertex[0])*std::tanh(eta)) * iradius;
  phi += sc.apply (vertex[0], -vertex[1]) * iradius;
}