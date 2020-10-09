/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"

#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"


#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/PixelRDORawData.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"    

//
#include "InDetIdentifier/PixelID.h"

// Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

// Gaudi includes
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
    m_clustersName("PixelTrigClusters"),
    m_ambiguitiesMapName("TrigPixelClusterAmbiguitiesMap"),
    m_doFullScan(false),
    m_etaHalfWidth(0.1),
    m_phiHalfWidth(0.1),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_doTimeOutChecks(true),
    m_skipBSDecoding(false)
  {  
    // Get parameter values from jobOptions file
    declareProperty("clusteringTool",          m_clusteringTool);
    declareProperty("gangedAmbiguitiesFinder", m_gangedAmbiguitiesFinder);
    declareProperty("ClustersName",            m_clustersName);
    declareProperty("AmbiguitiesMapName",      m_ambiguitiesMapName);

    declareProperty("doFullScan",              m_doFullScan);
    declareProperty("EtaHalfWidth",            m_etaHalfWidth);
    declareProperty("PhiHalfWidth",            m_phiHalfWidth);
    declareProperty("Pixel_RDOContainerName",  m_pixelRDOContainerName);
    declareProperty("RawDataProvider",         m_rawDataProvider);
    declareProperty("doTimeOutChecks",         m_doTimeOutChecks);
    declareProperty("skipBSDecoding",          m_skipBSDecoding);

    declareMonitoredVariable("numPixClusters", m_numPixClusters    );
    declareMonitoredVariable("numPixIds", m_numPixIds    );
    declareMonitoredStdContainer("PixHashId", m_ClusHashId);
    declareMonitoredStdContainer("PixelByteStreamErrs", m_PixBSErr);
  
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

  //---------------------------------------------------------------------------- 
  //          Initialize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltInitialize()
  //----------------------------------------------------------------------------
  {

    //using namespace InDetReco;
	
    ATH_MSG_INFO( "Pixel_TrgClusterization::hltInitialize()" );
  

    // get the InDet::MergedPixelsTool
    if ( m_clusteringTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_clusteringTool.propertyName() << ": Failed to retrieve tool " << m_clusteringTool.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      ATH_MSG_INFO( m_clusteringTool.propertyName() << ": Retrieved tool " << m_clusteringTool.type() );
    }

    // get the InDet::PixelGangedAmbiguitiesFinder
    if ( m_gangedAmbiguitiesFinder.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_gangedAmbiguitiesFinder.propertyName() << ": Failed to retrieve tool " << m_gangedAmbiguitiesFinder.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      ATH_MSG_INFO( m_gangedAmbiguitiesFinder.propertyName() << ": Retrieved tool " << m_gangedAmbiguitiesFinder.type() );
    }

    const PixelID * IdHelper(0);
    if (detStore()->retrieve(IdHelper, "PixelID").isFailure()) {
      ATH_MSG_FATAL( "Could not get PixelID helper" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    m_idHelper = IdHelper;

    // register the IdentifiableContainer into StoreGate
    // ------------------------------------------------------
  
    if(!store()->transientContains<PixelClusterContainer>(m_clustersName)){
      
      // declare the container:
      m_clusterContainer = new PixelClusterContainer(m_idHelper->wafer_hash_max());
      
      if (store()->record(m_clusterContainer, m_clustersName).isFailure()) {
	ATH_MSG_WARNING( " Container " << m_clustersName
			 << " could not be recorded in StoreGate !" );
      } 
      else {
	ATH_MSG_INFO( "Container " << m_clustersName 
		      << " registered  in StoreGate" );  
      }
    }
    else {    
      StatusCode sc = store()->retrieve(m_clusterContainer, m_clustersName);
      
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to get Cluster Container" );
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else { 
	ATH_MSG_INFO( "Got Cluster Container from TDS: " << m_clustersName );
      }
      
    }

    m_clusterContainer->addRef();
      
    // symlink the collection
    const SiClusterContainer* symSiContainer(0);
    StatusCode sc = store()->symLink(m_clusterContainer,symSiContainer);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Pixel clusters could not be symlinked in StoreGate !" );
    } else {
      ATH_MSG_DEBUG( "Pixel clusters '" << m_clustersName 
		     << "' symlinked in StoreGate" );
    }
    
    // Retrieving Region Selector Tool:
    if ( m_regionSelector.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_regionSelector.propertyName()
		     << " : Unable to retrieve RegionSelector tool "  
		     << m_regionSelector.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    //retrieve rob data provider service
    if (m_robDataProvider.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to retrieve " << m_robDataProvider );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else
      ATH_MSG_INFO( "Retrieved service " << m_robDataProvider << " in Pixel_trgClusterization. " );

    //decoding tool
    if (m_rawDataProvider.retrieve().isFailure()){
      ATH_MSG_ERROR( "Raw data provider not available" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }


    //BS Error Svc
    if (m_bsError.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_bsError );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    // Get the messaging service, print where you are
    ATH_MSG_INFO( "Pixel_TrgClusterization configured with:"
                  << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth );
    if (m_doFullScan) ATH_MSG_INFO( "FullScan mode" );
    ATH_MSG_INFO( "will be driven by RoI objects" );

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


    ATH_MSG_DEBUG( "REGTEST: Pixel_TrgClusterization::HLTexecute()" );
  
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

	ATH_MSG_WARNING( " Container " << m_clustersName
			 << " could not be recorded in StoreGate !" );
      } 
      else{
	ATH_MSG_DEBUG( " REGTEST: Cluster Container saved in StoreGate " );
      }


    }
    else {  
      ATH_MSG_DEBUG( "REGTEST: Container " << m_clustersName 
		     << " existed already in StoreGate " );
    }
 
    // ------------------------------------------------------
    // create and register the ganged pixel ambiguities map into SG
    // ------------------------------------------------------

    PixelGangedClusterAmbiguities* AmbiguitiesMap; 

    if(!store()->transientContains<PixelGangedClusterAmbiguities>(m_ambiguitiesMapName)){

      AmbiguitiesMap = new PixelGangedClusterAmbiguities();
      if (store()->record(AmbiguitiesMap,m_ambiguitiesMapName,true,true).isFailure()){
	ATH_MSG_WARNING( m_ambiguitiesMapName <<" could not be recorded in StoreGate !" );
	return HLT::OK;
      }
      else{
	ATH_MSG_DEBUG( "REGTEST: " << m_ambiguitiesMapName
		       << " recorded in StoreGate" );
      }
    }
    else{
      ATH_MSG_DEBUG( "REGTEST: " << m_ambiguitiesMapName
		     << " existed already in StoreGate" );

      if(!store()->retrieve(AmbiguitiesMap, m_ambiguitiesMapName)){
	ATH_MSG_WARNING(m_ambiguitiesMapName <<" could not be recorded in StoreGate !" );
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
    m_bsError->resetCounts();


    // Get RoiDescriptor
    const TrigRoiDescriptor* roi;
    if ( HLT::OK != getFeature(outputTE, roi) ) {
      ATH_MSG_WARNING( "Can't get RoI" );
      return HLT::NAV_ERROR;
    }
    
    if (!roi){
      ATH_MSG_WARNING( "Received NULL RoI" );
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
	ATH_MSG_DEBUG( "ROI range is different from configuration: " );
	ATH_MSG_DEBUG( "eta width = " << RoiEtaWidth << "; with etaPlus = " << roi->etaPlus() << "; etaMinus = " << roi->etaMinus() );
	ATH_MSG_DEBUG( "etaHalfWidth from config: " << m_etaHalfWidth );
      }
    }
    else {
      if (m_etaHalfWidth<2.5 || m_phiHalfWidth<3.) {
	ATH_MSG_WARNING( "FullScan RoI and etaHalfWidth from config: " << m_etaHalfWidth );
	ATH_MSG_WARNING( "FullScan RoI and phiHalfWidth from config: " << m_phiHalfWidth );
      }
    }


    ATH_MSG_DEBUG( "REGTEST:" << *roi );

    if (!(roi->isFullscan())){
      if(doTiming()) m_timerRegSel->start();
      m_regionSelector->HashIDList( *roi, m_listOfPixIds);
      m_numPixIds = m_listOfPixIds.size();
      ATH_MSG_DEBUG( "REGTEST: Pixel : Roi contains " 
		     << m_numPixIds << " det. Elements" );
      
      if(doTiming()) m_timerRegSel->stop();
    }
    
    if (!m_skipBSDecoding){
      StatusCode scdec = StatusCode::SUCCESS;
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
      
      
      if (scdec.isSuccess()){
	//check for recoverable errors
	
	int n_err_total = 0;
	
	int bsErrors[IPixelByteStreamErrorsTool::AllPixErrTypes.size()];
	
	for (const auto idx : IPixelByteStreamErrorsTool::AllPixErrTypes ){
	  int n_errors = m_bsError->getNumberOfErrors(idx);
	  n_err_total += n_errors;
	  bsErrors[idx] = n_errors;
	}
	
	ATH_MSG_DEBUG( "decoding errors: "  << n_err_total );
	
	if (n_err_total){
	  for (const auto idx : IPixelByteStreamErrorsTool::AllPixErrTypes ){
	    if (bsErrors[idx])
	      m_PixBSErr.push_back(idx);
	    if(msgLvl(MSG::DEBUG))
	      msg(MSG::DEBUG) << " " << bsErrors[idx];
	  }
	}	     
	ATH_MSG_DEBUG( "" );
	
      } else {
	ATH_MSG_DEBUG( " m_rawDataProvider->decode failed" );
      }
    }
    
    if(doTiming()) m_timerSGate->resume();
    
    const PixelRDO_Container* p_pixelRDOContainer = nullptr;
    if (store()->retrieve(p_pixelRDOContainer,  m_pixelRDOContainerName).isFailure() ) {
      ATH_MSG_WARNING( "Could not find the PixelRDO_Container " 
		       << m_pixelRDOContainerName );
      
      // Activate the TriggerElement anyhow.
      // (FEX algorithms should not cut and it could be that in the SCT/TRT 
      // tracks can anyhow be reconstructed) 
      
      return HLT::OK;
    } 
    else{
      ATH_MSG_DEBUG( "REGTEST: Found the Pixel_RDO_Container " 
		     << m_pixelRDOContainerName );
    }
    if(doTiming()) m_timerSGate->pause();

    ATH_MSG_VERBOSE("Size of " << m_pixelRDOContainerName << ":" << p_pixelRDOContainer->size());

    if (!(roi->isFullscan())){

      if(doTiming()) m_timerCluster->resume();

      for (unsigned int i=0; i<m_listOfPixIds.size(); i++) {

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}
 
	const InDetRawDataCollection<PixelRDORawData>* RDO_Collection (p_pixelRDOContainer->indexFindPtr(m_listOfPixIds[i]));
      
	if (!RDO_Collection) continue;
      

	ATH_MSG_VERBOSE( "RDO collection size="
			 << RDO_Collection->size()
			 << ", ID=" << RDO_Collection->identify() );
	ATH_MSG_VERBOSE( "Ambiguities map: " 
			 << AmbiguitiesMap->size() << " elements" );
      
      
	if (RDO_Collection->size() != 0){
	
	  m_clusterCollection = m_clusteringTool->clusterize(*RDO_Collection,*m_idHelper);
	
	  if (m_clusterCollection && m_clusterCollection->size()!=0){ 
	  
	  
	    m_numPixClusters+= m_clusterCollection->size();
	  
	    /*if ( m_clusterContainer->indexFind(clusterCollection->identifyHash()) != m_clusterContainer->end() )
	      msg() << MSG::INFO << "FOUND Cluster collection :" << clusterCollection->identifyHash() << endmsg; */
	  
	    if (m_clusterContainer->addCollection( m_clusterCollection, 
						   m_clusterCollection->identifyHash() ).isFailure()) {
	      
	      ATH_MSG_VERBOSE( "Failed to add Cluster collection : "
			       << m_clusterCollection->identifyHash() );
	      delete m_clusterCollection;
	    } else {
	      m_ClusHashId.push_back(m_clusterCollection->identifyHash());
	      ATH_MSG_VERBOSE( "Filling ambiguities map" );
	      
	      const size_t ambiMapSize = AmbiguitiesMap->size();
	      m_gangedAmbiguitiesFinder->execute(m_clusterCollection,
						 *AmbiguitiesMap);
	      
	      if (ambiMapSize != AmbiguitiesMap->size())
		ATH_MSG_DEBUG( "After filling - Ambiguities map: " 
			       << AmbiguitiesMap->size() << " elements" );
	    }
	  } 
	  else {
	    ATH_MSG_DEBUG( "Clustering algorithm found no clusters" );
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
    
      if(doTiming()) m_timerCluster->resume();

      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections){
	const InDetRawDataCollection<PixelRDORawData>* RDO_Collection(*rdoCollections);
	if (!RDO_Collection) continue;

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}


	ATH_MSG_VERBOSE( "RDO collection size="
			 << RDO_Collection->size()
			 << ", ID=" << RDO_Collection->identify() );
	ATH_MSG_VERBOSE( "Ambiguities map: " 
			 << AmbiguitiesMap->size() << " elements" );


	if (RDO_Collection->size() != 0){
	  // Use one of the specific clustering AlgTools to make clusters
	  m_clusterCollection = m_clusteringTool->clusterize(*RDO_Collection,
							     *m_idHelper);
	  if (m_clusterCollection){
	    if (m_clusterCollection->size() != 0) {

	      ATH_MSG_VERBOSE("Going to add colHash " << m_clusterCollection->identifyHash() << " of size "
			      << m_clusterCollection->size() );
	    
	      m_numPixClusters+= m_clusterCollection->size();


	      if (m_clusterContainer->addCollection( m_clusterCollection, 
						     m_clusterCollection->identifyHash() ).isSuccess()){
		m_ClusHashId.push_back(m_clusterCollection->identifyHash());
		ATH_MSG_VERBOSE( "Filling ambiguities map" );  
		
		m_gangedAmbiguitiesFinder->execute(m_clusterCollection,
						   *AmbiguitiesMap);
		
		ATH_MSG_VERBOSE( "Ambiguities map: " 
				 << AmbiguitiesMap->size() << " elements" );
	      

		ATH_MSG_VERBOSE( "Clusters successfully added to Container !" );
	      }
	      else{

		ATH_MSG_VERBOSE( "Failed to add Clusters to container" );
		delete m_clusterCollection;
		//return StatusCode::FAILURE;
	      }
	    } 
	    else{
	      ATH_MSG_DEBUG( "Clustering algorithm found no clusters" );
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

    ATH_MSG_DEBUG( " REGTEST: Number of reconstructed clusters = " 
		   << m_numPixClusters );
  

    return HLT::OK;
  }
  //-----------------------------------
  //          Finalize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::hltFinalize() 
  //----------------------------------------------------------------------------
  {
  
    ATH_MSG_INFO( "Pixel_TrgClusterization::finalize()" );

    m_clusterContainer->cleanup();    
	
    //delete m_globalPosAlg;
    //delete m_clusterContainer;

    //igb -fix me- it crashes
    m_clusterContainer->release();

    return HLT::OK;
  }

  //-------------------------------------------------------------------------
  HLT::ErrorCode Pixel_TrgClusterization::prepareRobRequests(const HLT::TriggerElement* inputTE ) {

    ATH_MSG_DEBUG( "Running prepareRobRequests in Pixel_trgClusterization. " );
    
    // Calculate ROBs needed  - this code should be shared with hltExecute to avoid slightly different requests
    const TrigRoiDescriptor* roi = 0;

    if (getFeature(inputTE, roi) != HLT::OK || roi == 0) {
      ATH_MSG_WARNING( "REGTEST / Failed to find RoiDescriptor " );
      return HLT::NAV_ERROR;
    }
 

    ATH_MSG_DEBUG( "REGTEST prepareROBs / event"
		   << " RoI id " << roi->roiId()
		   << " located at   phi = " <<  roi->phi()
		   << ", eta = " << roi->eta() );
		
    std::vector<unsigned int> uIntListOfRobs;
    m_regionSelector->ROBIDList( *roi, uIntListOfRobs );

    ATH_MSG_DEBUG( "list of pre-registered ROB ID in PIX: " );
    for(uint i_lid(0); i_lid<uIntListOfRobs.size(); i_lid++)
      ATH_MSG_DEBUG( uIntListOfRobs.at(i_lid) );
    
    //m_robDataProvider->addROBData( uIntListOfRobs );
    config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
    uIntListOfRobs.clear();

    return HLT::OK;

  }


}
