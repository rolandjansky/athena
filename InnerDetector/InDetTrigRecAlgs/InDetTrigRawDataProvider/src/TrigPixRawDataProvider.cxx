/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigRawDataProvider/TrigPixRawDataProvider.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"

namespace InDet {


  


  // --------------------------------------------------------------------
  // Constructor

  TrigPixRawDataProvider::TrigPixRawDataProvider( const std::string& type,
						  const std::string& name,
						  const IInterface* parent) :
    AlgTool(type,name,parent),
    m_regionSelector  ("RegSelSvc", name), 
    m_robDataProvider ("ROBDataProviderSvc", name),
    m_rawDataTool     ("PixelRawDataProviderTool"),
    m_storeGate       ("StoreGateSvc",name),
    m_detStore        ("DetectorStore",name),
    m_IdMapping       ("PixelCablingSvc",name),
    m_container(0),
    m_log(0)
  {
    declareInterface<InDet::ITrigRawDataProviderTool>(this);
    declareProperty("RDOKey", m_RDO_Key = "PixelRDOs_EFID");
    m_log = new MsgStream(msgSvc(), name);

  }

  TrigPixRawDataProvider::~TrigPixRawDataProvider(){
    delete m_log;
  }


  // --------------------------------------------------------------------
  // Initialize

  StatusCode TrigPixRawDataProvider::initialize() {
    m_log->setLevel(msgSvc()->outputLevel(name()));
    *m_log << MSG::INFO << "TrigPixRawDataProvider::initialize" << endreq;
    if ( m_regionSelector.retrieve().isFailure() ) {                     
      *m_log << MSG::FATAL                                                      
            << m_regionSelector.propertyName()                        
            << " : Unable to retrieve RegionSelector tool "       
            << m_regionSelector.type() << endreq;                          
      return StatusCode::FAILURE;
    }                                                                                

    // Get ROBDataProviderSvc
    if (m_robDataProvider.retrieve().isFailure()) {
      *m_log << MSG::FATAL << "Failed to retrieve " << m_robDataProvider << endreq;
      return StatusCode::FAILURE;
    } else
      *m_log << MSG::INFO << "Retrieved service " << m_robDataProvider << endreq;
 
    // Get PixelRawDataProviderTool
    if (m_rawDataTool.retrieve().isFailure()) {
      *m_log << MSG::FATAL << "Failed to retrieve " << m_rawDataTool << endreq;
      return StatusCode::FAILURE;
    } else
      *m_log << MSG::INFO << "Retrieved service " << m_rawDataTool << endreq;
 
    // Get an detector store
    if (m_detStore.retrieve().isFailure()) {
      *m_log << MSG::FATAL << "Failed to retrieve " << m_detStore << endreq;
      return StatusCode::FAILURE;
    } else
      *m_log << MSG::INFO << "Retrieved service " << m_detStore << endreq;
 
    StatusCode sc = m_detStore->retrieve(m_id,"PixelID"); 
    if (sc.isFailure()) {
      *m_log << MSG::FATAL << "Cannot retrieve Pixel ID helper!"      
	    << endreq;
      return StatusCode::FAILURE;
    } 

    // Get StoreGateSvc 
    if (m_storeGate.retrieve().isFailure()) {
      *m_log << MSG::FATAL << "Failed to retrieve service " << m_storeGate << endreq;
      return StatusCode::FAILURE;
    } else
      *m_log << MSG::INFO << "Retrieved service " << m_storeGate << endreq;
  
    // Retrieve id mapping 
    if (m_IdMapping.retrieve().isFailure()) {
      *m_log << MSG::FATAL << "Failed to retrieve tool " << m_IdMapping << endreq;
      return StatusCode::FAILURE;
    } else 
      *m_log << MSG::INFO << "Retrieved tool " << m_IdMapping << endreq;

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
	*m_log << MSG::FATAL << "Unable to record Pixel RDO Container" << endreq;
	return StatusCode::FAILURE;
      } else {
	*m_log << MSG::DEBUG << "Pixel RDO Container " << m_RDO_Key
	       << " recorded into SG" << endreq;
      }

    } else {
      if (!m_storeGate->retrieve(m_container,m_RDO_Key)){
	*m_log << MSG::FATAL << "Unable to retrieve existing Pixel RDO Container "
	       << m_RDO_Key << endreq;
	return StatusCode::FAILURE;
      } else {
	*m_log << MSG::DEBUG << "Retrieved existing Pixel RDO Container"
	       << m_RDO_Key << endreq;
      }
    }
    return sc;
  }


  // --------------------------------------------------------------------
  // Execute
  StatusCode TrigPixRawDataProvider::decode(const IRoiDescriptor *roi){

    std::vector<uint32_t> robIDlist;

    const int msgLvl = m_log->level();

    if (roi) {
      // Get RoiDescriptor                             
      if (msgLvl <= MSG::DEBUG) {                        
	*m_log << MSG::DEBUG << "REGTEST:" << *roi << endreq; 
      }  
      
      //double zmax = 168; 
      m_regionSelector->DetROBIDListUint( PIXEL, 
					  *roi, 
					  robIDlist); 
    } else {
      *m_log << MSG::ERROR << name() << " invoked without an RoI data " << endreq;
      return StatusCode::FAILURE;
      //robIDlist = m_IdMapping->getAllRods();
    }

    StatusCode sg = initContainer();
    if (sg.isFailure()){
      *m_log << MSG::ERROR << "cannot get RDO container" << endreq;
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
	*m_log << MSG::ERROR << "BS conversion into RDOs failed" << endreq;
    }
    return scon;
  }


  StatusCode TrigPixRawDataProvider::finalize() {
    m_container->cleanup();
    m_container->release();
    return StatusCode::SUCCESS;
  }

} //namespace


