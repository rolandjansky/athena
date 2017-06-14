/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigRawDataProvider/TrigPixRawDataProvider.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


namespace InDet {

  


  // --------------------------------------------------------------------
  // Constructor

  TrigPixRawDataProvider::TrigPixRawDataProvider( const std::string& type,
						  const std::string& name,
						  const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_regionSelector  ("RegSelSvc", name), 
    m_robDataProvider ("ROBDataProviderSvc", name),
    m_rawDataTool     ("PixelRawDataProviderTool"),
    m_storeGate       ("StoreGateSvc",name),
    m_detStore        ("DetectorStore",name),
    m_IdMapping       ("PixelCablingSvc",name),
    m_id(0),
    m_container(0)
  {
    declareInterface<InDet::ITrigRawDataProviderTool>(this);
    declareProperty("RDOKey", m_RDO_Key = "PixelRDOs_EFID");
    declareProperty("RawDataProviderTool", m_rawDataTool);
  }

  TrigPixRawDataProvider::~TrigPixRawDataProvider(){
  }


  // --------------------------------------------------------------------
  // Initialize

  StatusCode TrigPixRawDataProvider::initialize() {
    msg(MSG::INFO) << "TrigPixRawDataProvider::initialize" << endmsg;
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
 
    // Get PixelRawDataProviderTool
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
 
    StatusCode sc = m_detStore->retrieve(m_id,"PixelID"); 
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve Pixel ID helper!"      
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
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_IdMapping << endmsg;
      return StatusCode::FAILURE;
    } else 
      msg(MSG::INFO) << "Retrieved tool " << m_IdMapping << endmsg;

    //RDO container
    m_container = new PixelRDO_Container(m_id->wafer_hash_max()); 
    m_container ->addRef();       // make sure it is never deleted


    return StatusCode::SUCCESS;
  }


  StatusCode TrigPixRawDataProvider::initContainer(){

    StatusCode sc = StatusCode::SUCCESS;
    
    if(!m_storeGate->transientContains<PixelRDO_Container>(m_RDO_Key)){
      // record into StoreGate
      m_container->cleanup();
      if (m_storeGate->record(m_container, m_RDO_Key).isFailure()) {
	msg(MSG::FATAL) << "Unable to record Pixel RDO Container" << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::DEBUG) << "Pixel RDO Container " << m_RDO_Key
	       << " recorded into SG" << endmsg;
      }

    } else {
      if (!m_storeGate->retrieve(m_container,m_RDO_Key)){
	msg(MSG::FATAL) << "Unable to retrieve existing Pixel RDO Container "
	       << m_RDO_Key << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::DEBUG) << "Retrieved existing Pixel RDO Container"
	       << m_RDO_Key << endmsg;
      }
    }
    return sc;
  }


  // --------------------------------------------------------------------
  // Execute
  StatusCode TrigPixRawDataProvider::decode(const IRoiDescriptor *roi){

    std::vector<uint32_t> robIDlist;

    if (roi) {
      // Get RoiDescriptor  
      ATH_MSG_DEBUG ( "REGTEST:" << *roi );
    
      //double zmax = 168; 
      m_regionSelector->DetROBIDListUint( PIXEL, 
					  *roi, 
					  robIDlist); 
    } else {
      msg(MSG::ERROR) << name() << " invoked without an RoI data " << endmsg;
      return StatusCode::FAILURE;
      //robIDlist = m_IdMapping->getAllRods();
    }

    StatusCode sg = initContainer();
    if (sg.isFailure()){
      msg(MSG::ERROR) << "cannot get RDO container" << endmsg;
      return StatusCode::FAILURE;
    }

    // ask ROBDataProviderSvc for the vector of ROBFragment for all Pixel ROBIDs
    std::vector<const ROBFragment*> listOfRobf;
    m_robDataProvider->getROBData(robIDlist , listOfRobf);
    // ask PixelRawDataProviderTool to decode it and to fill the IDC

    StatusCode scon = StatusCode::FAILURE;
    if (m_container){
      scon = m_rawDataTool->convert(listOfRobf,m_container);
      if (scon==StatusCode::FAILURE)
	msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;
    }
    return scon;
  }


  StatusCode TrigPixRawDataProvider::finalize() {
    m_container->cleanup();
    m_container->release();
    return StatusCode::SUCCESS;
  }

} //namespace


