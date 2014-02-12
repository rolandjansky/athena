/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
-------------------
ATLAS Collaboration
***************************************************************************/

#include "InDetTrigToolInterfaces/ITrigSCT_SpacePointTool.h"
#include "SiTrigSpacePointFormation/SiTrigSpacePointFinder.h"
#include "SiSpacePointFormation/SiTrackerSpacePointFinder.h"
#include "SiSpacePointTool/SiSpacePointMakerTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "InDetReadoutGeometry/SiLocalPosition.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h" 
#include "InDetReadoutGeometry/SiDetectorElementCollection.h" 

// Space point Classes,
#include "TrkSpacePoint/SpacePoint.h" 
#include "TrkSpacePoint/SpacePointCollection.h" 
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"


// Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"


namespace InDet{

  //  using namespace Trk;
using namespace InDet;


//----------------------------------  
//           Constructor:
//----------------------------------------------------------------------------
SiTrigSpacePointFinder::SiTrigSpacePointFinder(const std::string& name, 
					       ISvcLocator* pSvcLocator):
//----------------------------------------------------------------------------
   HLT::FexAlgo(name, pSvcLocator),
   m_spacePointsSCTName ("SCT_TrigSpacePoints"),
   m_spacePointsPixelName ("PixelTrigSpacePoints"),
   m_pixelClustersName ("PixelTrigClusters"),
   m_sctClustersName ("SCT_TrigClusters"),
   m_spOverlapName("SPTrigOverlap"),
   m_selectPixels(true),
   m_selectSCTs(true),
   m_overlap(false),
   m_SpacePointContainerSCT(0),
   m_SpacePointContainerPixel(0),
   m_spOverlapColl(0),
   m_trigSpacePointTool ("InDet::SCT_TrigSpacePointTool"),
   m_SiSpacePointMakerTool ("InDet::SiSpacePointMakerTool"),
   m_regionSelector("RegSelSvc", name),
   m_doFullScan(false),
   m_etaHalfWidth(0.1),
   m_phiHalfWidth(0.1)
      
{
  //Use the same space point name both for internal use (for graphics) end
  // for storing in TDS. If a name is the empty string, do not retrieve
  // those clusters.
  declareProperty("SpacePointsSCTName",       m_spacePointsSCTName);
  declareProperty("SpacePointsPixelName",     m_spacePointsPixelName);
  declareProperty("ProcessPixels",            m_selectPixels);
  declareProperty("ProcessSCTs",              m_selectSCTs);
  declareProperty("SCTSpacePointTrigHelperTool", m_trigSpacePointTool);
  declareProperty("SiSpacePointMakerTool", m_SiSpacePointMakerTool);
  declareProperty("PixelClustersName",        m_pixelClustersName);
  declareProperty("SCTClustersName",          m_sctClustersName);
  declareProperty("ProcessOverlaps",          m_overlap);
  declareProperty("OverlapSPName",            m_spOverlapName);

  declareProperty("RegionSelectorTool",      m_regionSelector);
  declareProperty("doFullScan",              m_doFullScan);
  declareProperty("EtaHalfWidth",            m_etaHalfWidth );
  declareProperty("PhiHalfWidth",            m_phiHalfWidth );
 
  declareMonitoredVariable("numSctClusters", m_nReceivedClustersSCT   );
  declareMonitoredVariable("numSctSpacePoints", m_nSCT    );
  declareMonitoredVariable("numPixClusters", m_nReceivedClustersPix   );
  declareMonitoredVariable("numPixSpacePoints", m_nPix    );
  declareMonitoredStdContainer("SctSPHashId", m_SPindentifierSct);
  declareMonitoredStdContainer("PixSPHashId", m_SPindentifierPix);

  //timers
  m_timerRegSel = m_timerSeed = m_timerRtrSi = m_timerSGate =0;
  m_timerRtrPx = m_timerFindSi = m_timerFindPx = 0;

}
//--------------------------------------------------------------------------
SiTrigSpacePointFinder::~SiTrigSpacePointFinder(){
}
//----------------------------------  

//----------------------------------
//          beginRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode SiTrigSpacePointFinder::hltBeginRun(){

  msg() << MSG::INFO << "SiTrigSpacePointFinder::hltBeginRun()" << endreq;

  return HLT::OK;
}

//----------------------------------
//          Initialize method:
//----------------------------------------------------------------------------
HLT::ErrorCode SiTrigSpacePointFinder::hltInitialize()
//----------------------------------------------------------------------------
{

  msg() <<MSG::INFO <<"SiTrigSpacePointFinder::hltInitialize()"<<endreq;
  
  if (m_spacePointsSCTName ==""){
    msg()<< MSG::FATAL<< "No name set for SCT space points"<< endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  if (m_spacePointsPixelName ==""){
    msg()<< MSG::FATAL<< "No name set for Pixels space points"<< endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  
  // Declare the tool for space point formation:
  StatusCode sc = m_trigSpacePointTool.retrieve();
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Unable to locate TrigSpacePointTool " 
	  << m_trigSpacePointTool.type() << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  
  sc = m_SiSpacePointMakerTool.retrieve();
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Unable to locate SiSpacePointMakerTool " 
	  << m_SiSpacePointMakerTool.type() << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 

  if(!m_doFullScan){  
    // Retrieving Region Selector Tool:
    if ( m_regionSelector.retrieve().isFailure() ) {
      msg() << MSG::FATAL 
	    << "Unable to retrieve RegionSelector tool "  
	    << m_regionSelector.type() << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
  }
  else{
      msg() << MSG::INFO
	    << "RegionSelector is not needed for FullScan " << endreq;
  }

  // Get an Identifier helper object
  StoreGateSvc* detStore(0);
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Detector service not found !" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  
  // Get te Pixel helper
  const PixelID* idHelperPixel;
  sc = detStore->retrieve(idHelperPixel,"PixelID"); 
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Cannot retrieve Pixel ID helper!" 
	  << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  
  // Get the SCT Helper
  const SCT_ID* sctIdHelper;
  sc = detStore->retrieve(sctIdHelper,"SCT_ID"); 
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Cannot retrieve SCT ID helper!"      
	  << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  

  // register the IdentifiableContainer into StoreGate
  // ------------------------------------------------------
  if( !store()->transientContains<SpacePointContainer>(m_spacePointsPixelName)){

    // declare the containers of SpacePoints for Pixel and SCT 
    m_SpacePointContainerPixel = new SpacePointContainer(idHelperPixel->wafer_hash_max()); 

    if (store()->record(m_SpacePointContainerPixel, m_spacePointsPixelName).isFailure()) {
      msg() << MSG::WARNING << " Container " << m_spacePointsPixelName
	    << " could not be recorded in StoreGate !" 
	    << endreq;
    } 
    else {
      msg() << MSG::INFO << "Container " << m_spacePointsPixelName 
	    << " registered  in StoreGate" << endreq;  
    }
  }
  else {    
    sc = store()->retrieve(m_SpacePointContainerPixel, m_spacePointsPixelName);
    
    if (sc.isFailure()) {
      msg() << MSG::ERROR << "Failed to get Pixel space point Container" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else { 
      msg() << MSG::INFO << "Got Pixel space point Container from TDS: "
	    << m_spacePointsPixelName << endreq;
    }
  }
  m_SpacePointContainerPixel->addRef();  //preventing SG from deleting object: 

  if( !store()->transientContains<SpacePointContainer>(m_spacePointsSCTName)){

    m_maxKey = idHelperPixel->wafer_hash_max();
    m_SpacePointContainerSCT = new SpacePointContainer(sctIdHelper->wafer_hash_max()); 
    
    if (store()->record(m_SpacePointContainerSCT, m_spacePointsSCTName).isFailure()) {
      msg() << MSG::WARNING << " Container " << m_spacePointsSCTName
	    << " could not be recorded in StoreGate !" 
	    << endreq;
    } 
    else {
      msg() << MSG::INFO << "Container " << m_spacePointsSCTName 
	    << " registered  in StoreGate" << endreq;  
    }
  }
  else {    
    sc = store()->retrieve(m_SpacePointContainerSCT, m_spacePointsSCTName);
    
    if (sc.isFailure()) {
      msg() << MSG::ERROR << "Failed to get SCT space point Container" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else { 
      msg() << MSG::INFO << "Got SCT space point Container from TDS: "
	    << m_spacePointsSCTName << endreq;
    }
  }
  m_SpacePointContainerSCT->addRef();

  //overlap 
  if (m_overlap){
    if( !store()->transientContains<SpacePointOverlapCollection>(m_spOverlapName)){
      
      m_spOverlapColl = new SpacePointOverlapCollection();
      
      if (store()->record(m_spOverlapColl,  m_spOverlapName).isFailure()) {
	msg() << MSG::WARNING << " Container " << m_spOverlapName
	      << " could not be recorded in StoreGate !" 
	      << endreq;
      } 
      else {
	msg() << MSG::INFO << "Container " << m_spOverlapName
	      << " registered  in StoreGate" << endreq;  
      }
    }
    else {    
      sc = store()->retrieve(m_spOverlapColl, m_spOverlapName);
    
      if (sc.isFailure()) {
	msg() << MSG::ERROR << "Failed to get " << m_spOverlapName << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else { 
	msg() << MSG::INFO << "Got " << m_spOverlapName << " from TDS "
	      << endreq;
      }
    }
    //    m_spOverlapColl->addRef();
  }

  // initializing the IdentifiableContainers for clusters:
  m_pixelClusterContainer=0;
  m_sctClusterContainer=0;
  
  // Timers:
  m_timerRegSel = addTimer("InDetEFSPFRegSel");
  m_timerSeed   = addTimer("InDetEFSPFSeed");
  m_timerRtrSi  = addTimer("InDetEFSPFRtrSi");
  m_timerSGate  = addTimer("InDetEFSPFSGate");
  m_timerRtrPx  = addTimer("InDetEFSPFRtrPx");
  m_timerFindSi = addTimer("InDetEFSPFindSi");
  m_timerFindPx = addTimer("InDetEFSPFindPx"); 


  msg()<<MSG::INFO <<"Initialized successfully"<<endreq;
  
  return HLT::OK;
}

//--------------------------------
//         Execute method:
//-------------------------------------------------------------------------
HLT::ErrorCode SiTrigSpacePointFinder::hltExecute(const HLT::TriggerElement*,
                                                   HLT::TriggerElement* outputTE)
//-------------------------------------------------------------------------
{ 

  //initialisation of monitored quantities
  m_nPix = 0;
  m_nSCT = 0;
  m_nReceivedClustersPix = 0;
  m_nReceivedClustersSCT = 0;
  m_listOfPixIds.clear();
  m_listOfSctIds.clear();
  m_SPindentifierSct.clear();
  m_SPindentifierPix.clear();


  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG <<"SiTrigSpacePointFinder::hltExecute()"<<endreq;

  StatusCode sc;
 
  if(doTiming()) m_timerSGate->resume();
  
  // register the IdentifiableContainer's into StoreGate
  //--------------------------------------------------------
  if( !store()->transientContains<SpacePointContainer>(m_spacePointsPixelName)){

    // Do the cleanup only once per event.
    m_SpacePointContainerPixel->cleanup();

    if(store()->record(m_SpacePointContainerPixel,
			  m_spacePointsPixelName,false).isFailure()){

      msg() << MSG::WARNING << "Container " << m_spacePointsPixelName 
	    << " could not be recorded in StoreGate !"
	      << endreq;
    }
    else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Container " << m_spacePointsPixelName
	      << " recorded in StoreGate" << endreq;
    }
  }
  else {    
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Container " << m_spacePointsPixelName
	    << " existed already in StoreGate" << endreq;
  } 

  if( !store()->transientContains<SpacePointContainer>(m_spacePointsSCTName)){

    // Do the cleanup only once per event.
    m_SpacePointContainerSCT->cleanup();

    if(store()->record(m_SpacePointContainerSCT,
			  m_spacePointsSCTName,false).isFailure()){

      msg() << MSG::WARNING << "Container " << m_spacePointsSCTName 
	      << " could not be recorded in StoreGate !"
	      << endreq;
    }
    else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Container " << m_spacePointsSCTName
	    << " recorded in StoreGate" << endreq;
    }
  }
  else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Container " << m_spacePointsSCTName
	      << " existed already in StoreGate" << endreq;
  }

  if (m_overlap){
    if( !store()->transientContains<SpacePointOverlapCollection>(m_spOverlapName)){
      
      m_spOverlapColl = new SpacePointOverlapCollection();
      
      if (store()->record(m_spOverlapColl,  m_spOverlapName).isFailure()) {
	msg() << MSG::WARNING << " Container " << m_spOverlapName
	      << " could not be recorded in StoreGate !" 
	      << endreq;
      } 
      else {
	msg() << MSG::INFO << "Container " << m_spOverlapName
	      << " registered  in StoreGate" << endreq;  
      }
    }
    else {    
      /*
      sc = store()->retrieve(m_spOverlapColl, m_spOverlapName);
    
      if (sc.isFailure()) {
	msg() << MSG::ERROR << "Failed to get " << m_spOverlapName << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else {
      */ 
      msg() << MSG::DEBUG << m_spOverlapName << " from TDS "
	    << endreq;
      //}
    }
    //    m_spOverlapColl->addRef();
  } else {
    m_spOverlapColl = 0;
  }
    
  if(doTiming()) m_timerSGate->pause();

  //---------------------------
  // Trigger specific part:
  //---------------------------
  //   Get from the Trigger Element the RoI
  //   Ask the Region Selector for the list of hash indexes for this ROI.
  //-------------------------------------------------------------------------

  if(doTiming()) m_timerSeed->start();

  const TrigRoiDescriptor* roi;
    
  // Get RoiDescriptor
  if ( ( HLT::OK != getFeature(outputTE, roi) ) || !roi ) {
    msg() << MSG::WARNING << "Can't get RoI" << endreq;
    return HLT::NAV_ERROR;
  }
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST:" << *roi << endreq;
    msg() << MSG::DEBUG
	  <<  "PhiHalfWidth: " << m_phiHalfWidth
	  << " EtaHalfWidth: "<< m_etaHalfWidth <<endreq;
  }

  
  if(doTiming()) m_timerSeed->stop();

  // Reconstruct SCT clusters:
  //---------------------------

  bool doSCT=true;
  if(m_selectSCTs){

    if(doTiming()) m_timerRtrSi->resume();
    sc =store()->retrieve(m_sctClusterContainer , m_sctClustersName);
    if (sc.isFailure()  || !m_sctClusterContainer ){
      msg() << MSG::WARNING 
	    <<"SCT Cluster container for SCT not found"<< endreq;
      doSCT=false;
    } else{
      if(msgLvl() <= MSG::DEBUG)
	msg() <<MSG::DEBUG <<"SCT Cluster container for SCT found" <<endreq;
    } 
    if(doTiming()) m_timerRtrSi->pause();
  }

  if (m_selectSCTs &&  doSCT ){ 

    if(!m_doFullScan){
      //   Get the SCT RDO's:
      if(doTiming()) m_timerRegSel->resume();
      
      m_regionSelector->DetHashIDList(SCT, 
				      *roi, 
				      m_listOfSctIds );
      if(doTiming()) m_timerRegSel->pause();
    }

    m_nSCT=0;
    m_nReceivedClustersSCT=0;

    if(!m_doFullScan){

      if(doTiming()) m_timerFindSi->resume();
      // Iterate through the hash id list. Retrieve the cluster collections
      // and form space points
      for( std::vector<IdentifierHash>::iterator itIDs =  m_listOfSctIds.begin(); 
	   itIDs != m_listOfSctIds.end(); itIDs++){
     
	// Retrieve the clusterCollection:

	SCT_ClusterContainer::const_iterator 
	  itSCT_Clusters(m_sctClusterContainer->indexFind((*itIDs)));

	if( itSCT_Clusters ==  m_sctClusterContainer->end()) continue;
      
	const SCT_ClusterCollection * SCTClusterCollection = &(**itSCT_Clusters);
	
	if( SCTClusterCollection->begin() == SCTClusterCollection->end())
	  continue;
      
	m_nReceivedClustersSCT += SCTClusterCollection->size(); 
	
	// Create SpacePointCollection
	//IdentifierHash idHash = SCTClusterCollection->identifyHash();
	IdentifierHash idHash((*itIDs));
	
	Identifier elementID = SCTClusterCollection->identify(); 
	
	SpacePointCollection* spacepointCollection = 
	  new SpacePointCollection(idHash); 
	spacepointCollection->setIdentifier(elementID); 
       	
	if (SCTClusterCollection->size() != 0) { 
	  
	  m_trigSpacePointTool->addSCT_SpacePoints(SCTClusterCollection,
						   m_sctClusterContainer,
						   spacepointCollection,
						   m_spOverlapColl);
	}
	
	if (spacepointCollection->size() != 0){
	  
	  sc = m_SpacePointContainerSCT->addCollection(spacepointCollection,
						       spacepointCollection->identifyHash());
	  m_nSCT += spacepointCollection->size();
  
	  m_SPindentifierSct.push_back(spacepointCollection->identifyHash() );
	  
	  if(!sc.isSuccess()){
	    if (msgLvl() <= MSG::VERBOSE)
	      msg() << MSG::VERBOSE << "Failed to add SCT SpacePoints to container : "
		    << spacepointCollection->identifyHash() << endreq;
	    delete spacepointCollection;
	  }
	} 
	else{
	  // -me- cleanup memory
	  delete (spacepointCollection);
	}

      } //end of RegSel

      if(doTiming()) m_timerFindSi->pause();
    }

    else{   //fullScan

      SCT_ClusterContainer::const_iterator it     = m_sctClusterContainer->begin();
      SCT_ClusterContainer::const_iterator itend  = m_sctClusterContainer->end();

      if(doTiming()) m_timerFindSi->resume();

      for (; it != itend; ++it) {

	const SCT_ClusterCollection *colNext=&(**it);
      
	// Create SpacePointCollection
	IdentifierHash idHash = colNext->identifyHash(); 
      
	Identifier elementID = colNext->identify(); 
	SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash); 
	spacepointCollection->setIdentifier(elementID); 

	if ( colNext->size() != 0) { 
	  m_nReceivedClustersSCT += colNext->size();

	  m_trigSpacePointTool->addSCT_SpacePoints(colNext,
						   m_sctClusterContainer,
						   spacepointCollection,
						   m_spOverlapColl);
	}
	else{
	  if (msgLvl() <= MSG::DEBUG)
	    msg() <<MSG::DEBUG<<"Empty SCT cluster collection"<<endreq; 
	}

	if (spacepointCollection->size() == 0){
	  // -me- clean up memory
	  delete (spacepointCollection);
	}else{

	  // -me- new IDC does no longer register in Storegate if hash is used !
          // sc= m_SpacePointContainer_SCT->addCollection( spacepointCollection, spacepointCollection->identify() );
          sc= m_SpacePointContainerSCT->addCollection( spacepointCollection, spacepointCollection->identifyHash() );

	  m_nSCT += spacepointCollection->size();

          if (sc.isSuccess()){
            if (msgLvl() <= MSG::DEBUG) msg() << MSG::VERBOSE
		<< spacepointCollection->size() <<" SpacePoints successfully added to Container !"
		<< endreq;
          }
	  else{
	    if (msgLvl() <= MSG::VERBOSE)
	      msg() << MSG::VERBOSE << "Failed to add SCT SpacePoints to container" << endreq;
            delete spacepointCollection;
          }
        }    


      }
      if (m_nSCT==0 && msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "SiTrigTrackerSpacePointFinder algorithm found no SCT space points"
	      << endreq;

      if (m_spOverlapColl && m_spOverlapColl->size() !=0 ) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << m_spOverlapColl->size()
		<< "overlap space points registered." << endreq;
      }
      else {
	if (msgLvl() <= MSG::DEBUG && m_spOverlapColl)
	  msg() << MSG::DEBUG << "No "
		<< "overlap space points registered." << endreq;
      }

      if(doTiming()) m_timerFindSi->pause();
    }
    
    if (msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << "REGTEST: Received " << m_nReceivedClustersSCT << " clusters "
	    << endreq;
      msg() << MSG::DEBUG 
	    << "REGTEST: Found " << m_nSCT << " SCT space points" << endreq;

      if (m_spOverlapColl && m_spOverlapColl->size() !=0 ) {
	msg() << MSG::DEBUG << m_spOverlapColl->size()
	      << "overlap space points registered." << endreq;
      }
      else {
      if (m_spOverlapColl)
	msg() << MSG::DEBUG << "No "
	      << "overlap space points registered." << endreq;
      }
    }


  } //end of SCT part
  
  // Reconstruct Pixel clusters:
  //---------------------------

  bool doPixel=true;
  if(m_selectPixels){

    // Retrieve the Identifiable cluster container from StoreGate.
    // If it is not found, do not reconstruct pixels.
    // -----------------------------------------------------------
    if(doTiming()) m_timerRtrPx->resume();
    sc =store()->retrieve(m_pixelClusterContainer, m_pixelClustersName);
    if (sc.isFailure()  || !m_pixelClusterContainer ){
      msg() << MSG::WARNING 
	    <<"Pixel Cluster container not found"<< endreq;
      doPixel=false;
    }else{
      if(msgLvl() <= MSG::DEBUG)
	msg() <<MSG::DEBUG <<"Pixel Cluster container found" <<endreq;
    }
    if(doTiming()) m_timerRtrPx->pause();
  }

  if (m_selectPixels && doPixel) {

    if(!m_doFullScan){
      if(doTiming()) m_timerRegSel->resume();
      
      m_regionSelector->DetHashIDList( PIXEL, 
				       *roi, 
				       m_listOfPixIds);
      
      if(doTiming()) m_timerRegSel->pause();
    }
    
    m_nPix=0;
    m_nReceivedClustersPix=0;
    if(!m_doFullScan){

      if(doTiming()) m_timerFindPx->resume();

      for(std::vector<IdentifierHash>::iterator itIDs =  m_listOfPixIds.begin(); 
	  itIDs != m_listOfPixIds.end(); itIDs++){

	// Retrieve the clusterCollection:
	PixelClusterContainer::const_iterator 
	  itPixClusters(m_pixelClusterContainer->indexFind(*itIDs));
		
	if( itPixClusters == m_pixelClusterContainer->end()) continue;
	
	const PixelClusterCollection * pixelClusterCollection = 
	  &(**itPixClusters);

	if( pixelClusterCollection->begin() == pixelClusterCollection->end())
	  continue;

	m_nReceivedClustersPix += pixelClusterCollection->size(); 
	
	// Create SpacePointCollection
	IdentifierHash idHash = pixelClusterCollection->identifyHash(); 
	Identifier elementID = pixelClusterCollection->identify(); 
	SpacePointCollection* spacepointCollection = 
	  new SpacePointCollection(idHash); 
	spacepointCollection->setIdentifier(elementID); 
      
	if (pixelClusterCollection->size() != 0) {
		
	  m_SiSpacePointMakerTool->
	    fillPixelSpacePointCollection(pixelClusterCollection,
					  spacepointCollection);
	}
	
	if (spacepointCollection->size() != 0){
	  
	  sc = m_SpacePointContainerPixel->addCollection(spacepointCollection,
							 spacepointCollection->identifyHash());
	  
	  m_nPix += spacepointCollection->size();

	  m_SPindentifierPix.push_back(spacepointCollection->identifyHash() );
	  
	  if(!sc.isSuccess()){
	    if (msgLvl() <= MSG::VERBOSE)
	      msg() << MSG::VERBOSE << "Failed to add Pixel SpacePoints to container : "
		    << spacepointCollection->identifyHash() << endreq;
	    delete spacepointCollection;
	  }
	} 
	else{
	  // -me- cleanup memory
	  delete (spacepointCollection);
	}
      }
      if(doTiming()) m_timerFindPx->pause();
    }
    else{ //for FullScan

      // loop over Pixel clusterCollections
      PixelClusterContainer::const_iterator colNext  = m_pixelClusterContainer->begin();
      PixelClusterContainer::const_iterator lastCol  = m_pixelClusterContainer->end();
      
      if(doTiming()) m_timerFindPx->resume();

      for (; colNext != lastCol; ++colNext) {
	
	// Create SpacePointCollection
	IdentifierHash idHash = (*colNext)->identifyHash(); 
	Identifier elementID = (*colNext)->identify();
	
	SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash); 
	spacepointCollection->setIdentifier(elementID); 
	
	if ((*colNext)->size() != 0) {
	  m_nReceivedClustersPix += (*colNext)->size();

	  m_SiSpacePointMakerTool->fillPixelSpacePointCollection(*colNext,spacepointCollection);

	}
	else{
	  if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Empty pixel cluster collection"<<endreq;
	}
	
	if (spacepointCollection->size() == 0){
	  // -me- clean up memory
	  delete (spacepointCollection);
	}
	else{    
	  // -me- new IDC does no longer register in Storegate if hash is used !
	  // sc= m_SpacePointContainerPixel->addCollection( spacepointCollection, spacepointCollection->identify() );
	
	  sc= m_SpacePointContainerPixel->addCollection( spacepointCollection, spacepointCollection->identifyHash() );
	  
	  m_nPix += spacepointCollection->size();
	  
	  if (sc.isFailure()){
	    if (msgLvl() <= MSG::VERBOSE)
	      msg() << MSG::VERBOSE
		    << "Failed to add Pixel SpacePoints to container" << endreq;
	    delete spacepointCollection;
	  }
	  else{
	    if (msgLvl() <= MSG::VERBOSE)
	      msg() << MSG::VERBOSE << spacepointCollection->size()
		    << " SpacePoints successfully added to Container !" << endreq;
	  }
	}
      }	
      if (m_nPix ==0 && msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG
	      << "SiTrackerSpacePointFinder algorithm found no Pixel space points" << endreq;
      if(doTiming()) m_timerFindPx->pause();
    }

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: Found " << m_nPix << " Pixel space points" << endreq;
  } //end of Pixel part

  if(doTiming()){
    m_timerRegSel->stop();
    m_timerRtrSi->stop();
    m_timerSGate->stop();
    m_timerRtrPx->stop();
    m_timerFindSi->stop();
    m_timerFindPx->stop();
  }
  
  return HLT::OK;
}

//---------------------------------------------------------------------------
HLT::ErrorCode SiTrigSpacePointFinder::hltFinalize(){

  msg() << MSG::INFO << "SiTrigSpacePointFinder::finalize()" << endreq;

  m_SpacePointContainerPixel->cleanup();
  m_SpacePointContainerSCT->cleanup();

  //igb -fix me - it crashes
  m_SpacePointContainerPixel->release();
  m_SpacePointContainerSCT->release();

  //delete m_SpacePointContainerPixel;
  //delete m_SpacePointContainerSCT;
  
  return HLT::OK;
}

//----------------------------------
//          endRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode SiTrigSpacePointFinder::hltEndRun(){
 
 // Get the messaging service, print where you are
  msg() << MSG::INFO << "SiTrigSpacePointFinder::endRun()" << endreq;
 
  return HLT::OK;
}
//---------------------------------------------------------------------------
}
