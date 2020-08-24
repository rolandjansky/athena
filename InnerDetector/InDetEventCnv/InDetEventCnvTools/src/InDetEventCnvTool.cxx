/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetEventCnvTools/InDetEventCnvTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include <vector>
#include <cassert>
#include <iostream>

InDet::InDetEventCnvTool::InDetEventCnvTool(const std::string& t,
                                            const std::string& n,
                                            const IInterface*  p )
  :
  base_class(t,n,p),
  m_setPrepRawDataLink(false),
  m_IDHelper(nullptr),
  m_pixelHelper(nullptr),
  m_SCTHelper(nullptr),
  m_TRTHelper(nullptr),
  m_idDictMgr(nullptr)
{
  declareProperty("RecreatePRDLinks", m_setPrepRawDataLink);
  
}

StatusCode InDet::InDetEventCnvTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  // check if SLHC geo is used (TRT not implemented) 
  // if not SLHC, get the TRT Det Descr manager
  sc = detStore()->retrieve(m_idDictMgr, "IdDict");
  if (sc.isFailure()) {
    std::cout << "Could not get IdDictManager !" << std::endl;
    return StatusCode::FAILURE;
  } 
  const IdDictDictionary* dict = m_idDictMgr->manager()->find_dictionary("InnerDetector");
  if (!dict) {
    std::cout << " Cannot access InnerDetector dictionary "<< std::endl;
    return StatusCode::FAILURE;
  }

  bool isSLHC = false;
  // Find value for the field TRT - if not found is SLHC geo
  //int trtField   = -1;
  //if (dict->get_label_value("part", "TRT", trtField)) isSLHC=true;
  // Find string SLHC in dictionary file name - if found is SLHC geo
  if (dict->file_name().find("SLHC")!=std::string::npos) isSLHC=true;

  //retrieving the various ID helpers
  
  if (detStore()->retrieve(m_IDHelper, "AtlasID").isFailure()) {
    ATH_MSG_FATAL( "Could not get ATLAS ID helper");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( detStore()->retrieve(m_pixelHelper, "PixelID") );
  ATH_CHECK( detStore()->retrieve(m_SCTHelper, "SCT_ID") );
  ATH_CHECK( detStore()->retrieve(m_TRTHelper, "TRT_ID") );

  ATH_CHECK( m_pixClusContName.initialize() );
  ATH_CHECK( m_sctClusContName.initialize() );
  ATH_CHECK( m_trtDriftCircleContName.initialize() );

  ATH_CHECK( m_pixelDetEleCollKey.initialize() );
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );
  ATH_CHECK(!isSLHC && m_trtDetEleContKey.initialize(!m_trtDetEleContKey.key().empty()));

  return sc;
     
}

void 
InDet::InDetEventCnvTool::checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const {
  InDetConcreteType type=Unknown;
  if (0!=dynamic_cast<const SCT_ClusterOnTrack*>(&rioOnTrack) )     type = SCT;
  if (0!=dynamic_cast<const PixelClusterOnTrack*>(&rioOnTrack) )    type = Pixel;
  if (0!=dynamic_cast<const TRT_DriftCircleOnTrack*>(&rioOnTrack) ) type = TRT;
  if (type==Unknown) {
    ATH_MSG_ERROR("Type does not match known concrete type of InDet! Dumping RoT:"<<rioOnTrack);
  } else {
    ATH_MSG_VERBOSE("Type = "<<type);
  }
    
  return;
}

std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
InDet::InDetEventCnvTool::getLinks( Trk::RIO_OnTrack& rioOnTrack ) const
{
  using namespace Trk;
  const TrkDetElementBase* detEl = 0;
  const PrepRawData*       prd   = 0;
  const Identifier& id           = rioOnTrack.identify();
   
  if (m_IDHelper->is_pixel(id) ) {
    ATH_MSG_DEBUG ("Set Pixel detector element.");
    // use IdentifierHash for speed
    detEl = getPixelDetectorElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = pixelClusterLink( id, rioOnTrack.idDE() );
  } else if (m_IDHelper->is_sct(id)) {
    ATH_MSG_DEBUG("Set SCT detector element" );
    // use IdentifierHash for speed
    detEl = getSCTDetectorElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = sctClusterLink( id, rioOnTrack.idDE() );
  } else if (m_IDHelper->is_trt(id)) {
    ATH_MSG_DEBUG("Set TRT detector element" );
    // use IdentifierHash for speed
    detEl = getTRTDetectorElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = trtDriftCircleLink( id, rioOnTrack.idDE() );
  } else {
    ATH_MSG_WARNING("Unknown type of ID detector from identifier :"
                    << id<<", in string form:"
                    << m_IDHelper->show_to_string(id)
                    );
  }
  return std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>(detEl,prd);
}

void InDet::InDetEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
    
  InDet::PixelClusterOnTrack* pixel = dynamic_cast<InDet::PixelClusterOnTrack*>(RoT);
  if (pixel!=0) {
    prepareRIO_OnTrackElementLink<const InDet::PixelClusterContainer, InDet::PixelClusterOnTrack>(pixel);
    return;
  }
  InDet::SCT_ClusterOnTrack* sct = dynamic_cast<InDet::SCT_ClusterOnTrack*>(RoT);
  if (sct!=0) {
    prepareRIO_OnTrackElementLink<const InDet::SCT_ClusterContainer, InDet::SCT_ClusterOnTrack>(sct);
    return;
  }
  InDet::TRT_DriftCircleOnTrack* trt = dynamic_cast<InDet::TRT_DriftCircleOnTrack*>(RoT);
  if (trt!=0) {
    prepareRIO_OnTrackElementLink<const InDet::TRT_DriftCircleContainer, InDet::TRT_DriftCircleOnTrack>(trt);
    return;
  }
  return;
}

void
InDet::InDetEventCnvTool::prepareRIO_OnTrackLink( const Trk::RIO_OnTrack *RoT,
                                                  ELKey_t& key,
                                                  ELIndex_t& index) const
{
  const InDet::PixelClusterOnTrack* pixel = dynamic_cast<const InDet::PixelClusterOnTrack*>(RoT);
  if (pixel!=0) {
    prepareRIO_OnTrackElementLink<const InDet::PixelClusterContainer, InDet::PixelClusterOnTrack>(pixel, key, index);
    return;
  }
  const InDet::SCT_ClusterOnTrack* sct = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(RoT);
  if (sct!=0) {
    prepareRIO_OnTrackElementLink<const InDet::SCT_ClusterContainer, InDet::SCT_ClusterOnTrack>(sct, key, index);
    return;
  }
  const InDet::TRT_DriftCircleOnTrack* trt = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(RoT);
  if (trt!=0) {
    prepareRIO_OnTrackElementLink<const InDet::TRT_DriftCircleContainer, InDet::TRT_DriftCircleOnTrack>(trt, key, index);
    return;
  }
  return;
}

void InDet::InDetEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
  std::pair<const Trk::TrkDetElementBase *, const Trk::PrepRawData *> pair = getLinks( *RoT );
  Trk::ITrkEventCnvTool::setRoT_Values( pair, RoT );
  return;
}

const Trk::TrkDetElementBase* 
InDet::InDetEventCnvTool::getDetectorElement(const Identifier& id, const IdentifierHash& idHash) const {

  const Trk::TrkDetElementBase* detEl=0;

  if (m_IDHelper->is_pixel(id)) {

    ATH_MSG_DEBUG("Set Pixel detector element.");
    // use IdentifierHash for speed
    detEl = getPixelDetectorElement( idHash ) ;
  } else if (m_IDHelper->is_sct(id)) {

    ATH_MSG_DEBUG("Set SCT detector element" );
    // use IdentifierHash for speed
    detEl = getSCTDetectorElement( idHash ) ;
  } else if (m_IDHelper->is_trt(id)) {

    ATH_MSG_DEBUG("Set TRT detector element" );
    // use IdentifierHash for speed
    detEl = getTRTDetectorElement( idHash ) ;
  } else {
    ATH_MSG_WARNING("Unknown type of ID detector from identifier :"
                    << id<<", in string form:"
                    << m_IDHelper->show_to_string(id)
                    );
  }
  return detEl;
}

const Trk::TrkDetElementBase* 
InDet::InDetEventCnvTool::getDetectorElement(const Identifier& id) const {
 
  const Trk::TrkDetElementBase* detEl=0;
 
  if (m_IDHelper->is_pixel(id) ) {
  
    ATH_MSG_DEBUG("Set Pixel detector element.");
    // use IdentifierHash for speed
    const IdentifierHash wafer_hash = m_pixelHelper->wafer_hash(id);
    detEl = getPixelDetectorElement( wafer_hash ) ;
  } else if (m_IDHelper->is_sct(id)) {

    ATH_MSG_DEBUG("Set SCT detector element" );
    const Identifier wafer_id = m_SCTHelper->wafer_id(id);
    const IdentifierHash wafer_hash = m_SCTHelper->wafer_hash(wafer_id);
    detEl = getSCTDetectorElement( wafer_hash ) ;
  } else if (m_IDHelper->is_trt(id)) {
    ATH_MSG_DEBUG("Set TRT detector element");
    // use IdentifierHash for speed
    const Identifier strawLayerId = m_TRTHelper->layer_id(id);
    const IdentifierHash hashId = m_TRTHelper->straw_layer_hash(strawLayerId);
    detEl = getTRTDetectorElement( hashId ) ;
  } else {
    ATH_MSG_WARNING("Unknown type of ID detector from identifier :"<< id<<", in string form:"
                    << m_IDHelper->show_to_string(id) );
  }
  return detEl;
}

const Trk::PrepRawData* 
InDet::InDetEventCnvTool::pixelClusterLink( const Identifier& id,  const IdentifierHash& idHash ) const {
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<PixelClusterContainer> h_pixClusCont (m_pixClusContName);
  if (!h_pixClusCont.isValid()) {
    ATH_MSG_ERROR("Pixel Cluster container not found at "<<m_pixClusContName);
    return 0;
  } else {
    ATH_MSG_DEBUG("Pixel Cluster Container found" );
  }
  const PixelClusterCollection *ptr = h_pixClusCont->indexFindPtr(idHash);
  // if we find PRD, then recreate link
  if (ptr!=nullptr) {
    //loop though collection to find matching PRD.
    PixelClusterCollection::const_iterator collIt    = ptr->begin();
    PixelClusterCollection::const_iterator collItEnd = ptr->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++){
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

const Trk::PrepRawData* 
InDet::InDetEventCnvTool::sctClusterLink( const Identifier& id,  const IdentifierHash& idHash ) const {
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<SCT_ClusterContainer> h_sctClusCont(m_sctClusContName);
  if (!h_sctClusCont.isValid()) {
    ATH_MSG_ERROR("SCT Cluster container not found at "<<m_sctClusContName);
    return 0;
  } else {
    ATH_MSG_DEBUG("SCT Cluster Container found" );
  }
  const SCT_ClusterCollection *ptr = h_sctClusCont->indexFindPtr(idHash);
  // if we find PRD, then recreate link
  if (ptr!=nullptr) {
    //loop though collection to find matching PRD.
    SCT_ClusterCollection::const_iterator collIt    = ptr->begin();
    SCT_ClusterCollection::const_iterator collItEnd = ptr->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++) {
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

const Trk::PrepRawData* 
InDet::InDetEventCnvTool::trtDriftCircleLink( const Identifier& id,  const IdentifierHash& idHash ) const {
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<TRT_DriftCircleContainer> h_trtDriftCircleCont (m_trtDriftCircleContName);
  if (!h_trtDriftCircleCont.isValid()) {
    ATH_MSG_ERROR("TRT Drift Circles container not found at "<<m_trtDriftCircleContName);
    return 0;
  } else {
    ATH_MSG_DEBUG("TRT Drift Circles Container found" );
  }
  const TRT_DriftCircleCollection *ptr = h_trtDriftCircleCont->indexFindPtr(idHash);
  // if we find PRD, then recreate link
  if (ptr!=nullptr) {
    //loop though collection to find matching PRD.
    TRT_DriftCircleCollection::const_iterator collIt    = ptr->begin();
    TRT_DriftCircleCollection::const_iterator collItEnd = ptr->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++) {
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

const InDetDD::SiDetectorElement* InDet::InDetEventCnvTool::getPixelDetectorElement(const IdentifierHash& waferHash) const {
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEle{m_pixelDetEleCollKey};
  if (not pixelDetEle.isValid()) return nullptr;
  return pixelDetEle->getDetectorElement(waferHash);
}

const InDetDD::SiDetectorElement* InDet::InDetEventCnvTool::getSCTDetectorElement(const IdentifierHash& waferHash) const {
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle{m_SCTDetEleCollKey};
  if (not sctDetEle.isValid()) return nullptr;
  return sctDetEle->getDetectorElement(waferHash);
}

const InDetDD::TRT_BaseElement* InDet::InDetEventCnvTool::getTRTDetectorElement(const IdentifierHash& HashId) const {
  SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDetEleHandle(m_trtDetEleContKey);
  const InDetDD::TRT_DetElementCollection* elements(trtDetEleHandle->getElements());
  if (not trtDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_FATAL(m_trtDetEleContKey.fullKey() << " is not available.");
    return nullptr;
  }
  return elements->getDetectorElement(HashId);
}
