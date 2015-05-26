/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalManager.cxx
 * Implementation of inner detector global monitoring manager 
 * Based on AthenaMon.cxx
 *
 * @author
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>     
 *
 * $Id: InDetGlobalManager.cxx,v 1.33 2009-04-24 11:54:37 kastanas Exp $
 *
 *****************************************************************************/

//Framework
#include "InDetGlobalManager.h"
#include "InDetGlobalMotherMonTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ServiceHandle.h" 


#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/PixelID.h" 

//Standard c++
#include <string>
#include <vector>

//--------------------------------------------------------------------------
InDetGlobalManager::InDetGlobalManager(
    const std::string &name, 
    ISvcLocator *pSvcLocator)
    : AthenaMonManager(name, pSvcLocator),
      m_CombinedTracks(0),
      m_doTopBottom(false)
{
    declareProperty("SCTTrackName",         m_SCTTracksName);
    declareProperty("TRTTrackName",         m_TRTTracksName);
    declareProperty("PixelTrackName",       m_PIXTracksName);
    declareProperty("CombinedTrackName",    m_CombinedTracksName="ExtendedTracks");
    declareProperty("TRT_DriftCircleName",  m_TRT_DriftCircleName);
    declareProperty("doTopBottom",          m_doTopBottom);
}

//--------------------------------------------------------------------------
StatusCode InDetGlobalManager::initialize()
{

    StatusCode  sc;

    const TRT_ID *trtID = 0;
    if (detStore()->retrieve(trtID, "TRT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
      return StatusCode::FAILURE;
    }


    const SCT_ID *sctID = 0;
    if (detStore()->retrieve(sctID, "SCT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
      return StatusCode::FAILURE;
    }


    const PixelID *pixelID = 0;
    if (detStore()->retrieve(pixelID, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
      return StatusCode::FAILURE;
    }
    
    //--------------------------------------------------------------
    // Loop over all MonTools Name
    // ---> Create monitoring tool for each
    // ---> Book histograms
    //--------------------------------------------------------------
    
    sc = AthenaMonManager::initialize();
    if( !sc.isSuccess() ) return sc;
    
    ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
    for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); 
	 i != monToolsEnd; ++i ) {
      ToolHandle<IMonitorToolBase>& tool = *i;
      IMonitorToolBase* mon = tool.operator->();
      InDetGlobalMotherMonTool* managed = 
	dynamic_cast<InDetGlobalMotherMonTool*>( mon );	
      if( managed != 0 ) {
	managed->fillDetectorManagers(trtID, sctID, pixelID ); 
	managed->retrieveTools();
      }
    }
    
    
    return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------
StatusCode InDetGlobalManager::execute()
{    
  //----------------------------------------------------------------------
  // Retrieve track collections etc needed by the different monitoring tools
  // Avoid to retrieve several times the same object in the different 
  // monitoring tools
  //----------------------------------------------------------------------
  
  const TrackCollection *sct_tracks = 0;
  if (evtStore()->contains<TrackCollection>(m_SCTTracksName)) {
    StatusCode sc = evtStore()->retrieve(sct_tracks, m_SCTTracksName);
    if  ( msgLvl(MSG::DEBUG) ){
      if ( sc.isFailure()) {
	msg(MSG::DEBUG) <<"No SCT segments in StoreGate"<<endreq;
      } else {
	msg(MSG::DEBUG) <<"found SCT segments in StoreGate " 
			<<m_SCTTracksName<<" "<<sct_tracks->size()<<endreq;
      }
    }
  }
  
  const TrackCollection *pix_tracks = 0;
  if ( evtStore()->contains<TrackCollection>(m_PIXTracksName) ) {
    StatusCode sc = evtStore()->retrieve(pix_tracks, m_PIXTracksName);
    if  ( msgLvl(MSG::DEBUG) ) {
      if ( sc.isFailure() ) { 
	msg(MSG::DEBUG) <<"No PIX segments in StoreGate"<<endreq;
      } else {
	msg(MSG::DEBUG) <<"found PIX segments in StoreGate "  
			<<m_PIXTracksName<<" "<<pix_tracks->size()<<endreq;
      }
    }
  }
  
  const TrackCollection *trt_tracks = 0;
  if ( evtStore()->contains<TrackCollection>(m_TRTTracksName) ) {
    StatusCode sc = evtStore()->retrieve(trt_tracks, m_TRTTracksName);
    if  ( msgLvl(MSG::DEBUG) ) {
      if  ( sc.isFailure()) { 
	msg(MSG::DEBUG) <<"No TRT segments in StoreGate "
			<< m_TRTTracksName<<endreq;
      } else { 	    
	msg(MSG::DEBUG) <<"found TRT segments in StoreGate "
			<<m_TRTTracksName <<" "<<trt_tracks->size()<<endreq;
      }
    }
  }
  
  const TrackCollection *combined_tracks = 0;
  if ( evtStore()->contains<TrackCollection>(m_CombinedTracksName) ) {
    StatusCode sc = evtStore()->retrieve(combined_tracks,m_CombinedTracksName);
    if ( msgLvl(MSG::DEBUG) ) {
      if ( sc.isFailure()) {
	msg(MSG::DEBUG) <<"No combined tracks in StoreGate "
			<< m_CombinedTracksName<<endreq; 
      } else {                 
	msg(MSG::DEBUG)    <<"found combined tracks in StoreGate "
			   <<m_CombinedTracksName <<" "
			   <<combined_tracks->size()<<endreq;
      }
    }
  }
    
  //m_tracks=m_combined_tracks;   
  
  const BCM_RDO_Container* bcmRdoContainer=0;
  if ( evtStore()->contains<BCM_RDO_Container>("BCM_RDOs") ) {
    StatusCode sc = evtStore()->retrieve( bcmRdoContainer, "BCM_RDOs" );
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !bcmRdoContainer) {
        msg(MSG::DEBUG) <<"No BCM RDOs in StoreGate "<<endreq;
      } else {
        msg(MSG::DEBUG)
          <<"found BCM RDOs in StoreGate " <<endreq;
      }
    }
  }

  const PixelRDO_Container* pixRdoContainer=0;
  if ( evtStore()->contains<PixelRDO_Container>("PixelRDOs") ) {
    StatusCode sc = evtStore()->retrieve( pixRdoContainer, "PixelRDOs" );
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !pixRdoContainer) {
	msg(MSG::DEBUG) <<"No Pixel RDOs in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  <<"found Pixel RDOs in StoreGate " <<endreq;
      }
    }
  }
  
  const SCT_RDO_Container* sctRdoContainer=0;
  if ( evtStore()->contains<SCT_RDO_Container>("SCT_RDOs") ) {
    StatusCode sc = evtStore()->retrieve( sctRdoContainer, "SCT_RDOs" );
    if  ( msgLvl(MSG::DEBUG) ){
      if (sc.isFailure() || !sctRdoContainer) {
	msg(MSG::DEBUG) <<"No SCT RDOs in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  <<"found SCT RDOs in StoreGate " <<endreq;
      }
    }
  }
  
  
  const InDet::TRT_DriftCircleContainer* driftCircleContainer = 0;  
  if ( evtStore()->contains<InDet::TRT_DriftCircleContainer>(m_TRT_DriftCircleName) ) {
    StatusCode sc = evtStore()->retrieve(driftCircleContainer, m_TRT_DriftCircleName);
    if  ( msgLvl(MSG::DEBUG) ) { 
      if (sc.isFailure() || !driftCircleContainer) {
	msg(MSG::DEBUG) <<"No TRT RDO in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG) 
	  <<"found TRT driftcircles in StoreGate " <<endreq; 
      }
    }
  }
  
  const InDetTimeCollection *pixel_LVL1IDColl = 0;
  if ( evtStore()->contains<InDetTimeCollection>("PixelLVL1ID") ){
    StatusCode sc = evtStore()->retrieve(pixel_LVL1IDColl, "PixelLVL1ID");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !pixel_LVL1IDColl) {
	msg(MSG::DEBUG) << "No Pixel LVL1ID information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found Pixel LVL1ID information in StoreGate " <<endreq;
      }
    }
  }
  
  const InDetTimeCollection *sct_LVL1IDColl = 0;
  if ( evtStore()->contains<InDetTimeCollection>("SCT_LVL1ID") ){
    StatusCode sc = evtStore()->retrieve(sct_LVL1IDColl, "SCT_LVL1ID");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !sct_LVL1IDColl) {
	msg(MSG::DEBUG) << "No SCT LVL1ID information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found SCT LVL1ID information in StoreGate " <<endreq;
      }
    }
  }
  
  const InDetTimeCollection *trt_LVL1IDColl = 0;
  if ( evtStore()->contains<InDetTimeCollection>("TRT_LVL1ID") ){
    StatusCode sc = evtStore()->retrieve(trt_LVL1IDColl, "TRT_LVL1ID");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !trt_LVL1IDColl) {
	msg(MSG::DEBUG) << "No TRT LVL1ID information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found TRT LVL1ID information in StoreGate " <<endreq;
      }
    }
  }
  
  const InDetTimeCollection *pixel_BCIDColl = 0;
  if ( evtStore()->contains<InDetTimeCollection>("PixelBCID") ){
    StatusCode sc = evtStore()->retrieve(pixel_BCIDColl, "PixelBCID");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !pixel_BCIDColl) {
	msg(MSG::DEBUG) << "No Pixel BCID information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found Pixel BCID information in StoreGate " <<endreq;
      }
    }
  }
  
  const InDetTimeCollection *sct_BCIDColl = 0;
  if ( evtStore()->contains<InDetTimeCollection>("SCT_BCID") ){
    StatusCode sc = evtStore()->retrieve(sct_BCIDColl, "SCT_BCID");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !sct_BCIDColl) {
	msg(MSG::DEBUG) << "No SCT BCID information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found SCT BCID information in StoreGate " <<endreq;
      }
    }
  }
  
  const InDetTimeCollection *trt_BCIDColl = 0;
  if ( evtStore()->contains<InDetTimeCollection>("TRT_BCID") ){
    StatusCode sc = evtStore()->retrieve(trt_BCIDColl, "TRT_BCID");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !trt_BCIDColl) {
	msg(MSG::DEBUG) << "No TRT BCID information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found TRT BCID information in StoreGate " <<endreq;
      }
    }
  }
  
  const ComTime *trtPhase = 0;
  if ( evtStore()->contains<ComTime>("TRT_Phase") ){
    StatusCode sc = evtStore()->retrieve(trtPhase, "TRT_Phase");
    if  ( msgLvl(MSG::DEBUG) ) {
      if (sc.isFailure() || !trtPhase) {
	msg(MSG::DEBUG) << "No TRT event phase information in StoreGate "<<endreq;
      } else {
	msg(MSG::DEBUG)
	  << "found TRT event phase information in StoreGate " <<endreq;
      }
    }
  }
  
  //------------------------------------------------------------------
  
  
  ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
  for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); 
       i != monToolsEnd; ++i ) {
      ToolHandle<IMonitorToolBase>& tool = *i;
      IMonitorToolBase* mon = tool.operator->();
      InDetGlobalMotherMonTool* managed = 
	  dynamic_cast<InDetGlobalMotherMonTool*>( mon );
      
      if( managed != 0 ) {
	  if ( msgLvl(MSG::DEBUG) )    msg(MSG::DEBUG) 
	      <<"Loop over InDetGlobalMonTool " <<managed << endreq;
	  managed->fillTracks(sct_tracks, trt_tracks, 
			      combined_tracks, pix_tracks);
	  managed->fillRDOContainers(bcmRdoContainer, pixRdoContainer, sctRdoContainer, driftCircleContainer);	      
	  managed->fillTimeContainers( pixel_LVL1IDColl,sct_LVL1IDColl,trt_LVL1IDColl,
				       pixel_BCIDColl, sct_BCIDColl, trt_BCIDColl,
				       trtPhase );
      }
  }
  return AthenaMonManager::execute(); 
}
    
//------------------------------------------------------------------------
StatusCode InDetGlobalManager::finalize()
{
    return AthenaMonManager::finalize();    
}
