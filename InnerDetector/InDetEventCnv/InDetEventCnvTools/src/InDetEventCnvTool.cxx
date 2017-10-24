/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetEventCnvTools/InDetEventCnvTool.h"

//#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
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

InDet::InDetEventCnvTool::InDetEventCnvTool(
  const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_pixMgrLocation("Pixel"),
  m_pixMgr(0),
  m_sctMgrLocation("SCT"),
  m_sctMgr(0),
  m_trtMgrLocation("TRT"),
  m_trtMgr(0),
  m_setPrepRawDataLink(false)
{
  declareInterface<ITrkEventCnvTool>(this);
  declareProperty("PixelMgrLocation", m_pixMgrLocation);
  declareProperty("SCT_MgrLocation", m_sctMgrLocation);
  declareProperty("TRT_MgrLocation", m_trtMgrLocation);
  declareProperty("RecreatePRDLinks", m_setPrepRawDataLink);
  
}

InDet::InDetEventCnvTool::~InDetEventCnvTool()
{
}

StatusCode InDet::InDetEventCnvTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  // get Pixel Detector Description Manager
  if( detStore()->contains<InDetDD::PixelDetectorManager>(m_pixMgrLocation)){
    sc = detStore()->retrieve(m_pixMgr, m_pixMgrLocation);
    if (sc.isFailure()) {
        ATH_MSG_FATAL("Could not get PixelDetectorDescription");
        return sc;
    }
  } else {
    ATH_MSG_INFO("No Pixels? Could not get PixelDetectorDescription"); 
  }
        
  // Get SCT Detector Description Manager
  if(detStore()->contains<InDetDD::SCT_DetectorManager>(m_sctMgrLocation)){
    sc = detStore()->retrieve(m_sctMgr, m_sctMgrLocation);
    if (sc.isFailure()) {
        ATH_MSG_FATAL("Could not get SCT_DetectorDescription");
        return sc;
    }
  } else {
    ATH_MSG_INFO("No SCT? Could not get SCT_DetectorDescription"); 
  }
  // check if SLHC geo is used (TRT not implemented) 
  // if not SLHC, get the TRT Det Descr manager
  sc = detStore()->retrieve(m_idDictMgr, "IdDict");
  if (sc.isFailure()) {
    std::cout << "Could not get IdDictManager !" << std::endl;
    return StatusCode::FAILURE;
  } 
  const IdDictDictionary* dict = m_idDictMgr->manager()->find_dictionary("InnerDetector"); 
  if(!dict) {
    std::cout << " Cannot access InnerDetector dictionary "<< std::endl;
    return StatusCode::FAILURE;
  }
  bool isSLHC = false;
  // Find value for the field TRT - if not found is SLHC geo
  //int trtField   = -1;
  //if (dict->get_label_value("part", "TRT", trtField)) isSLHC=true;
  // Find string SLHC in dictionary file name - if found is SLHC geo
  if (dict->file_name().find("SLHC")!=std::string::npos) isSLHC=true;

  if(!isSLHC && detStore()->contains<InDetDD::TRT_DetectorManager>(m_trtMgrLocation)){
    // Get TRT Detector Description Manager
    sc = detStore()->retrieve(m_trtMgr, m_trtMgrLocation);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not get TRT_DetectorDescription");
      return sc;
    }
  }else {
    ATH_MSG_INFO("No TRT? Could not get TRT_DetectorDescription"); 
  }


//retrieving the various ID helpers
  
  if (detStore()->retrieve(m_IDHelper, "AtlasID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get ATLAS ID helper");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_pixClusContName.initialize() );
  ATH_CHECK( m_sctClusContName.initialize() );
  ATH_CHECK( m_trtDriftCircleContName.initialize() );

  return sc;
     
}

void 
InDet::InDetEventCnvTool::checkRoT( const Trk::RIO_OnTrack& rioOnTrack )
{
  InDetConcreteType type=Unknown;
  if (0!=dynamic_cast<const SCT_ClusterOnTrack*>(&rioOnTrack) )     type = SCT;
  if (0!=dynamic_cast<const PixelClusterOnTrack*>(&rioOnTrack) )    type = Pixel;
  if (0!=dynamic_cast<const TRT_DriftCircleOnTrack*>(&rioOnTrack) ) type = TRT;
  if (type==Unknown) {
    ATH_MSG_ERROR("Type does not match known concrete type of InDet! Dumping RoT:"<<rioOnTrack);
  }else{
    ATH_MSG_VERBOSE("Type = "<<type);
  }
      
 // const Identifier& id           = rioOnTrack.identify();

    
  return;
}

std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
    InDet::InDetEventCnvTool::getLinks( const Trk::RIO_OnTrack& rioOnTrack    )
{
  using namespace Trk;
  const TrkDetElementBase* detEl = 0;
  const PrepRawData*       prd   = 0;
  const Identifier& id           = rioOnTrack.identify();
   
  if (m_IDHelper->is_pixel(id) ) 
  {
    ATH_MSG_DEBUG ("Set Pixel detector element."); 
    // use IdentifierHash for speed
    detEl = m_pixMgr->getDetectorElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = pixelClusterLink( id, rioOnTrack.idDE() );
  }
  else if (m_IDHelper->is_sct(id) ) 
  {
    ATH_MSG_DEBUG("Set SCT detector element" ); 
    // use IdentifierHash for speed
    detEl = m_sctMgr->getDetectorElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = sctClusterLink( id, rioOnTrack.idDE() );
  }
  else if (m_IDHelper->is_trt(id)) 
  {
    ATH_MSG_DEBUG("Set TRT detector element" ); 
    // use IdentifierHash for speed
    detEl = m_trtMgr->getElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = trtDriftCircleLink( id, rioOnTrack.idDE() );
  }else{    
    ATH_MSG_WARNING("Unknown type of ID detector from identifier :"
        << id<<", in string form:" 
        << m_IDHelper->show_to_string(id) 
        ); 
  }
  return std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>(detEl,prd); 
}

void InDet::InDetEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT ) {
    
  InDet::PixelClusterOnTrack* pixel = dynamic_cast<InDet::PixelClusterOnTrack*>(RoT);
  if (pixel!=0){
    prepareRIO_OnTrackElementLink<const InDet::PixelClusterContainer, InDet::PixelClusterOnTrack>(pixel);
    return;
  }
  InDet::SCT_ClusterOnTrack* sct = dynamic_cast<InDet::SCT_ClusterOnTrack*>(RoT);
  if (sct!=0){
    prepareRIO_OnTrackElementLink<const InDet::SCT_ClusterContainer, InDet::SCT_ClusterOnTrack>(sct);
    return;
  }
  InDet::TRT_DriftCircleOnTrack* trt = dynamic_cast<InDet::TRT_DriftCircleOnTrack*>(RoT);
  if (trt!=0){
    prepareRIO_OnTrackElementLink<const InDet::TRT_DriftCircleContainer, InDet::TRT_DriftCircleOnTrack>(trt);
    return;
  }
  return;
}

void InDet::InDetEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) {
  std::pair<const Trk::TrkDetElementBase *, const Trk::PrepRawData *> pair = getLinks( *RoT );
  Trk::ITrkEventCnvTool::setRoT_Values( pair, RoT );
  return;
}

const Trk::TrkDetElementBase* 
InDet::InDetEventCnvTool::getDetectorElement(const Identifier& id, const IdentifierHash& idHash)
{
    
  const Trk::TrkDetElementBase* detEl=0;
//    if (m_pixMgr->getIdg()->is_pixel(id) ) 

  if (m_IDHelper->is_pixel(id) ) 
  {
  
    ATH_MSG_DEBUG("Set Pixel detector element."); 
    // use IdentifierHash for speed
    detEl = m_pixMgr->getDetectorElement( idHash ) ;
  }
  else if (m_IDHelper->is_sct(id) )
  {

    ATH_MSG_DEBUG("Set SCT detector element" ); 
    // use IdentifierHash for speed
    detEl = m_sctMgr->getDetectorElement( idHash ) ;
  }
  else if (m_IDHelper->is_trt(id) ) 
  {

    ATH_MSG_DEBUG("Set TRT detector element" ); 
    // use IdentifierHash for speed
    detEl = m_trtMgr->getElement( idHash ) ;
  }else{
    ATH_MSG_WARNING("Unknown type of ID detector from identifier :"
        << id<<", in string form:" 
        << m_IDHelper->show_to_string(id) 
        ); 
  }
  return detEl;
}

const Trk::TrkDetElementBase* 
InDet::InDetEventCnvTool::getDetectorElement(const Identifier& id)
{
 
  const Trk::TrkDetElementBase* detEl=0;
 
  if (m_IDHelper->is_pixel(id) ) 
  {
  
    ATH_MSG_DEBUG("Set Pixel detector element."); 
    // use IdentifierHash for speed
    detEl = m_pixMgr->getDetectorElement( id ) ;
  }
  else if (m_IDHelper->is_sct(id) ) 
  {

    ATH_MSG_DEBUG("Set SCT detector element" ); 
    // use IdentifierHash for speed
    detEl = m_sctMgr->getDetectorElement( id ) ;
  }
  else if (m_IDHelper->is_trt(id) ) 
  {
    ATH_MSG_DEBUG("Set TRT detector element" ); 
    // use IdentifierHash for speed
    detEl = m_trtMgr->getElement( id ) ;
  }else{
    ATH_MSG_WARNING("Unknown type of ID detector from identifier :"<< id<<", in string form:" 
        << m_IDHelper->show_to_string(id) ); 
  }
  return detEl;
}

const Trk::PrepRawData* 
    InDet::InDetEventCnvTool::pixelClusterLink( const Identifier& id,  const IdentifierHash& idHash  )
{
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<PixelClusterContainer> h_pixClusCont (m_pixClusContName);
  if (!h_pixClusCont.isValid()) {
      ATH_MSG_ERROR("Pixel Cluster container not found at "<<m_pixClusContName);
      return 0;
  }
  else{
    ATH_MSG_DEBUG("Pixel Cluster Container found" );
  }
  const PixelClusterContainer* pixClusCont = h_pixClusCont.cptr();

  PixelClusterContainer::const_iterator it = pixClusCont->indexFind(idHash);
  // if we find PRD, then recreate link
  if (it!=pixClusCont->end()) 
  {
    //loop though collection to find matching PRD.
    PixelClusterCollection::const_iterator collIt = (*it)->begin();
    PixelClusterCollection::const_iterator collItEnd = (*it)->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++){
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

const Trk::PrepRawData* 
    InDet::InDetEventCnvTool::sctClusterLink( const Identifier& id,  const IdentifierHash& idHash  )
{
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<SCT_ClusterContainer> h_sctClusCont (m_sctClusContName);
  if (!h_sctClusCont.isValid()) {
      ATH_MSG_ERROR("SCT Cluster container not found at "<<m_sctClusContName);
      return 0;
  }
  else{
    ATH_MSG_DEBUG("SCT Cluster Container found" );
  }
  const SCT_ClusterContainer* sctClusCont = h_sctClusCont.cptr();
 
  SCT_ClusterContainer::const_iterator it = sctClusCont->indexFind(idHash);
  // if we find PRD, then recreate link
  if (it!=sctClusCont->end()) 
  {
    //loop though collection to find matching PRD.
    SCT_ClusterCollection::const_iterator collIt = (*it)->begin();
    SCT_ClusterCollection::const_iterator collItEnd = (*it)->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++){
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

const Trk::PrepRawData* 
    InDet::InDetEventCnvTool::trtDriftCircleLink( const Identifier& id,  const IdentifierHash& idHash  )
{
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<TRT_DriftCircleContainer> h_trtDriftCircleCont (m_trtDriftCircleContName);
  if (!h_trtDriftCircleCont.isValid()) {
      ATH_MSG_ERROR("TRT Drift Circles container not found at "<<m_trtDriftCircleContName);
      return 0;
  }
  else{
    ATH_MSG_DEBUG("TRT Drift Circles Container found" );
  }
  const TRT_DriftCircleContainer* trtDriftCircleCont = h_trtDriftCircleCont.cptr();
  
  TRT_DriftCircleContainer::const_iterator it = trtDriftCircleCont->indexFind(idHash);
  // if we find PRD, then recreate link
  if (it!=trtDriftCircleCont->end()) 
  {
    //loop though collection to find matching PRD.
    TRT_DriftCircleCollection::const_iterator collIt = (*it)->begin();
    TRT_DriftCircleCollection::const_iterator collItEnd = (*it)->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++){
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

