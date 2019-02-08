/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/UnassociatedHitsGetterTool.h"

#include "AthenaKernel/errorcheck.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"

namespace DerivationFramework {

UnassociatedHitsGetterTool::UnassociatedHitsGetterTool(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) : 
  AthAlgTool(type, name, parent),
  m_storeGate(0),
  m_assoTool("Trk::PRD_AssociationTool"),
  m_firstTime (true),
  m_disabled (false)
{
  declareInterface<IUnassociatedHitsGetterTool>(this);
  declareProperty("PRDAssociationTool", m_assoTool);
  declareProperty("TrackCollection", m_trackCollection = "Tracks");
  declareProperty("PixelClusters", m_pixelClusterContainer = "PixelClusters");
  declareProperty("SCTClusterContainer", m_SCTClusterContainer = "SCT_Clusters");
  declareProperty("TRTDriftCircleContainer", m_TRTDriftCircleContainer = "TRT_DriftCircles");
}

UnassociatedHitsGetterTool::~UnassociatedHitsGetterTool(){
  
}
  
StatusCode UnassociatedHitsGetterTool::initialize(){

  CHECK(AthAlgTool::initialize());
  
  // Retrieve a pointer to StoreGate
  CHECK( service("StoreGateSvc", m_storeGate) );
  
  // retrieve PRD association tool
  CHECK( m_assoTool.retrieve() );

  // Read Cond Handle Key
  CHECK( m_SCTDetEleCollKey.initialize() );

  return StatusCode::SUCCESS;
}
  
const MinBiasPRDAssociation* UnassociatedHitsGetterTool::get (bool /*allowMissing*/) const {
    
  // If we fail to find something we need in SG on the first call,
  // issue a warning and don't try again (this can happen if we're
  // reading an AOD).  But consider it an ERROR if it happens
  // after the first call.
  
  if (m_disabled){
    return 0;
  }
  
  MSG::Level level = MSG::ERROR;
  if (m_firstTime) {
    level = MSG::WARNING;
    m_firstTime = false;
  }
  
  // retrieve track collection
  const TrackCollection* trackCollection;
  StatusCode sc = m_storeGate->retrieve(trackCollection, m_trackCollection);  
  if(sc.isFailure() || !trackCollection) {
    m_disabled = true;
    REPORT_MESSAGE (level) << "Track collection " << m_trackCollection << " not found in StoreGate";
    return 0;
  }
  
  // retrieve pixel clusters
  const InDet::PixelClusterContainer *pixelClusters;
  sc = m_storeGate->retrieve(pixelClusters, m_pixelClusterContainer);
  if(sc.isFailure() || !pixelClusters) {
    m_disabled = true;
    REPORT_MESSAGE (level) << "Pixel cluster container '" << m_pixelClusterContainer << "' not found in StoreGate";
    return 0;
  }
  
  // retrieve SCT clusters
  const InDet::SCT_ClusterContainer *SCTClusters;
  sc = m_storeGate->retrieve(SCTClusters, m_SCTClusterContainer);
  if(sc.isFailure() || !SCTClusters) {
    m_disabled = true;
    REPORT_MESSAGE (level) << "SCT cluster container '" << m_SCTClusterContainer << "' not found in StoreGate";
    return 0;
  }

  // retrieve TRT drift circles
  const InDet::TRT_DriftCircleContainer *TRTDriftCircles;
  sc = m_storeGate->retrieve(TRTDriftCircles, m_TRTDriftCircleContainer);
  if(sc.isFailure() || !TRTDriftCircles) {
    m_disabled = true;
    REPORT_MESSAGE (level) << "TRT drift circle container '" << m_TRTDriftCircleContainer << "' not found in StoreGate";
    return 0;
  }

  // clear the PRD association tool
  m_assoTool->reset();

  // Loop over tracks and add PRDs to the PRD association tool
  TrackCollection::const_iterator trkItr = trackCollection->begin();
  TrackCollection::const_iterator trkEnd = trackCollection->end();
  for(; trkItr!=trkEnd; trkItr++){
    StatusCode sc = m_assoTool->addPRDs(*(*trkItr));
    if(sc.isFailure()){
      REPORT_MESSAGE (MSG::ERROR) << "Could not add PRDs to track";
      return 0;
    }
  }

  MinBiasPRDAssociation *PRDAssociation = new MinBiasPRDAssociation();

  // Loop on pixel clusters
  InDet::PixelClusterContainer::const_iterator pixCollItr = pixelClusters->begin();
  InDet::PixelClusterContainer::const_iterator pixCollEnd = pixelClusters->end();
  for(; pixCollItr!=pixCollEnd; pixCollItr++){

    InDet::PixelClusterCollection::const_iterator pixItr = (*pixCollItr)->begin();
    InDet::PixelClusterCollection::const_iterator pixEnd = (*pixCollItr)->end();
    for(; pixItr!=pixEnd; pixItr++){

      // ask the association tool if the hit was associated
      if(m_assoTool->isUsed(*(*pixItr))) continue;

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

  // Loop on SCT clusters
  InDet::SCT_ClusterContainer::const_iterator sctCollItr = SCTClusters->begin();
  InDet::SCT_ClusterContainer::const_iterator sctCollEnd = SCTClusters->end();
  for(; sctCollItr!=sctCollEnd; sctCollItr++){

    InDet::SCT_ClusterCollection::const_iterator sctItr = (*sctCollItr)->begin();
    InDet::SCT_ClusterCollection::const_iterator sctEnd = (*sctCollItr)->end();
    for(; sctItr!=sctEnd; sctItr++){

      // ask the association tool if the hit was associated
      if(m_assoTool->isUsed(*(*sctItr))) continue;

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

  // Loop on TRT clusters
  InDet::TRT_DriftCircleContainer::const_iterator trtCollItr = TRTDriftCircles->begin();
  InDet::TRT_DriftCircleContainer::const_iterator trtCollEnd = TRTDriftCircles->end();
  for(; trtCollItr!=trtCollEnd; trtCollItr++){

    InDet::TRT_DriftCircleCollection::const_iterator trtItr = (*trtCollItr)->begin();
    InDet::TRT_DriftCircleCollection::const_iterator trtEnd = (*trtCollItr)->end();
    for(; trtItr!=trtEnd; trtItr++){

      // ask the association tool if the hit was associated
      if(m_assoTool->isUsed(*(*trtItr))) continue;

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

  return PRDAssociation;
}

void UnassociatedHitsGetterTool::releaseObject (const MinBiasPRDAssociation* p) const {
  if(p) delete p;
}

} // namespace DerivationFramework
