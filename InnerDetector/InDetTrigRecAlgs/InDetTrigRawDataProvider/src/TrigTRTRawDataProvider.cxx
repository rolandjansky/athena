/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigRawDataProvider/TrigTRTRawDataProvider.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "TRT_RawDataByteStreamCnv/ITRTRawDataProviderTool.h"


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


namespace InDet {

  // --------------------------------------------------------------------
  // Constructor

  TrigTRTRawDataProvider::TrigTRTRawDataProvider( const std::string& type,
						  const std::string& name,
						  const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_regionSelector  ("RegSelSvc", name), 
    m_robDataProvider ("ROBDataProviderSvc", name),
    m_rawDataTool     ("TRTRawDataProviderTool/InDetTrigTRTRawDataProviderTool"),
    m_storeGate       ("StoreGateSvc",name),
    m_detStore        ("DetectorStore",name),
    m_IdMapping       ("TRT_CablingSvc",name),
    m_id(0),
    m_container(0)
  {
    declareInterface<InDet::ITrigRawDataProviderTool>(this);
    declareProperty("RDOKey", m_RDO_Key = "TRT_RDOs_EFID");
    declareProperty("RawDataTool", m_rawDataTool);
  }

  TrigTRTRawDataProvider::~TrigTRTRawDataProvider(){
  }

  // --------------------------------------------------------------------
  // Initialize

  StatusCode TrigTRTRawDataProvider::initialize() {
    msg(MSG::INFO) << "TrigTRTRawDataProvider::initialize" << endmsg;

    if ( m_regionSelector.retrieve().isFailure() ) {        
      msg(MSG::FATAL) << m_regionSelector.propertyName()
		      << " : Unable to retrieve RegionSelector tool "
		      << m_regionSelector.type() << endmsg;           
      return StatusCode::FAILURE;                          
    }                                                       

    // Get ROBDataProviderSvc
    if (m_robDataProvider.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_robDataProvider << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved service " << m_robDataProvider << endmsg;
 
    // Get TRTRawDataProviderTool
    if (m_rawDataTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_rawDataTool << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved service " << m_rawDataTool << endmsg;
 
    // Get an detector store
    if (m_detStore.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_detStore << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved service " << m_detStore << endmsg;
 
    StatusCode sc = m_detStore->retrieve(m_id,"TRT_ID"); 
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve TRT ID helper!"      
	    << endmsg;
      return StatusCode::FAILURE;
    } 

    // Get StoreGateSvc 
    if (m_storeGate.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_storeGate << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved service " << m_storeGate << endmsg;
  
    // Retrieve id mapping 
    if (m_IdMapping.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_IdMapping << endmsg;
      return StatusCode::FAILURE;
    } else 
      msg(MSG::INFO) << "Retrieved service " << m_IdMapping << endmsg;

    m_container = new TRT_RDO_Container(m_id->straw_layer_hash_max()); 
    m_container ->addRef();     // make sure it is never deleted

    return StatusCode::SUCCESS;
  }

  StatusCode TrigTRTRawDataProvider::initContainer(){

    StatusCode sc = StatusCode::SUCCESS;

    if(!m_storeGate->transientContains<TRT_RDO_Container>(m_RDO_Key)){

      msg(MSG::DEBUG) << "Create TRT RDO Container on first event" << endmsg;
      // now create the container and register the collections
      // write into StoreGate
      if (m_storeGate->record(m_container, m_RDO_Key).isFailure()) {
	msg(MSG::FATAL) << "Unable to record TRT RDO Container" << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::DEBUG) << "TRT RDO Container recorded into SG" << endmsg;
      }
    } else {
      if (!m_storeGate->retrieve(m_container,m_RDO_Key)){
	msg(MSG::FATAL) << "Unable to retrieve existing TRT RDO Container" << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::DEBUG) << "Retrieved existing TRT RDO Container" << endmsg;
      }
    }
    
    return sc;
    
  }


  // --------------------------------------------------------------------
  // Execute

  StatusCode TrigTRTRawDataProvider::decode(const IRoiDescriptor *roi){
    std::vector<uint32_t> robIDlist;
    
    if (roi) {
      // Get RoiDescriptor
      ATH_MSG_DEBUG( "REGTEST:" << *roi );
            
      //double zmax = 168; 
      m_regionSelector->DetROBIDListUint( TRT, 
					  *roi, 
					  robIDlist);
    } else {
      robIDlist = m_IdMapping->getAllRods();
    }

    StatusCode sg = initContainer();
    if (sg.isFailure()){
      msg(MSG::ERROR) << "cannot get RDO container" << endmsg;
      return StatusCode::FAILURE;
    }

    // ask ROBDataProviderSvc for the vector of ROBFragment for all TRT ROBIDs
    std::vector<const ROBFragment*> listOfRobf;
    m_robDataProvider->getROBData(robIDlist , listOfRobf);
    // ask TRTRawDataProviderTool to decode it and to fill the IDC
    StatusCode scon = StatusCode::FAILURE;
    if (m_container){
      scon = m_rawDataTool->convert(listOfRobf,m_container);
      if (scon==StatusCode::FAILURE)
	msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;
    }
    return scon;

  }

  StatusCode TrigTRTRawDataProvider::finalize() {
    m_container->cleanup();
    m_container->release();
    return StatusCode::SUCCESS;
  }

} //namespace
