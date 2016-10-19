/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigLongLivedParticlesHypo/TrigLLPInnerDetectorHypo.h"

#include "xAODEventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

// Space point Classes,
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "GaudiKernel/IssueSeverity.h"

//---------------------------------------------------------------------------------

TrigLLPInnerDetectorHypo::TrigLLPInnerDetectorHypo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator), 
    m_hltExecuteInitialisationRun(kFALSE),
    m_detStore("DetectorStore", name),
    m_regionSelector("RegSelSvc", name),
    m_log(msgSvc(), name),
    m_pixHelper(0),
    m_sctHelper(0)
    {

      declareProperty("PixelSP_ContainerName",                   m_pixelSpName = "PixelTrigSpacePoints"  );
      declareProperty("SCT_SP_ContainerName",                    m_sctSpName = "SCT_TrigSpacePoints"     );
      declareProperty("RegionSelectorTool",                      m_regionSelector);
      
      declareProperty("MaxNModIdentifier",                       m_maxnid = 100 );
      declareProperty("PixelModuleThreshold",                    m_pixModuleThreshold = 50 );
      declareProperty("SCTModuleThreshold",                      m_sctModuleThreshold = 100 );
      declareProperty("PixelClusToTCut",                         m_pixelClusToTCut = 20. );
      declareProperty("PixelClusTotBins",                        m_hPixelClusTotBins = 20 );
      declareProperty("PixelClusTotMin",                         m_hPixelClusTotMin = 0.5 );
      declareProperty("PixelClusTotMax",                         m_hPixelClusTotMax = 20.5 );
      declareProperty("PixelClusSizeBins",                       m_hPixelClusSizeBins = 1 );
      declareProperty("PixelClusSizeMin",                        m_hPixelClusSizeMin = 1. );
      declareProperty("PixelClusSizeMax",                        m_hPixelClusSizeMax = 2. );
      
      declareMonitoredStdContainer("PixelDataErrors",             m_pixelDataErrors);
      declareMonitoredStdContainer("SctDataErrors",               m_sctDataErrors);
      
      declareMonitoredVariable("PIX_SPCount",                     m_totNumPixSP );
      declareMonitoredVariable("SCT_SPCount",                     m_totNumSctSP );
      declareMonitoredVariable("RatioA",                          m_ratioA );
      declareMonitoredVariable("RatioB",                          m_ratioB );
      
      // spacepoints per module
      declareMonitoredStdContainer("PixSpPerModule",              m_pixSpPerModule );
      declareMonitoredStdContainer("SctSpPerModule",              m_sctSpPerModule);
      
      // EC/barrel monitoring
      declareMonitoredVariable("PixClBarrel",                      m_pixClBarrel );
      declareMonitoredVariable("PixClEndcapA",                     m_pixClEndcapA );
      declareMonitoredVariable("PixClEndcapC",                     m_pixClEndcapC );
      
      // initialization of non-static class members to clean up Coverity
      declareProperty("pixel_barrel",   m_pixel_barrel = true );
      declareProperty("pixel_b_layer",  m_pixel_b_layer = true );
      declareProperty("pixel_disk",     m_pixel_disk = true );
      declareProperty("nPixSP",         m_nPixSP = 0. );
      declareProperty("pixclToT",       m_pixclToT = 0. );
      declareProperty("totNumPixCL_1",  m_totNumPixCL_1 = 0. );
      declareProperty("totNumPixCL_2",  m_totNumPixCL_2 = 0. );
      declareProperty("totNumPixCLmin3",m_totNumPixCLmin3 = 0. );
      declareProperty("pixListSize",    m_pixListSize = 0 );
      declareProperty("sctListSize",    m_sctListSize = 0 );
      declareProperty("SPpixBarr",      m_SPpixBarr = 0 );
      declareProperty("SPpixECA",       m_SPpixECA = 0 );
      declareProperty("SPpixECC",       m_SPpixECC = 0 );
      declareProperty("nSctSP",         m_nSctSP = 0 );
      declareProperty("SPsctBarr",      m_SPsctBarr = 0 );
      declareProperty("SPsctECA",       m_SPsctECA = 0 );
      declareProperty("SPsctECC",       m_SPsctECC = 0 );
}


//---------------------------------------------------------------------------------


TrigLLPInnerDetectorHypo::~TrigLLPInnerDetectorHypo() {}


//---------------------------------------------------------------------------------


HLT::ErrorCode TrigLLPInnerDetectorHypo::hltInitialize() {

  m_log.setLevel(outputLevel());

  if (msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Initialize: " << name() << endreq;

  if ( m_regionSelector.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Unable to retrieve RegionSelector tool " << m_regionSelector.type() << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } else
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Successfully initialised RegionSelector tool !" << endreq;

  if(m_detStore.retrieve().isFailure()) {
    if (msgLvl() <= MSG::FATAL) m_log << MSG::FATAL << "Failed to connect to " << m_detStore.typeAndName() << endreq;
    return StatusCode::FAILURE;
  } else
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Successfully initialised DetectorStore!" << endreq;
 
  StatusCode sc_pixH = m_detStore->retrieve(m_pixHelper, "PixelID");
  if( sc_pixH.isFailure() ){
    if (msgLvl() <= MSG::WARNING) m_log << MSG::WARNING << "Could not obtain pix helper!" << endreq;
    return StatusCode::FAILURE;
  } else 
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Successfully initialised pixel helper" << endreq;

  StatusCode sc_sctH = m_detStore->retrieve(m_sctHelper, "SCT_ID");
  if( sc_sctH.isFailure() ){
    if (msgLvl() <= MSG::WARNING) m_log << MSG::WARNING << "Could not obtain sct helper!" << endreq;
    return StatusCode::FAILURE;
  } else
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Successfully initialised SCT helper" << endreq;
  
  if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << " TrigLLPInnerDetectorHypo initialized successfully" << endreq; 
  return HLT::OK;  
}

//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigLLPInnerDetectorHypo::checkDetectorMask() {

  m_hltExecuteInitialisationRun = true;

  m_log << MSG::DEBUG << "[TrigLLPInnerDetectorHypo::checkDetectorMask]  beginning run with this " << name() << endreq;

  const xAOD::EventInfo* evinfo = 0;
  if (store()->retrieve(evinfo).isFailure()) {
    if (msgLvl() <= MSG::ERROR) m_log << MSG::ERROR << "Cannot retrieve xAOD::EventInfo from SG for detmasks" << endreq;
    return HLT::SG_ERROR;
  }
  else {
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "xAOD::EventInfo Run Information [Run,Evt,Lumi,Time,BunchCross,DetMask] = [" 
      << evinfo->runNumber()
      << "," << evinfo->eventNumber()
      << "," << evinfo->lumiBlock()
      << "," << evinfo->timeStamp()
      << ":" << evinfo->timeStampNSOffset()
      << "," << evinfo->bcid()
      << ",0x" << std::hex << evinfo->detectorMask() << std::dec
      << "]" << endreq;

    uint64_t mask = evinfo->detectorMask();
    eformat::helper::DetectorMask decoder(mask);

    if (mask == 0) {
      if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Detector Mask == 0." << endreq; 
      m_sct_barrel_a_side = true;
      m_sct_barrel_c_side = true;
      m_sct_endcap_a_side = true;
      m_sct_endcap_c_side = true;
      m_pixel_barrel  = true;
      m_pixel_b_layer = true;
      m_pixel_disk    = true;
    } else {
      m_sct_barrel_a_side = decoder.is_set(eformat::SCT_BARREL_A_SIDE);
      m_sct_barrel_c_side = decoder.is_set(eformat::SCT_BARREL_C_SIDE);
      m_sct_endcap_a_side = decoder.is_set(eformat::SCT_ENDCAP_A_SIDE);
      m_sct_endcap_c_side = decoder.is_set(eformat::SCT_ENDCAP_C_SIDE);
      m_pixel_barrel  = decoder.is_set(eformat::PIXEL_BARREL);
      m_pixel_b_layer = decoder.is_set(eformat::PIXEL_B_LAYER);
      m_pixel_disk    = decoder.is_set(eformat::PIXEL_DISK);
    }

    if( msgLvl() <= MSG::INFO ){
      m_log << MSG::INFO << "sct_barrel_a_side is " << (m_sct_barrel_a_side==true? "present" : "OFF! ") << endreq;
      m_log << MSG::INFO << "sct_barrel_c_side is " << (m_sct_barrel_c_side==true? "present" : "OFF! ") << endreq;
      m_log << MSG::INFO << "sct_endcap_a_side is " << (m_sct_endcap_a_side==true? "present" : "OFF! ") << endreq;
      m_log << MSG::INFO << "sct_endcap_c_side is " << (m_sct_endcap_c_side==true? "present" : "OFF! ") << endreq;
      
      m_log << MSG::INFO << "pixel_barrel is      " << (m_pixel_barrel==true? "present" : "OFF! ") << endreq;
      m_log << MSG::INFO << "pixel_b_layer is     " << (m_pixel_b_layer==true? "present" : "OFF! ") << endreq;
      m_log << MSG::INFO << "pixel_disk is        " << (m_pixel_disk==true? "present" : "OFF! ") << endreq;
    }
  }
  return HLT::OK;
}

HLT::ErrorCode TrigLLPInnerDetectorHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  if (m_hltExecuteInitialisationRun == false) {
    HLT::ErrorCode ec = checkDetectorMask();
    if (ec != HLT::OK) return ec;
  }

  if ( msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Executing " << name() << endreq;
  }

  std::vector<IdentifierHash>::iterator hashIt, hashItEnd;
  
  m_totNumPixSP = 0;
  m_totNumPixCL_1 = 0; 
  m_totNumPixCL_2 = 0; 
  m_totNumPixCLmin3 = 0; 
  m_pixclToT = 0.;
 
  // pixel spacepoints according to position EC/Barrel
  m_pixClBarrel = 0;
  m_pixClEndcapA = 0;
  m_pixClEndcapC = 0;
  m_SPpixBarr = 0;
  m_SPpixECA = 0;
  m_SPpixECC = 0;

  StatusCode sc;
  
  // this counts timing for pixels only though SCT SP are retrieved at the same time
  m_listOfPixIds.clear();
  m_listOfSctIds.clear();

  const SpacePointContainer* pixCont = 0;
  sc = store()->retrieve( pixCont, m_pixelSpName );

  if(sc.isFailure() || !pixCont){
    if (msgLvl() <= MSG::ERROR) m_log << MSG::ERROR<< "Trig Pixel SP container " << m_pixelSpName <<" not found"<<endreq; 
    return HLT::TOOL_FAILURE;
  } else{
    if (msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Successfully retrieved pixel SP container!" << endreq;
  }
  
  m_regionSelector->DetHashIDList(PIXEL, m_listOfPixIds );
  m_pixListSize = m_listOfPixIds.size();
  
  if( m_pixListSize != 0 ){
    
    // loop over pixel collections
    const InDet::PixelCluster* pixClust;
    const std::vector<Identifier>& rdoList();
    
    hashIt=m_listOfPixIds.begin();
    hashItEnd=m_listOfPixIds.end();
    
    for( ; hashIt != hashItEnd; ++hashIt ){
      
      // get single pixel collection -> pixSpCollIt
      SpacePointContainer::const_iterator pixSpCollIt = pixCont->indexFind( (*hashIt) );
      if( pixSpCollIt == pixCont->end() ) continue;
      if( (*pixSpCollIt) == NULL ) continue;
      
      // identify a module/wafer
      Identifier pixid = (*pixSpCollIt)->identify(); 
      
      int bec = m_pixHelper->barrel_ec(pixid);
      
      // retrieve number of pixel SP/CL per collection
      SpacePointCollection::const_iterator spItEnd = (*pixSpCollIt)->end();
      SpacePointCollection::const_iterator spIt = (*pixSpCollIt)->begin();
      
      for( ; spIt != spItEnd; ++spIt ){
	
	const Trk::SpacePoint* pSP = (*spIt);
	pixClust = static_cast<const InDet::PixelCluster*> ( pSP->clusterList().first );
	m_pixclToT = pixClust->totalToT();
		
	// total 
	m_pixSpPerModule.push_back(m_nPixSP);
	
	if( msgLvl() <= MSG::DEBUG ){ 
	  m_log << MSG::DEBUG << "REGTEST : Formed  " << m_totNumPixSP << " pixel spacepoints in total." << endreq; 
	  m_log << MSG::DEBUG << "REGTEST : " << m_totNumPixCL_1 << " have cl size == 1 in total." << endreq; 
	  m_log << MSG::DEBUG << "REGTEST : " << m_totNumPixCL_2 << " have cl size == 2 in total." << endreq; 
	  m_log << MSG::DEBUG << "REGTEST : " << m_totNumPixCLmin3 << " have cl size >= 3 in total." << endreq; 
	  m_log << MSG::DEBUG << "REGTEST : Formed  " << m_totNumPixSP << " pixel spacepoints after ToT cut in total." << endreq;
	  m_log << MSG::DEBUG << "REGTEST : Formed " << m_pixClBarrel << " SP in pixel barrel in total." << endreq;
	  m_log << MSG::DEBUG << "REGTEST : Formed " << m_pixClEndcapA << " SP in pixel ECA in total." << endreq;
	  m_log << MSG::DEBUG << "REGTEST : Formed " << m_pixClEndcapC << " SP in pixel ECC in total." << endreq;
	}    
	
	m_ratioA = -1.;  m_ratioB = -1.;  
	if( m_totNumPixSP > 0. )
	  m_ratioA = (m_totNumPixCL_2 + m_totNumPixCLmin3)/m_totNumPixSP;  
	
	if( m_totNumPixCL_1 > 0. )
	  m_ratioB = m_totNumPixCL_2/m_totNumPixCL_1;
	
      }
    }
  }
  

  // ------ SCT PART ------------------------------------------------
  std::vector<Identifier> droppedSctModules;
  droppedSctModules.clear();
  
  m_nSctSP = 0;
  m_sctDataErrors.clear();
  m_sctSpPerModule.clear();
  m_sctSpBarrel = 0;
  m_sctSpEndcapA = 0;
  m_sctSpEndcapC = 0;
  m_SPsctBarr = 0;
  m_SPsctECA = 0;
  m_SPsctECC = 0;
    
  // empty the pixel list here
  m_listOfPixIds.clear();
  m_listOfSctIds.clear();
  
  const SpacePointContainer* sctCont;
  sc = store()->retrieve( sctCont, m_sctSpName );
  if( sc.isFailure() ){
    if (msgLvl() <= MSG::WARNING) m_log << MSG::WARNING << "Trig SP SCT container " << m_sctSpName <<" not found"<<endreq; 
    return HLT::TOOL_FAILURE;
  }
  
  m_regionSelector->DetHashIDList(SCT, m_listOfSctIds );
  m_sctListSize = m_listOfSctIds.size();
  
  if( m_sctListSize !=0 ){
    
    // loop over SCT collections
    std::vector<IdentifierHash>::iterator sctItEnd = m_listOfSctIds.end();
    std::vector<IdentifierHash>::iterator sctIt = m_listOfSctIds.begin();
    for( ; sctIt != sctItEnd; ++sctIt ){
      
      SpacePointContainer::const_iterator sctSpCollIt = sctCont->indexFind( (*sctIt) );
      
      if( sctSpCollIt == sctCont->end() ) continue;
      if( (*sctSpCollIt) == NULL ) continue;
      
      const std::vector<Identifier>& rdoList();
      m_nSctSP = (*sctSpCollIt)->size();
      
      // returns detector element identifier
      Identifier sctid = (*sctSpCollIt)->identify();
      
      int bec = (int)m_sctHelper->barrel_ec(sctid);
      
      if( msgLvl() <= MSG::VERBOSE ){ 
	m_log << MSG::VERBOSE << " Formed " << m_nSctSP << " sct spacepoints" ;
	m_log << MSG::VERBOSE << " with sctid module " << sctid << endreq;
      }
      
      // total 
      m_sctSpPerModule.push_back(m_nSctSP);
      
      // check if there is a tripped module
      if( (m_SPsctBarr > m_sctModuleThreshold) || (m_SPsctECA > m_sctModuleThreshold) || (m_SPsctECC > m_sctModuleThreshold) ){
	
	unsigned int nSctModIds = droppedSctModules.size();
	if( nSctModIds <= m_maxnid ) {
	  droppedSctModules.push_back(sctid);
	} else {
	  if (msgLvl() <= MSG::WARNING) m_log << MSG::WARNING << "More than " << m_maxnid << " sct modules are noisy, dump their id : " << sctid << endreq;
	}
      }
      else{
	m_sctSpBarrel += m_SPsctBarr;
	m_sctSpEndcapA += m_SPsctECA;
	m_sctSpEndcapC += m_SPsctECC;
      }
      m_SPsctBarr = 0;
      m_SPsctECA = 0;
      m_SPsctECC = 0;
      m_nSctSP = 0;
    }
  }//end of //if( m_sctListSize !=0 )
  
  m_totNumSctSP = m_sctSpEndcapC + m_sctSpBarrel + m_sctSpEndcapA;
  if( msgLvl() <= MSG::DEBUG ){ 
    m_log << MSG::DEBUG << "REGTEST : Formed  " << m_totNumSctSP << " sct spacepoints in total." << endreq;
    m_log << MSG::DEBUG << "REGTEST : Formed  " << m_sctSpEndcapC << " sct ECC spacepoints in total." << endreq;
    m_log << MSG::DEBUG << "REGTEST : Formed  " << m_sctSpBarrel << " sct Barr spacepoints in total." << endreq;
    m_log << MSG::DEBUG << "REGTEST : Formed  " << m_sctSpEndcapA << " sct ECA spacepoints in total." << endreq;
  }
  
  return HLT::OK;
}

//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigLLPInnerDetectorHypo::hltFinalize() {
  m_log << MSG::DEBUG << " finalizing TrigLLPInnerDetectorHypo : "<< name() << endreq; 
  return HLT::OK;  
}

