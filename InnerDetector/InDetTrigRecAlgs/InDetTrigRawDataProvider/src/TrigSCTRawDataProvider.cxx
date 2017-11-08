/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigRawDataProvider/TrigSCTRawDataProvider.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

//#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRegSelSvc.h" 


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


namespace InDet {


  // --------------------------------------------------------------------
  // Constructor

  TrigSCTRawDataProvider::TrigSCTRawDataProvider( const std::string& type,
						  const std::string& name,
						  const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_regionSelector  ("RegSelSvc", name), 
    m_robDataProvider ("ROBDataProviderSvc", name),
    m_rawDataTool     ("SCTRawDataProviderTool"),
    m_storeGate       ("StoreGateSvc",name),
    m_detStore        ("DetectorStore",name),
    m_cablingSvc      ("SCT_CablingSvc",name),
    m_id(0),
    m_container(0)
  {
    declareInterface<InDet::ITrigRawDataProviderTool>(this);
    declareProperty("RDOKey", m_RDO_Key = "SCT_RDOs_EFID");
    declareProperty("RawDataTool", m_rawDataTool);
  }

  TrigSCTRawDataProvider::~TrigSCTRawDataProvider(){
  }

  // --------------------------------------------------------------------
  // Initialize
  StatusCode TrigSCTRawDataProvider::initialize() {

    msg(MSG::INFO) << "TrigSCTRawDataProvider::initialize" << endmsg;

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
 
    // Get SCTRawDataProviderTool
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
 
    StatusCode sc = m_detStore->retrieve(m_id,"SCT_ID"); 
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve SCT_ID helper!"      
	    << endmsg;
      return StatusCode::FAILURE;
    } 

    // Get StoreGateSvc 
    if (m_storeGate.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve servive " << m_storeGate << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Retrieved service " << m_storeGate << endmsg;
  
    // Retrieve id mapping 
    if (m_cablingSvc.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_cablingSvc << endmsg;
      return StatusCode::FAILURE;
    } else 
      msg(MSG::INFO) << "Retrieved service " << m_cablingSvc << endmsg;

    //RDO Container
    m_container = new SCT_RDO_Container(m_id->wafer_hash_max()); 
    m_container ->addRef();   // make sure it is never deleted
    
    IIncidentSvc* pIncsvc;
    if ( serviceLocator()->service("IncidentSvc", pIncsvc).isSuccess() ) {
      int priority = 100;
      pIncsvc->addListener( this, "BeginEvent", priority);
    } else {
      msg(MSG::ERROR) << "No connection to incidentSvc used for cleanup" << endmsg;
      return StatusCode::FAILURE;
    }


    return StatusCode::SUCCESS;
  }
  
  void TrigSCTRawDataProvider::handle(const Incident& inc) {
  if (inc.type() == "BeginEvent") {
    m_container->cleanup();
    m_rawDataTool->BeginNewEvent();
  }  
}

  
  StatusCode TrigSCTRawDataProvider::initContainer(){

    StatusCode sc = StatusCode::SUCCESS;

    if(!m_storeGate->transientContains<SCT_RDO_Container>(m_RDO_Key)){

      // now create the container and register the collections
      // write into StoreGate
      if (m_storeGate->record(m_container, m_RDO_Key).isFailure()) {
	msg(MSG::FATAL) << "Unable to record SCT RDO Container" << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::DEBUG) << "SCT RDO Container recorded into SG" << endmsg;
      }
    } else {
      if (!m_storeGate->retrieve(m_container,m_RDO_Key)){
	msg(MSG::FATAL) << "Unable to retrieve existing SCT RDO Container" << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::DEBUG) << "Retrieved existing SCT RDO Container" << endmsg;
      }
    }

    return sc;
  }


  // --------------------------------------------------------------------
  // Execute

  StatusCode TrigSCTRawDataProvider::decode(const IRoiDescriptor *roi){

    std::vector<uint32_t> robIDlist;

    if (roi) {
      // Get RoiDescriptor                                    
      ATH_MSG_DEBUG( "REGTEST:" << *roi);

      //      double zmax = 168; 
      m_regionSelector->DetROBIDListUint( SCT, 
					  *roi, 
					  robIDlist);                                       


    } else {
      m_cablingSvc->getAllRods(robIDlist);
    }

    StatusCode sg = initContainer();
    if (sg.isFailure()){
      msg(MSG::WARNING) << "RDO container cannot be registered" << endmsg;
      return StatusCode::FAILURE;
    }

    // ask ROBDataProviderSvc for the vector of ROBFragment for all SCT ROBIDs
    std::vector<const ROBFragment*> listOfRobf;
    m_robDataProvider->getROBData(robIDlist , listOfRobf);

    // ask SCTRawDataProviderTool to decode it and to fill the IDC
    StatusCode scon = StatusCode::FAILURE;
    if (m_container){
      scon =  m_rawDataTool->convert(listOfRobf,*m_container,nullptr);
      if (scon==StatusCode::FAILURE)
	msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;
    }

    return scon;
  }


  StatusCode TrigSCTRawDataProvider::finalize() {
    m_container->cleanup();
    m_container->release();
    return StatusCode::SUCCESS;
  }

} //namespace
