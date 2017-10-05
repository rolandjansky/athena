/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCaloClusterRecTools/ClusterFilterTool.h"
#include "TrackCaloClusterRecTools/IParticleToCaloExtensionMap.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "FourMomUtils/P4Helpers.h"

ClusterFilterTool::ClusterFilterTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
  m_caloEntryMapName("ParticleToCaloExtensionMap"),
  m_loosetrackvertexassoTool("LooseTrackVertexAssociationTool")
{
  declareInterface<IClusterFilterTool>(this);
  declareProperty("LooseTrackVertexAssoTool",         m_loosetrackvertexassoTool                              );
  declareProperty("ParticleCaloEntryMapName",         m_caloEntryMapName                                      );
  declareProperty("TrackParticleContainerName",       m_trackParticleCollectionName = "InDetTrackParticles"   );
  declareProperty("VertexCollection",                 m_vertexCollectionName        = "PrimaryVertices"       );
  declareProperty("ConeSize",                         m_coneSize                    = 0.2                     );
  declareProperty("PtFractionAtPV0",                  m_ptFractionPV0               = 0.1                     );
}

ClusterFilterTool::~ClusterFilterTool() {}

StatusCode ClusterFilterTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK(m_loosetrackvertexassoTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode ClusterFilterTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

bool ClusterFilterTool::rejectCluster(const xAOD::CaloCluster& cluster) {
      
  // loop on all the tracks
  const auto allTracks   = getContainer<xAOD::TrackParticleContainer>(m_trackParticleCollectionName);
  if (not allTracks) {
    ATH_MSG_DEBUG ("No rejection applied... returning");
    return false;
  }
  
  const auto allVertices = getContainer<xAOD::VertexContainer>(m_vertexCollectionName);
  if (not allVertices) {
    ATH_MSG_DEBUG ("No rejection applied... returning");
    return false;
  }  
  
  bool matchedPV0 = false;
  bool matchedPVX  = false;
  double pt_PV0 = 0.;
  double pt_PVX = 0.;
      
  for (const auto& track : *allTracks) {
    // check compatibility with PVX where X>0
    // retrieve the caloExtensionContainer to get the track position at the calo entrance
    IParticleToCaloExtensionMap * caloExtensionMap = 0;
    if(evtStore()->retrieve(caloExtensionMap,m_caloEntryMapName).isFailure())
      ATH_MSG_WARNING( "Unable to retrieve " << m_caloEntryMapName << " will leak the ParticleCaloExtension" );
    const Trk::TrackParameters* pars = caloExtensionMap->readCaloEntry(track);
    if (not pars) {
      ATH_MSG_DEBUG( "Extrapolated parameters non existing --> No rejection applied... returning" );
      continue;
    }
    
    float eta = pars->position().eta();     
    float phi = pars->position().phi();
    
    float dPhi = P4Helpers::deltaPhi( cluster.phi(), phi);
    float dEta = cluster.eta()-eta;
    float dr2  = dPhi*dPhi+ dEta*dEta;
        
    // check if the track is matching the cluster
    if (dr2<m_coneSize*m_coneSize) {      
      if (allVertices && allVertices->size()!=0) {
	if (m_loosetrackvertexassoTool->isCompatible(*track, *(allVertices->at(0)))) {
	  ATH_MSG_DEBUG ("PV0 is matched");
	  matchedPV0 = true;
	  pt_PV0 +=track->pt();
	} else matchedPVX = true;
	pt_PVX+=track->pt();
      } else {
	ATH_MSG_WARNING ("Vertex container " << m_vertexCollectionName << " is empty! Can't perform TVA! --> No rejection applied... returning");
	return false;
      }
    }
  }
      
  if (not matchedPV0 and matchedPVX) return true;
 
  return false;
  
}