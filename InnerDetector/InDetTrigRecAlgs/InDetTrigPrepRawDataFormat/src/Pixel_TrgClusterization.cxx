/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for Pixel_TrgClusterization.
// (see header file)
//****************************************************************************

#include "EventContainers/IdentifiableContainer.h"
// Silicon trackers includes
#include "InDetTrigPrepRawDataFormat/Pixel_TrgClusterization.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"

#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"

#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"


#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/PixelRDORawData.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"    

//
#include "InDetIdentifier/PixelID.h"

// Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/Timeout.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

namespace InDet{
  
  using namespace InDet;

  //----------------------------------  
  //           Constructor:
  //----------------------------------------------------------------------------
  Pixel_TrgClusterization::Pixel_TrgClusterization(const std::string &name, 
						   ISvcLocator *pSvcLocator) :
    //----------------------------------------------------------------------------
    HLT::FexAlgo(name,pSvcLocator),
    m_rawDataProvider("InDet::TrigPixRawDataProvider"),
    m_gangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder"),
    m_clusteringTool("InDet::MergedPixelsTool"),
    m_pixelRDOContainerName("PixelRDOs"),    // RDO container
    m_managerName("Pixel"),
    m_clustersName("PixelTrigClusters"),
    m_ambiguitiesMapName("TrigPixelClusterAmbiguitiesMap"),
    m_regionSelector("RegSelSvc", name),
    m_doFullScan(false),
    m_etaHalfWidth(0.1),
    m_phiHalfWidth(0.1),
    m_bsErrorSvc("PixelByteStreamErrorsSvc",name),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_doTimeOutChecks(true)
  {  
    // Get parameter values from jobOptions file
    declareProperty("DetectorManagerName",     m_managerName);
    declareProperty("clusteringTool",          m_clusteringTool);
    declareProperty("gangedAmbiguitiesFinder", m_gangedAmbiguitiesFinder);
    declareProperty("ClustersName",            m_clustersName);
    declareProperty("AmbiguitiesMapName",      m_ambiguitiesMapName);

    declareProperty("RegionSelectorTool",      m_regionSelector);
    declareProperty("doFullScan",              m_doFullScan);
    declareProperty("EtaHalfWidth",            m_etaHalfWidth);
    declareProperty("PhiHalfWidth",            m_phiHalfWidth);
    declareProperty("Pixel_RDOContainerName",  m_pixelRDOContainerName);
    declareProperty("RawDataProvider",         m_rawDataProvider);
    declareProperty("doTimeOutChecks",         m_doTimeOutChecks);

    declareMonitoredVariable("numPixClusters", m_numPixClusters    );
    declareMonitoredVariable("numPixIds", m_numPixIds    );
    declareMonitoredStdContainer("PixHashId", m_ClusHashId);
    declareMonitoredStdContainer("PixBSErr", m_PixBSErr);
  
    m_clusterCollection = NULL;
    
    // error strategy
    //
    // 0 : broad errors (cluster width/sqrt(12) )
    //     as in old clustering code (release 6 and 7)
    // 1 : narrow errors (pixel pitch/sqrt(12.) )
    //     DEFAULT - should be more accurate, 
    //     and still conservative
    // 2 : parameterization as a function of eta & cluster size
    //     same as in atrecon (fortran code)
    //declareProperty("ErrorStrategy",m_errorStrategy); <-- this is now in the MergedPixelTool

    // position strategy
    //
    // 0 : arithmetic mean of pixel position
    // 1 : simple charge interpolation
    
    // declareProperty("PositionStrategy",m_positionStrategy); <-- this is now in the MergedPixelTool

    //timers
    m_timerRegSel = m_timerDecoder = 0;
    m_timerCluster = m_timerSGate = 0;

  }

  //----------------------------------  
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltBeginRun()
  {

    // Get the messaging service, print where you are
    msg() << MSG::INFO << "Pixel_TrgClusterization::hltBeginRun() configured with";
    msg() << MSG::INFO << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth;
    if (m_doFullScan) msg() << MSG::INFO << "FullScan mode";
    msg() << MSG::INFO << "will be driven by RoI objects" << endreq;

    /*
    StatusCode sc = m_rawDataProvider->initContainer();
    if (sc.isFailure())
      msg() << MSG::WARNING << "RDO container cannot be registered" << endreq;
    */
    return HLT::OK;
  }

  //---------------------------------------------------------------------------- 
  //          Initialize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltInitialize()
  //----------------------------------------------------------------------------
  {

    //using namespace InDetReco;
	
    msg() << MSG::INFO << "Pixel_TrgClusterization::hltInitialize()" << endreq;
  

    // get the InDet::MergedPixelsTool
    if ( m_clusteringTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << m_clusteringTool.propertyName() << ": Failed to retrieve tool " << m_clusteringTool.type() << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      msg() << MSG::INFO << m_clusteringTool.propertyName() << ": Retrieved tool " << m_clusteringTool.type() << endreq;
    }

    // get the InDet::PixelGangedAmbiguitiesFinder
    if ( m_gangedAmbiguitiesFinder.retrieve().isFailure() ) {
      msg() << MSG::FATAL << m_gangedAmbiguitiesFinder.propertyName() << ": Failed to retrieve tool " << m_gangedAmbiguitiesFinder.type() << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      msg() << MSG::INFO << m_gangedAmbiguitiesFinder.propertyName() << ": Retrieved tool " << m_gangedAmbiguitiesFinder.type() << endreq;
    }

    // get the detector description.
    StoreGateSvc* detStore(0);
    StatusCode sc = service("DetectorStore", detStore);
    if (sc.isFailure()){
      msg() << MSG::FATAL << "Detector service not found !" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Detector service found" << endreq;

    sc =detStore->retrieve(m_manager,m_managerName);
    if (sc.isFailure()){
      msg() << MSG::FATAL << "Cannot retrieve the Pixel detector manager " 
	    << m_managerName << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Pixel detector manager retrieved!" << endreq;
    }

    const PixelID * IdHelper(0);
    if (detStore->retrieve(IdHelper, "PixelID").isFailure()) {
      msg() << MSG::FATAL << "Could not get SCT ID helper" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    m_idHelper = IdHelper;

    // register the IdentifiableContainer into StoreGate
    // ------------------------------------------------------
  
    if(!store()->transientContains<PixelClusterContainer>(m_clustersName)){
  
      // declare the container:
      m_clusterContainer = new PixelClusterContainer(m_idHelper->wafer_hash_max());
  
      if (store()->record(m_clusterContainer, m_clustersName).isFailure()) {
	msg() << MSG::WARNING << " Container " << m_clustersName
	      << " could not be recorded in StoreGate !" 
	      << endreq;
      } 
      else {
	msg() << MSG::INFO << "Container " << m_clustersName 
	      << " registered  in StoreGate" << endreq;  
      }
    }
    else {    
      sc = store()->retrieve(m_clusterContainer, m_clustersName);

      if (sc.isFailure()) {
	msg() << MSG::ERROR << "Failed to get Cluster Container" << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else { 
	msg() << MSG::INFO << "Got Cluster Container from TDS: " << m_clustersName << endreq;
      }
    }
    m_clusterContainer->addRef();

    // symlink the collection
    const SiClusterContainer* symSiContainer(0);
    sc = store()->symLink(m_clusterContainer,symSiContainer);
    if (sc.isFailure()) {
      msg() << MSG::WARNING
	    << "Pixel clusters could not be symlinked in StoreGate !"
	    << endreq;
    } 
    else if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Pixel clusters '" << m_clustersName 
	    << "' symlinked in StoreGate" << endreq;
    }

    if(!m_doFullScan){
      // Retrieving Region Selector Tool:
      if ( m_regionSelector.retrieve().isFailure() ) {
	msg() << MSG::FATAL 
	      << m_regionSelector.propertyName()
	      << " : Unable to retrieve RegionSelector tool "  
	      << m_regionSelector.type() << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
    }
    else{
      msg() << MSG::INFO 
	    << "RegionSelector tool not needed for FullScan"  << endreq;
    }  

    //retrieve rob data provider service
    if (m_robDataProvider.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to retrieve " << m_robDataProvider << endreq;
      return StatusCode::FAILURE;
    } else
      msg() << MSG::INFO << "Retrieved service " << m_robDataProvider << " in Pixel_trgClusterization. " << endreq;

    //decoding tool
    if (m_rawDataProvider.retrieve().isFailure()){
      msg() << MSG::ERROR 
	    << "Raw data provider not available"  << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }


    //BS Error Svc
    if (m_bsErrorSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Could not retrieve " << m_bsErrorSvc << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    // Timers:
    m_timerRegSel  = addTimer("RegSel");
    m_timerDecoder    = addTimer("Decoder");
    m_timerCluster = addTimer("Cluster");
    m_timerSGate   = addTimer("SGate");

    return HLT::OK;
  }

  //--------------------------------
  //         Execute method:
  //--------------------------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltExecute(const HLT::TriggerElement*,
						     HLT::TriggerElement* outputTE){
    //----------------------------------------------------------------------------

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: Pixel_TrgClusterization::HLTexecute()" << endreq;
  
    //initialisation of monitored quantities
    m_numPixIds = 0;
    m_numPixClusters = 0;
    m_clusterCollection = NULL;
    m_listOfPixIds.clear();
    m_ClusHashId.clear();
    m_PixBSErr.clear();


    if(doTiming()) m_timerSGate->resume();

    // register the IdentifiableContainer into StoreGate
    // ------------------------------------------------------

    if(!store()->transientContains<PixelClusterContainer>(m_clustersName)) { 

      // The cluster container is cleant up only at the begining of the event.
      m_clusterContainer->cleanup();
      if (store()->record(m_clusterContainer,
			  m_clustersName,false).isFailure()) {

	msg() << MSG::WARNING << " Container " << m_clustersName
	      << " could not be recorded in StoreGate !" 
	      << endreq;
      } 
      else{
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " REGTEST: Cluster Container saved in StoreGate "
		<< endreq;
      }


    }
    else {  
      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST: Container " << m_clustersName 
	      << " existed already in StoreGate " << endreq;
    }
 
    // ------------------------------------------------------
    // create and register the ganged pixel ambiguities map into SG
    // ------------------------------------------------------

    PixelGangedClusterAmbiguities* AmbiguitiesMap; 

    if(!store()->transientContains<PixelGangedClusterAmbiguities>(m_ambiguitiesMapName)){

      AmbiguitiesMap = new PixelGangedClusterAmbiguities();
      if (store()->record(AmbiguitiesMap,m_ambiguitiesMapName,true,true).isFailure()){
	msg() << MSG::WARNING
	      <<m_ambiguitiesMapName <<" could not be recorded in StoreGate !"
	      << endreq;
	return HLT::OK;
      }
      else{
	if (msgLvl() <= MSG::DEBUG) 
	  msg() << MSG::DEBUG << "REGTEST: " << m_ambiguitiesMapName
		<< " recorded in StoreGate" << endreq;
      }
    }
    else{
      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST: " << m_ambiguitiesMapName
	      << " existed already in StoreGate" << endreq;

      if(!store()->retrieve(AmbiguitiesMap, m_ambiguitiesMapName)){
	msg() << MSG::WARNING
	      <<m_ambiguitiesMapName <<" could not be recorded in StoreGate !"
	      << endreq;
	return HLT::OK;
      }
    }
     
    if(doTiming()) m_timerSGate->pause();

    //---------------------------
    // Trigger specific part:
    //---------------------------
    //   Get from the Trigger Element the RoI
    //   Ask the Region Selector for the list of hash indexes for this ROI.
    //   Transform those indexes into a RDO collection.
    //-------------------------------------------------------------------------

    //handling of decoding problems
    StatusCode scdec = StatusCode::SUCCESS;
    m_bsErrorSvc->resetCounts();


    // Get RoiDescriptor
    const TrigRoiDescriptor* roi;
    if ( HLT::OK != getFeature(outputTE, roi) ) {
      msg() << MSG::WARNING << "Can't get RoI" << endreq;
      return HLT::NAV_ERROR;
    }
    
    if (!roi){
      msg() << MSG::WARNING << "Received NULL RoI" << endreq;
      return HLT::NAV_ERROR;
    }
 
    double RoiEtaWidth = roi->etaPlus() - roi->etaMinus();
    double RoiPhiWidth = roi->phiPlus() - roi->phiMinus();
    const double M_2PI = 2*M_PI;
    const float  M_PIF = float(M_PI);
    while ( RoiPhiWidth> M_PIF ) RoiPhiWidth -= M_2PI;
    while ( RoiPhiWidth<-M_PIF ) RoiPhiWidth += M_2PI;
    if ( RoiPhiWidth> M_PI ) RoiPhiWidth -= 1e-7;
    if ( RoiPhiWidth<-M_PI ) RoiPhiWidth += 1e-7;

    if (!roi->isFullscan()){
      if( fabs(RoiEtaWidth/2. - m_etaHalfWidth) > 0.02) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "ROI range is different from configuration: " << endreq;
	  msg() << MSG::DEBUG << "eta width = " << RoiEtaWidth << "; with etaPlus = " << roi->etaPlus() << "; etaMinus = " << roi->etaMinus() << endreq;
	  msg() << MSG::DEBUG << "etaHalfWidth from config: " << m_etaHalfWidth << endreq;
	}
      }

    }
    else {
      if (m_etaHalfWidth<2.5 || m_phiHalfWidth<3.) {
	msg() << MSG::WARNING << "FullScan RoI and etaHalfWidth from config: " << m_etaHalfWidth << endreq;
	msg() << MSG::WARNING << "FullScan RoI and phiHalfWidth from config: " << m_phiHalfWidth << endreq;
      }
    }

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:" << *roi << endreq;
    }
    
    if(doTiming()) m_timerDecoder->start();
    scdec = m_rawDataProvider->decode(roi);
    if(doTiming()) m_timerDecoder->stop();

    
    //   Get the Pixel RDO's:
    //     - First get the Pixel ID's using the RegionSelector
    //     - Retrieve from SG the RDOContainer: 
    //       Identifiable Container that contains pointers to all the RDO 
    //        collections (one collection per detector)
    //     - Retrieve from StoreGate the RDO collections.
    //       (the ByteStreamConvertors are called here).
    
    
    if (!(roi->isFullscan())){
      if(doTiming()) m_timerRegSel->start();
      m_regionSelector->DetHashIDList( PIXEL, *roi, m_listOfPixIds);
      m_numPixIds = m_listOfPixIds.size();
      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST: Pixel : Roi contains " 
	      << m_numPixIds << " det. Elements" << endreq;
      
      if(doTiming()) m_timerRegSel->stop();
    }


    if (scdec.isSuccess()){
      //check for recoverable errors

      int n_err_total = 0;
       
      int bsErrors[IPixelByteStreamErrorsSvc::lastErrType+1];
      
      for (size_t idx = 0; idx<=size_t(IPixelByteStreamErrorsSvc::lastErrType); idx++){
	int n_errors = m_bsErrorSvc->getNumberOfErrors(idx);
	n_err_total += n_errors;
	bsErrors[idx] = n_errors;
      }

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "decoding errors: "  << n_err_total;

      if (n_err_total){
	for (size_t idx = 0; idx<=size_t(IPixelByteStreamErrorsSvc::lastErrType); idx++){
	  //	  m_PixBSErr.push_back(bsErrors[idx]);
	  if (bsErrors[idx])
	    m_PixBSErr.push_back(idx);
	  msg() << MSG::DEBUG << " " << bsErrors[idx];
	}
      }

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << endreq;


    } else {
      msg() << MSG::DEBUG << " m_rawDataProvider->decode failed" << endreq;
    }


    if(doTiming()) m_timerSGate->resume();
    
    PixelRDO_Container* p_pixelRDOContainer;
    if (store()->retrieve(p_pixelRDOContainer,  m_pixelRDOContainerName).isFailure() ) {
      msg() << MSG::WARNING << "Could not find the PixelRDO_Container " 
	    << m_pixelRDOContainerName << endreq;
    
      // Activate the TriggerElement anyhow.
      // (FEX algorithms should not cut and it could be that in the SCT/TRT 
      // tracks can anyhow be reconstructed) 
    
      return HLT::OK;
    } 
    else if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " REGTEST: Found the Pixel_RDO_Container " 
	    << m_pixelRDOContainerName << endreq;
    }
    if(doTiming()) m_timerSGate->pause();


    if (!(roi->isFullscan())){

      if(doTiming()) m_timerCluster->resume();

      for (unsigned int i=0; i<m_listOfPixIds.size(); i++) {

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  msg() << MSG::WARNING << "Timeout reached. Aborting sequence." << endreq;
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}
 
	PixelRDO_Container::const_iterator 
	  RDO_collection_iter = p_pixelRDOContainer->indexFind(m_listOfPixIds[i]); 
      
	if (RDO_collection_iter == p_pixelRDOContainer->end()) continue;
      
	const InDetRawDataCollection<PixelRDORawData>* RDO_Collection (*RDO_collection_iter);
      
	if (!RDO_Collection) continue;
      
	if (msgLvl() <= MSG::DEBUG){
	  msg() << MSG::VERBOSE << "RDO collection size="
		<< RDO_Collection->size()
		<< ", ID=" << RDO_Collection->identify() << endreq;
	  msg() << MSG::VERBOSE << "Ambiguities map: " 
		<< AmbiguitiesMap->size() << " elements" << endreq;
	}
      
	if (RDO_Collection->size() != 0){
	
	  m_clusterCollection = m_clusteringTool->clusterize(*RDO_Collection,*m_manager,*m_idHelper);
	
	  if (m_clusterCollection && m_clusterCollection->size()!=0){ 
	  
	  
	    m_numPixClusters+= m_clusterCollection->size();
	  
	    /*if ( m_clusterContainer->indexFind(clusterCollection->identifyHash()) != m_clusterContainer->end() )
	      msg() << MSG::INFO << "FOUND Cluster collection :" << clusterCollection->identifyHash() << endreq; */
	  
	    if (m_clusterContainer->addCollection( m_clusterCollection, 
						   m_clusterCollection->identifyHash() ).isFailure()) {
	      if (msgLvl() <= MSG::VERBOSE)
		msg() << MSG::VERBOSE << "Failed to add Cluster collection : "
		      << m_clusterCollection->identifyHash() << endreq;
	      delete m_clusterCollection;
	    } else {
	      m_ClusHashId.push_back(m_clusterCollection->identifyHash());
	      if (msgLvl() <= MSG::VERBOSE)
		msg() << MSG::VERBOSE << "Filling ambiguities map" << endreq;
	      
	      const size_t ambiMapSize = AmbiguitiesMap->size();
	      m_gangedAmbiguitiesFinder->execute(m_clusterCollection,
						 *m_manager, *AmbiguitiesMap);
	      
	      if (msgLvl() <= MSG::DEBUG && ambiMapSize != AmbiguitiesMap->size())
		msg() << MSG::DEBUG << "After filling - Ambiguities map: " 
		      << AmbiguitiesMap->size() << " elements" << endreq;
	    }
	  } 
	  else {
	    if (msgLvl() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Clustering algorithm found no clusters"
		    << endreq;
	    delete m_clusterCollection;
	  }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }
    else{      //fullscan

      //p_pixelRDOContainer->clID(); 	// anything to dereference the DataHandle
      // will trigger the converter
    
      PixelRDO_Container::const_iterator rdoCollections      = p_pixelRDOContainer->begin();
      PixelRDO_Container::const_iterator rdoCollectionsEnd   = p_pixelRDOContainer->end();
    
      AtlasDetectorID detType;

      if(doTiming()) m_timerCluster->resume();

      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections){
	const InDetRawDataCollection<PixelRDORawData>* RDO_Collection(*rdoCollections);
	if (!RDO_Collection) continue;

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  msg() << MSG::WARNING << "Timeout reached. Aborting sequence." << endreq;
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	if (msgLvl() <= MSG::DEBUG){
	  msg() << MSG::VERBOSE << "RDO collection size="
		<< RDO_Collection->size()
		<< ", ID=" << RDO_Collection->identify() << endreq;
	  msg() << MSG::VERBOSE << "Ambiguities map: " 
		<< AmbiguitiesMap->size() << " elements" << endreq;
	}

	if (RDO_Collection->size() != 0){
	  // Use one of the specific clustering AlgTools to make clusters
	  m_clusterCollection = m_clusteringTool->clusterize(*RDO_Collection,*m_manager,
							     *m_idHelper);
	
	  if (m_clusterCollection){
	    if (m_clusterCollection->size() != 0) {

	    
	      m_numPixClusters+= m_clusterCollection->size();


	      if (m_clusterContainer->addCollection( m_clusterCollection, 
						     m_clusterCollection->identifyHash() ).isSuccess()){
		m_ClusHashId.push_back(m_clusterCollection->identifyHash());
		if (msgLvl() <= MSG::DEBUG)	    
		  msg() << MSG::VERBOSE << "Filling ambiguities map" << endreq;  
		
		m_gangedAmbiguitiesFinder->execute(m_clusterCollection,
						   *m_manager,
						   *AmbiguitiesMap);
		if (msgLvl() <= MSG::DEBUG)
		  msg() << MSG::VERBOSE << "Ambiguities map: " 
			<< AmbiguitiesMap->size() << " elements" << endreq;
	      
		if (msgLvl() <= MSG::DEBUG)
		  msg() << MSG::VERBOSE
			<< "Clusters successfully added to Container !"
			<< endreq;
	      }
	      else{
		if (msgLvl() <= MSG::VERBOSE){
		  msg() << MSG::VERBOSE << "Failed to add Clusters to container"
			<< endreq;
		}
		delete m_clusterCollection;
		//return StatusCode::FAILURE;
	      }
	    } 
	    else{
	      if (msgLvl() <= MSG::DEBUG)
		msg() << MSG::DEBUG << "Clustering algorithm found no clusters"
		      << endreq;
	      // -me- clean up memory
	      delete m_clusterCollection;
	    }
	  }
	}
      }  
      if(doTiming()) m_timerCluster->pause();
    }

    if(doTiming()){
      m_timerCluster->stop();
      m_timerSGate->stop();
    }

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Number of reconstructed clusters = " 
	    << m_numPixClusters << endreq;
  

    return HLT::OK;
  }
  //-----------------------------------
  //          Finalize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltFinalize() 
  //----------------------------------------------------------------------------
  {
  
    msg() << MSG::INFO << "Pixel_TrgClusterization::finalize()" << endreq;

    m_clusterContainer->cleanup();    
	
    //delete m_globalPosAlg;
    //delete m_clusterContainer;

    //igb -fix me- it crashes
    m_clusterContainer->release();

    return HLT::OK;
  }
  //----------------------------------  
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltEndRun()
  {

    msg() << MSG::INFO << "Pixel_TrgClusterization::endRun()" << endreq;

    return HLT::OK;
  }

  //---------------------------------------------------------------------------


  //-------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::prepareRobRequests(const HLT::TriggerElement* inputTE ) {

    msg() << MSG::INFO << "Running prepareRobRequests in Pixel_trgClusterization. " << endreq;
 
    // Calculate ROBs needed  - this code should be shared with hltExecute to avoid slightly different requests
    const TrigRoiDescriptor* roi = 0;

    if (getFeature(inputTE, roi, "forID") != HLT::OK || roi == 0) {
      getFeature(inputTE, roi);
    }
 
    if ( roi==NULL ) { 
      msg() <<  MSG::WARNING << "REGTEST / Failed to find RoiDescriptor " << endreq;
      return HLT::NAV_ERROR;
    }

    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST prepareROBs / event"
				      << " RoI id " << roi->roiId()
				      << " located at   phi = " <<  roi->phi()
				      << ", eta = " << roi->eta()
				      << endreq;

    //const TrigRoiDescriptor fs(true);

    std::vector<unsigned int> uIntListOfRobs;
    m_regionSelector->DetROBIDListUint( PIXEL, *roi, uIntListOfRobs );
    //m_regionSelector->DetROBIDListUint( PIXEL, fs, uIntListOfRobs );

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "list of pre-registered ROB ID in PIX: "  << endreq;
      for(uint i_lid(0); i_lid<uIntListOfRobs.size(); i_lid++)
	msg() << MSG::DEBUG << uIntListOfRobs.at(i_lid) << endreq;
    }

    //m_robDataProvider->addROBData( uIntListOfRobs );
    config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
    uIntListOfRobs.clear();

    return HLT::OK;

  }


}
