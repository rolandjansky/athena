/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for TRT_TrgRIO_Maker.
//  (see header file)
//****************************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "AthenaKernel/Timeout.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetTrigPrepRawDataFormat/TRT_TrgRIO_Maker.h"
#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"


//Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

namespace InDet{

  // /////////////////////////////////////////////////////////////////
  // Constructor
  // /////////////////////////////////////////////////////////////////
  TRT_TrgRIO_Maker::TRT_TrgRIO_Maker 
  (const std::string& name,ISvcLocator* pSvcLocator) : 
    HLT::FexAlgo(name,pSvcLocator),
    m_trt_rdo_location("TRT_RDOs"),
    m_trt_rio_location("TRT_TrigDriftCircles"),
    m_rawDataProvider("InDet::TrigTRTRawDataProvider"),
    m_driftcircle_tool("InDet::TRT_DriftCircleTool"), 
    m_riocontainer(0),
    m_mode_rio_production(0),
    m_regionSelector("RegSelSvc", name),
    m_bsErrorSvc("TRT_ByteStream_ConditionsSvc",name),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_doFullScan(false),
    m_etaHalfWidth (0.1),
    m_phiHalfWidth (0.1),
    m_doTimeOutChecks(true)
  {
    
    // Read TRT_TrgRIO_Maker steering parameters
    declareProperty("TRT_RIOContainerName", m_trt_rio_location);
    declareProperty("TRT_RDOContainerName", m_trt_rdo_location);
    declareProperty("ModeRIOProduction",    m_mode_rio_production);
    declareProperty("doFullScan",           m_doFullScan);
    declareProperty("EtaHalfWidth",         m_etaHalfWidth);
    declareProperty("PhiHalfWidth",         m_phiHalfWidth);
    declareProperty("RawDataProvider",      m_rawDataProvider);
    declareProperty("doTimeOutChecks",      m_doTimeOutChecks);

    //selection of the TRT bad channels(true/false)
    declareProperty("TRT_DriftCircleTool",    m_driftcircle_tool);

    declareMonitoredVariable("numTrtDriftCircles", m_numTrtDriftCircles   );
    declareMonitoredVariable("numTrtIds", m_numTrtIds    );
    declareMonitoredStdContainer("TrtHashId", m_ClusHashId);
    declareMonitoredStdContainer("TrtBsErr", m_TrtBSErr);
  
    m_timerRegSel=m_timerSeed=m_timerCluster=0;
    m_timerSGate=0;  

  }
  
  //----------------------------------  
  //          Initialize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltInitialize() {
    //----------------------------------------------------------------------------

      
    // Get TRT_DriftCircle tool
    if ( m_driftcircle_tool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_driftcircle_tool.propertyName() << ": Failed to retrieve tool " << m_driftcircle_tool );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else
      ATH_MSG_INFO( m_driftcircle_tool.propertyName() << ": Retrieved tool " << m_driftcircle_tool );
    
  
    // Get DetectorStore service
    //
    /*StoreGateSvc* detStore;
    StatusCode s=service("DetectorStore",detStore);
    if (s.isFailure())  {
      ATH_MSG_FATAL( "DetectorStore service not found" ); 
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }*/ 


    const TRT_ID * IdHelper(0);
    if (detStore()->retrieve(IdHelper, "TRT_ID").isFailure()) {
      ATH_MSG_FATAL ("Could not get TRT ID helper" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }


    if(!store()->transientContains<InDet::TRT_DriftCircleContainer>(m_trt_rio_location)) {

      // Create TRT RIO container
      m_riocontainer = new InDet::TRT_DriftCircleContainer(IdHelper->straw_layer_hash_max());
      if(!m_riocontainer) {
	ATH_MSG_FATAL( "Could not creat TRT_DriftCircleContainer " << m_trt_rio_location ); 
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }

      if (store()->record(m_riocontainer, m_trt_rio_location).isFailure()) {
	ATH_MSG_WARNING( " Container " << m_trt_rio_location
			 << " could not be recorded in StoreGate !" ); 
      } 
      else{ 
	ATH_MSG_INFO( "Container " << m_trt_rio_location 
		      << " successfully registered  in StoreGate" );  
      }
    }
    else {    
      StatusCode s = store()->retrieve(m_riocontainer, m_trt_rio_location);
    
      if (s.isFailure()) {
	ATH_MSG_ERROR( "Failed to get Cluster Container" );
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else{  
	ATH_MSG_INFO( "Got Cluster Container from TDS: " << m_trt_rio_location );
      }
    }
    m_riocontainer->addRef();

    if(!m_doFullScan){
      // Retrieving Region Selector Tool:
      if ( m_regionSelector.retrieve().isFailure() ) {
	ATH_MSG_FATAL( m_regionSelector.propertyName()
		       << " : Unable to retrieve RegionSelector tool "
		       << m_regionSelector.type() );
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
    }
    else{
      ATH_MSG_INFO( "RegionSelector tool not needed for FullScan" );
    }

    if (m_bsErrorSvc.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_bsErrorSvc );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    
    //retrieve rob data provider service
    if (m_robDataProvider.retrieve().isFailure()){
      ATH_MSG_FATAL( "Failed to retrieve " << m_robDataProvider );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else
      ATH_MSG_INFO( "Retrieved service " << m_robDataProvider << " in TRT_TrgRIO_Maker." );

    //decoding tool
    if (m_rawDataProvider.retrieve().isFailure()){
      ATH_MSG_ERROR( "Raw data provider not available" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    // Get the messaging service, print where you are
    if (!m_doFullScan)
      ATH_MSG_INFO( "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth );
    else
      ATH_MSG_INFO( "FullScan mode" );
    
    // Timers:
    m_timerRegSel  = addTimer("InDetEFTClRegSel"); // 1
    m_timerSeed    = addTimer("InDetEFTClSeed");   // 2
    m_timerCluster = addTimer("InDetEFTCluster");  // 3
    m_timerSGate   = addTimer("InDetEFTClSGate");  // 4

    return HLT::OK;
  }
  //----------------------------------------------------------------------------
  TRT_TrgRIO_Maker::~TRT_TrgRIO_Maker() {}  
  //----------------------------------------------------------------------------
  //         Execute method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltExecute(const HLT::TriggerElement*,
					      HLT::TriggerElement* outputTE){
    //----------------------------------------------------------------------------
    
    //initialisation of monitored quantities
    m_numTrtIds = 0;
    m_numTrtDriftCircles = 0;
    m_listOfTrtIds.clear();
    m_ClusHashId.clear();
    m_TrtBSErr.clear();

    // For the moment, the output will be stored in StoreGate:
    // ------------------------------------------------------
    // TRT_DriftCircle container registration
  
    if(doTiming()) m_timerSGate->resume();

    if(!store()->transientContains<InDet::TRT_DriftCircleContainer>(m_trt_rio_location)) {
      m_riocontainer->cleanup();
      
      if (store()->record(m_riocontainer,
			  m_trt_rio_location,false).isFailure()) {
	ATH_MSG_WARNING( "Container " << m_trt_rio_location
			 << " could not be recorded in StoreGate !" );
      } 
      else{
	ATH_MSG_DEBUG( "REGTEST: Container '" << m_trt_rio_location
		       << " recorded in StoreGate " );
      }
    }
    else {
      ATH_MSG_DEBUG( "Container '" << m_trt_rio_location
		    << "' recorded in StoreGate" );
    }
  
    if(doTiming()) m_timerSGate->pause();

    //---------------------------
    // Trigger specific part:
    //---------------------------
    //   Get from the Trigger Element the RoI
    //   Ask the Region Selector for the list of hash indexes for this ROI.
    //   Transform those indexes into a RDO collection.
    //-------------------------------------------------------------------------

    //reset BS error service
    m_bsErrorSvc->resetCounts();
    StatusCode scdec = StatusCode::SUCCESS;

    if(!m_doFullScan){ //this is not needed for FullScan
  
      if(doTiming()) m_timerSeed->start();
    
      // Get RoiDescriptor
      const TrigRoiDescriptor* roi;
      if ( HLT::OK !=  getFeature(outputTE, roi) ) {
	ATH_MSG_WARNING( "Can't get RoI" );
	return HLT::NAV_ERROR;
      }
    
      if (!roi){
	ATH_MSG_WARNING ( "Received NULL RoI" );
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


      //NaN. this is for the check on the case in which the call of phi() and eta() do not return a well-defined numerical value
      if (roi->phi() != roi->phi() || roi->eta() !=roi->eta()){
	ATH_MSG_WARNING( "Received bad RoI " << *roi ); 
	return HLT::NAV_ERROR;
      }

      scdec = m_rawDataProvider->decode(roi);

  
      if(doTiming()) m_timerSeed->stop();

      //   Get the TRT RDO's:
      //     - First get the TRT ID's using the RegionSelector
      //     - Retrieve from SG the RDOContainer: 
      //       Identifiable Container that contains pointers to all the RDO 
      //        collections (one collection per detector)
      //     - Retrieve from StoreGate the RDO collections.
      //       (the ByteStreamConvertors are called here).
      //std::vector<IdentifierHash> listOfTrtIds; 
    
      if(doTiming()) m_timerRegSel->start();
      
      m_regionSelector->DetHashIDList( TRT, *roi, m_listOfTrtIds);
      m_numTrtIds = m_listOfTrtIds.size();

      if(doTiming()) m_timerRegSel->stop();

    
      ATH_MSG_DEBUG( "REGTEST: TRT : Roi contains " 
		     << m_numTrtIds << " det. Elements" );
    }
    else {   //fullscan
      scdec = m_rawDataProvider->decode(0);    //NULL RoI means decode everything
    }
    
    if (scdec.isSuccess()){
      //check for recoverable errors
      int n_err_total = 0;
       
      int bsErrors[EFID_MAXNUM_TRT_BS_ERRORS];
      
      for (size_t idx = 0; idx<size_t(EFID_MAXNUM_TRT_BS_ERRORS); idx++){
	int n_errors = m_bsErrorSvc->getNumberOfErrors(TRTByteStreamErrors::errorTypes(idx));
	n_err_total += n_errors;
	bsErrors[idx] = n_errors;
      }

      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "decoding errors: " << n_err_total;

      if (n_err_total){
	for (size_t idx = 0; idx<size_t(EFID_MAXNUM_TRT_BS_ERRORS); idx++){
	  //	  m_TrtBSErr.push_back(bsErrors[idx]);
	  if (bsErrors[idx])
	    m_TrtBSErr.push_back(idx);
	  if(msgLvl(MSG::DEBUG))
	    msg(MSG::DEBUG) << " " << bsErrors[idx];
	}
      }
      ATH_MSG_DEBUG( "" );
    }
    else {
      ATH_MSG_DEBUG( " m_rawDataProvider->decode failed" );
    }

  

    if(doTiming()) m_timerSGate->start();

    const TRT_RDO_Container* p_trtRDOContainer;
    StatusCode sc = store()->retrieve(p_trtRDOContainer, m_trt_rdo_location);

    if (sc.isFailure() ) {
      ATH_MSG_FATAL( "Could not find the TRT_RDO_Container " 
		     << m_trt_rdo_location );

      // Activate the TriggerElement anyhow.
      // (FEX algorithms should not cut and it could be that in the Pixel/SCT 
      // tracks can anyhow be reconstructed) 
      return HLT::ERROR;
    } 
    else{

      ATH_MSG_DEBUG( "REGTEST: Found the TRT_RDO_Container "
		     << m_trt_rdo_location );
    }
    if(doTiming()) m_timerSGate->pause();

    if(!m_doFullScan){
    
      if(doTiming()) m_timerCluster->resume();

      for (unsigned int i=0; i<m_listOfTrtIds.size(); i++) {

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	TRT_RDO_Container::const_iterator 
	  RDO_collection_iter = p_trtRDOContainer->indexFind(m_listOfTrtIds[i]); 

	if (RDO_collection_iter == p_trtRDOContainer->end()) continue;
    
	const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(*RDO_collection_iter);
      
	if (!RDO_Collection) continue;
      

	ATH_MSG_VERBOSE( "RDO collection size="
			 << RDO_Collection->size()
			 << ", ID=" << RDO_Collection->identify() );
      
	if (RDO_Collection->size() != 0){

	  const InDet::TRT_DriftCircleCollection* p_rio = 
	    m_driftcircle_tool->convert (m_mode_rio_production, RDO_Collection, true);
      
	  m_numTrtDriftCircles += p_rio->size();
	  // -me- fix test
	  if(p_rio->size() != 0) {
	  
	    // Record in StoreGate the drift circle container:
	    sc = m_riocontainer->addCollection(p_rio,p_rio->identifyHash());

	    m_ClusHashId.push_back(p_rio->identifyHash());

	    if(sc.isFailure()) {
	      ATH_MSG_VERBOSE( "Failed registering TRT_DriftCircle collection : "
			       <<  p_rio->identify() );
	      delete p_rio;
	    }
	  } else {
	    // -me- cleanup memory
	    delete p_rio;
	  }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }
    else{   //fullscan
    
      //      p_trtRDOContainer->clID();
      TRT_RDO_Container::const_iterator rdoCollections    = p_trtRDOContainer->begin();
      TRT_RDO_Container::const_iterator rdoCollectionsEnd = p_trtRDOContainer->end();
    
      if(doTiming()) m_timerCluster->resume();
    
      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections) {
      
	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	const InDetRawDataCollection<TRT_RDORawData>* currentCollection(*rdoCollections);
	
        const InDet::TRT_DriftCircleCollection* p_rio=
          m_driftcircle_tool->convert(m_mode_rio_production, currentCollection , true);
        
	if(p_rio) {
	  if (p_rio->size() != 0) {
	    m_numTrtDriftCircles += p_rio->size();
	    // -me- new IDC does no longer register in Storegate if hash is used !
	    //s = p_riocontainer->addCollection(p_rio, p_rio->identify()); 

	    sc = m_riocontainer->addCollection(p_rio, p_rio->identifyHash()); 
	    //s = m_store_gate->record(p_rio,p_riocontainer->key(p_rio->identify()));
	    if(sc.isFailure()) {
	      ATH_MSG_VERBOSE( "Error while registering TRT_DriftCircle collection" );
	      //return HLT::ERROR;
	      delete p_rio;
	    }
	  }
	  else {
	    ATH_MSG_DEBUG( "Don't write empty collections" );
	    // -me- cleanup memory
	    delete p_rio;
          }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }
    
    if(doTiming()){
      m_timerCluster->stop();
      m_timerSGate->stop();
    }

    ATH_MSG_DEBUG( "REGTEST: Reconstructed " << m_numTrtDriftCircles 
		   << " drift circles in the RoI " );
  
    //igb fixme
    //sc = store()->setConst(m_riocontainer);
    //if(sc.isFailure()) {
    //msg() << MSG::ERROR
    //<< " ??? " <<endmsg;
    //return sc;
    //}

    return HLT::OK;
  }
  //-----------------------------------
  //          Finalize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltFinalize() {
    //----------------------------------------------------------------------------
    m_riocontainer->cleanup();

    //delete m_riocontainer;
    //igb -fix me- it crashes
    m_riocontainer->release();

    return HLT::OK;
  }

  //---------------------------------
  //        prepareRobRequests method:
  //--------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::prepareRobRequests(const HLT::TriggerElement* inputTE){

    ATH_MSG_DEBUG( "TRT_TrgRIO_Maker::prepareRobRequests()" );

    const TrigRoiDescriptor* roi = 0;

    if (getFeature(inputTE, roi) != HLT::OK || roi == 0){
      ATH_MSG_WARNING( "REGTEST / Failed to find RoiDescriptor" );
      return HLT::NAV_ERROR;
    }

    ATH_MSG_DEBUG( "REGTEST prepareROBs / event RoI " << *roi);

    std::vector<unsigned int> uIntListOfRobs;
    m_regionSelector->DetROBIDListUint( TRT, *roi, uIntListOfRobs );

    ATH_MSG_DEBUG( "list of pre-registered ROB ID in TRT: ");
    for(auto i : uIntListOfRobs)
      ATH_MSG_DEBUG( i );

    //m_robDataProvider->addROBData( uIntListOfRobs );
    config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
    uIntListOfRobs.clear();

    return HLT::OK;
    
  }

  //---------------------------------------------------------------------------
}
