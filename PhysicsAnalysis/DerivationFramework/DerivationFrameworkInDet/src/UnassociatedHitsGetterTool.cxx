/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/UnassociatedHitsGetterTool.h"

#include "AthenaKernel/errorcheck.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkTrack/Track.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace DerivationFramework {

UnassociatedHitsGetterTool::UnassociatedHitsGetterTool(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) : 
  AthAlgTool(type, name, parent)
{
  declareInterface<IUnassociatedHitsGetterTool>(this);
}

UnassociatedHitsGetterTool::~UnassociatedHitsGetterTool(){
  
}
  
StatusCode UnassociatedHitsGetterTool::initialize(){

  CHECK(AthAlgTool::initialize());
  
  // retrieve PRD association tool
  ATH_CHECK( m_prdToTrackMapKey.initialize(!m_prdToTrackMapKey.key().empty()));
  ATH_CHECK( m_assoTool.retrieve(DisableTool{!m_prdToTrackMapKey.key().empty()} ) );
  ATH_CHECK( m_trackCollection.initialize());
  ATH_CHECK( m_pixelClusterContainer.initialize( !m_pixelClusterContainer.key().empty()) );
  ATH_CHECK( m_SCTClusterContainer.initialize( !m_SCTClusterContainer.key().empty()) );
  // Read Cond Handle Key
  ATH_CHECK( m_SCTDetEleCollKey.initialize(!m_SCTClusterContainer.key().empty()) );

  ATH_CHECK( m_TRTDriftCircleContainer.initialize( !m_TRTDriftCircleContainer.key().empty()) );

  return StatusCode::SUCCESS;
}
  
const MinBiasPRDAssociation* UnassociatedHitsGetterTool::get (bool /*allowMissing*/) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // If we fail to find something we need in SG on the first call,
  // issue a warning and don't try again (this can happen if we're
  // reading an AOD).  But consider it an ERROR if it happens
  // after the first call.

  // retrieve track collection
  SG::ReadHandle<TrackCollection> trackCollection(m_trackCollection,ctx);
  if(!trackCollection.isValid()) {
    ATH_MSG_FATAL("Track collection " << m_trackCollection.key() << " not found in StoreGate");
    return nullptr;
  }

  // Get empty state for PRD association tool.
  const Trk::PRDtoTrackMap *prd_to_track_map;
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map_cleanup;
  if (!m_prdToTrackMapKey.key().empty()) {
     SG::ReadHandle<Trk::PRDtoTrackMap> prd_to_track_map_handle(m_prdToTrackMapKey,ctx);
     if (!prd_to_track_map_handle.isValid()) {
        ATH_MSG_ERROR(  "Failed to get PRDs to track map " << m_prdToTrackMapKey.key());
        return nullptr;
     }
     prd_to_track_map=prd_to_track_map_handle.cptr();
  }
  else {
     prd_to_track_map_cleanup = m_assoTool->createPRDtoTrackMap();
  // Loop over tracks and add PRDs to the PRD association tool
     for (const Trk::Track* track : *trackCollection) {
        StatusCode sc = m_assoTool->addPRDs(*prd_to_track_map_cleanup, *track);
        if(sc.isFailure()){
           ATH_MSG_FATAL(  "Could not add PRDs to track");
           return nullptr;
        }
     }
     prd_to_track_map = prd_to_track_map_cleanup.get();
  }

  std::unique_ptr<MinBiasPRDAssociation> PRDAssociation(std::make_unique<MinBiasPRDAssociation>());
  if (!m_pixelClusterContainer.key().empty()) {
  // retrieve pixel clusters
  SG::ReadHandle<InDet::PixelClusterContainer> pixelClusters(m_pixelClusterContainer,ctx);
  if(!pixelClusters.isValid()) {
    ATH_MSG_FATAL("Pixel cluster container '" << m_pixelClusterContainer.key() << "' not found in StoreGate");
    return nullptr;
  }
  // Loop on pixel clusters
  InDet::PixelClusterContainer::const_iterator pixCollItr = pixelClusters->begin();
  InDet::PixelClusterContainer::const_iterator pixCollEnd = pixelClusters->end();
  for(; pixCollItr!=pixCollEnd; pixCollItr++){

    InDet::PixelClusterCollection::const_iterator pixItr = (*pixCollItr)->begin();
    InDet::PixelClusterCollection::const_iterator pixEnd = (*pixCollItr)->end();
    for(; pixItr!=pixEnd; pixItr++){

      // ask the association tool if the hit was associated
      if(prd_to_track_map->isUsed(*(*pixItr))) continue;

      // count number of unassociated pixel hits
      PRDAssociation->nPixelUA++;

      // find out which detector element the hit belongs to
      const InDetDD::SiDetectorElement *det = (*pixItr)->detectorElement();

      // count pixel barrel unassociated hits
      if(det->isBarrel()) PRDAssociation->nPixelBarrelUA++;
      
      // count pixel endcap unassociated hits: use hit Z position to determin A/C side
      if(det->isEndcap()){
	if((*pixItr)->globalPosition().z()<0){
	  PRDAssociation->nPixelEndCapCUA++;
	}else{
	  PRDAssociation->nPixelEndCapAUA++;
	}
      }

      // count B-Layer unassociated hits
      if(det->isBlayer()) PRDAssociation->nBlayerUA++;
    }
  }
  }

  // retrieve SCT clusters
  if (!m_SCTClusterContainer.key().empty()) {
  SG::ReadHandle<InDet::SCT_ClusterContainer> SCTClusters(m_SCTClusterContainer,ctx);
  if(!SCTClusters.isValid()) {
    ATH_MSG_FATAL("SCT cluster container '" << m_SCTClusterContainer.key() << "' not found in StoreGate");
    return nullptr;
  }
  // Loop on SCT clusters
  InDet::SCT_ClusterContainer::const_iterator sctCollItr = SCTClusters->begin();
  InDet::SCT_ClusterContainer::const_iterator sctCollEnd = SCTClusters->end();
  for(; sctCollItr!=sctCollEnd; sctCollItr++){

    InDet::SCT_ClusterCollection::const_iterator sctItr = (*sctCollItr)->begin();
    InDet::SCT_ClusterCollection::const_iterator sctEnd = (*sctCollItr)->end();
    for(; sctItr!=sctEnd; sctItr++){

      // ask the association tool if the hit was associated
      if(prd_to_track_map->isUsed(*(*sctItr))) continue;

      // count number of unassociated SCT hits
      PRDAssociation->nSCTUA++;

      // find out which detector element the hit belongs to
      const InDetDD::SiDetectorElement *det = (*sctItr)->detectorElement();

      // count SCT barrel unassociated hits
      if(det->isBarrel()) PRDAssociation->nSCTBarrelUA++;
      
      // count SCT endcap unassociated hits: use hit Z position to determin A/C side
      if(det->isEndcap()){
	if((*sctItr)->globalPosition().z()<0){
	  PRDAssociation->nSCTEndCapCUA++;
	}else{
	  PRDAssociation->nSCTEndCapAUA++;
	}
      }
    }
  }
  }

  // retrieve TRT drift circles
  if (!m_TRTDriftCircleContainer.key().empty()) {
  SG::ReadHandle<InDet::TRT_DriftCircleContainer> TRTDriftCircles(m_TRTDriftCircleContainer,ctx);
  if(!TRTDriftCircles.isValid()) {
    ATH_MSG_FATAL("TRT drift circle container '" << m_TRTDriftCircleContainer << "' not found in StoreGate");
    return nullptr;
  }
  // Loop on TRT clusters
  InDet::TRT_DriftCircleContainer::const_iterator trtCollItr = TRTDriftCircles->begin();
  InDet::TRT_DriftCircleContainer::const_iterator trtCollEnd = TRTDriftCircles->end();
  for(; trtCollItr!=trtCollEnd; trtCollItr++){

    InDet::TRT_DriftCircleCollection::const_iterator trtItr = (*trtCollItr)->begin();
    InDet::TRT_DriftCircleCollection::const_iterator trtEnd = (*trtCollItr)->end();
    for(; trtItr!=trtEnd; trtItr++){

      // ask the association tool if the hit was associated
      if(prd_to_track_map->isUsed(*(*trtItr))) continue;

      // count number of unassociated TRT hits
      PRDAssociation->nTRTUA++;

      // find out which detector element the hit belongs to
      const InDetDD::TRT_BaseElement *det = (*trtItr)->detectorElement();

      InDetDD::TRT_BaseElement::Type type = det->type();

      // count TRT barrel unassociated hits
      if(type == InDetDD::TRT_BaseElement::BARREL) PRDAssociation->nTRTBarrelUA++;
      
      // count TRT endcap unassociated hits: use hit Z position to determin A/C side
      if(type == InDetDD::TRT_BaseElement::ENDCAP){
	if(det->center().z()<0){
	  PRDAssociation->nTRTEndCapCUA++;
	}else{
	  PRDAssociation->nTRTEndCapAUA++;
	}
      }
    }
  }
  }

  return PRDAssociation.release();
}

void UnassociatedHitsGetterTool::releaseObject (const MinBiasPRDAssociation* p) const {
  if(p) delete p;
}

} // namespace DerivationFramework
