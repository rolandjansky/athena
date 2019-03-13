/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    : AthenaMonManager(name, pSvcLocator)
{
 
}

//--------------------------------------------------------------------------
StatusCode InDetGlobalManager::initialize()
{

    StatusCode  sc;

    const TRT_ID *trtID = 0;
    if (detStore()->retrieve(trtID, "TRT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
      return StatusCode::FAILURE;
    }


    const SCT_ID *sctID = 0;
    if (detStore()->retrieve(sctID, "SCT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg;
      return StatusCode::FAILURE;
    }


    const PixelID *pixelID = 0;
    if (detStore()->retrieve(pixelID, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
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
    
    ATH_CHECK( m_CombinedTracksName.initialize() );
    ATH_CHECK( m_TRT_DriftCircleName.initialize(m_doTRT) );
    ATH_CHECK( m_BCM_RDOs.initialize(m_doBCM) );
    ATH_CHECK( m_PixelRDOs.initialize(m_doPixel) );
    ATH_CHECK( m_SCT_RDOs.initialize(m_doSCT) );
    ATH_CHECK( m_PixelLVL1ID.initialize(m_doPixel && m_doTiming) );
    ATH_CHECK( m_SCT_LVL1ID.initialize(m_doSCT && m_doTiming) );
    ATH_CHECK( m_TRT_LVL1ID.initialize(m_doTRT && m_doTiming) );
    ATH_CHECK( m_PixelBCID.initialize(m_doPixel && m_doTiming) );
    ATH_CHECK( m_SCT_BCID.initialize(m_doSCT && m_doTiming) );
    ATH_CHECK( m_TRT_BCID.initialize(m_doTRT && m_doTiming) );
    ATH_CHECK( m_TRT_Phase.initialize(m_doTRT && m_doTRTPhase) );
    
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
 
  SG::ReadHandle<TrackCollection> combined_tracks (m_CombinedTracksName);
  if (combined_tracks.isValid()){
    msg(MSG::DEBUG) <<"found combined tracks in StoreGate " 
		    <<m_CombinedTracksName.key()<<" "<<combined_tracks->size()<<endmsg;
  }
  else{
    msg(MSG::ERROR) <<"No combined tracks in StoreGate"<<endmsg;
    return StatusCode::FAILURE;
  } 

  const BCM_RDO_Container * int_BCM_RDOs;
  
  if(m_doBCM){

    SG::ReadHandle<BCM_RDO_Container> bcmRdoContainer (m_BCM_RDOs);
    if (bcmRdoContainer.isValid()){
      msg(MSG::DEBUG) <<"found BCM RDOs in StoreGate " 
		      <<endmsg;
      int_BCM_RDOs = bcmRdoContainer.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No BCM RDOs in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  else{
    int_BCM_RDOs = NULL; 
  }
  
  
  const PixelRDO_Container * int_Pixel_RDOs;
  const InDetTimeCollection * int_Pixel_LVL1ID;
  const InDetTimeCollection * int_Pixel_BCID;
  
  if(m_doPixel){
    SG::ReadHandle<PixelRDO_Container> pixRdoContainer (m_PixelRDOs);
    if (pixRdoContainer.isValid()){
      msg(MSG::DEBUG) <<"found Pixel RDOs in StoreGate " 
		      <<endmsg;
      int_Pixel_RDOs = pixRdoContainer.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No Pixel RDOs in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    if(m_doTiming){
    SG::ReadHandle<InDetTimeCollection> pixel_LVL1IDColl (m_PixelLVL1ID);
    if (pixel_LVL1IDColl.isValid()){
      msg(MSG::DEBUG) <<"found Pixel LVL1ID information in StoreGate " 
		      <<endmsg;
      int_Pixel_LVL1ID = pixel_LVL1IDColl.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No Pixel LVL1ID information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    
    SG::ReadHandle<InDetTimeCollection> pixel_BCIDColl (m_PixelBCID);
    if (pixel_BCIDColl.isValid()){
      msg(MSG::DEBUG) <<"found Pixel BCID information in StoreGate " 
		      <<endmsg;
      int_Pixel_BCID = pixel_BCIDColl.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No Pixel BCID information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    }
    else{
      int_Pixel_LVL1ID = NULL;
      int_Pixel_BCID = NULL; 
    }
  }
  
  else{
    int_Pixel_RDOs = NULL;
    int_Pixel_LVL1ID = NULL;
    int_Pixel_BCID = NULL;
  }
  
  
  const SCT_RDO_Container * int_SCT_RDOs;
  const InDetTimeCollection * int_SCT_LVL1ID;
  const InDetTimeCollection * int_SCT_BCID;
  
  if(m_doSCT){
    SG::ReadHandle<SCT_RDO_Container> sctRdoContainer (m_SCT_RDOs);
    if (sctRdoContainer.isValid()){
      msg(MSG::DEBUG) <<"found SCT RDOs in StoreGate " 
		      <<endmsg;
      int_SCT_RDOs = sctRdoContainer.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No SCT RDOs in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    } 
  
    if(m_doTiming){
    SG::ReadHandle<InDetTimeCollection> sct_LVL1IDColl (m_SCT_LVL1ID);
    if (sct_LVL1IDColl.isValid()){
      msg(MSG::DEBUG) <<"found SCT LVL1ID information in StoreGate " 
		      <<endmsg;
      int_SCT_LVL1ID = sct_LVL1IDColl.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No SCT LVL1ID information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    
    SG::ReadHandle<InDetTimeCollection> sct_BCIDColl (m_SCT_BCID);
    if (sct_BCIDColl.isValid()){
      msg(MSG::DEBUG) <<"found SCT BCID information in StoreGate " 
		      <<endmsg;
      int_SCT_BCID = sct_BCIDColl.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No SCT BCID information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    }
    else{
     int_SCT_LVL1ID= NULL;
    int_SCT_BCID = NULL;  
    }
  }
  
  else{
    int_SCT_RDOs = NULL;
    int_SCT_LVL1ID= NULL;
    int_SCT_BCID = NULL;
  }
  
  
  const InDet::TRT_DriftCircleContainer * int_DriftCircles;
  const InDetTimeCollection * int_TRT_LVL1ID;
  const InDetTimeCollection * int_TRT_BCID;
  const ComTime * int_TRT_Phase;
  
  if(m_doTRT){
    
    SG::ReadHandle<InDet::TRT_DriftCircleContainer> driftCircleContainer (m_TRT_DriftCircleName);
    if (driftCircleContainer.isValid()){
      msg(MSG::DEBUG) <<"found TRT driftcircles in StoreGate " 
		      <<endmsg;
      int_DriftCircles = driftCircleContainer.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No TRT driftcircles in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    
    if(m_doTiming){
    SG::ReadHandle<InDetTimeCollection> trt_LVL1IDColl (m_TRT_LVL1ID);
    if (trt_LVL1IDColl.isValid()){
      msg(MSG::DEBUG) <<"found TRT LVL1ID information in StoreGate " 
		      <<endmsg;
      int_TRT_LVL1ID = trt_LVL1IDColl.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No TRT LVL1ID information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    
    SG::ReadHandle<InDetTimeCollection> trt_BCIDColl (m_TRT_BCID);
    if (trt_BCIDColl.isValid()){
      msg(MSG::DEBUG) <<"found TRT BCID information in StoreGate " 
		      <<endmsg;
      int_TRT_BCID = trt_BCIDColl.cptr();
    }
    else{
      msg(MSG::ERROR) <<"No TRT BCID information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    }
    else{
      int_TRT_LVL1ID = NULL;
       int_TRT_BCID = NULL;
    }

    if(m_doTRTPhase){
    SG::ReadHandle<ComTime> trtPhase (m_TRT_Phase);
    if (trtPhase.isValid()){
      msg(MSG::DEBUG) <<"found TRT event phase information in StoreGate " 
		      <<endmsg;
      int_TRT_Phase = trtPhase.cptr();
    }
    
    else{
      msg(MSG::ERROR) <<"No TRT event phase information in StoreGate"<<endmsg;
      return StatusCode::FAILURE;
    }
    }
    else int_TRT_Phase = NULL;
}
  
  else{
    int_DriftCircles = NULL;
    int_TRT_LVL1ID = NULL;
    int_TRT_BCID = NULL;
    int_TRT_Phase = NULL;
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
				     <<"Loop over InDetGlobalMonTool " <<managed << endmsg;
      managed->fillTracks(combined_tracks.cptr());
      managed->fillRDOContainers(int_BCM_RDOs, int_Pixel_RDOs, int_SCT_RDOs, int_DriftCircles);	      
      managed->fillTimeContainers( int_Pixel_LVL1ID,int_SCT_LVL1ID,int_TRT_LVL1ID, int_Pixel_BCID, int_SCT_BCID, int_TRT_BCID, int_TRT_Phase );
    }
  }
  return AthenaMonManager::execute(); 
}

//------------------------------------------------------------------------
StatusCode InDetGlobalManager::finalize()
{
    return AthenaMonManager::finalize();    
}
