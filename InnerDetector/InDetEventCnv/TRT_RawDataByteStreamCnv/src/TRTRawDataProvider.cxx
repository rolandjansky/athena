/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTRawDataProvider.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"  

// --------------------------------------------------------------------
// Constructor

TRTRawDataProvider::TRTRawDataProvider(const std::string& name,
				       ISvcLocator* pSvcLocator) :
  AthAlgorithm      ( name, pSvcLocator ),
  m_robDataProvider ( "ROBDataProviderSvc", name ),
  m_rawDataTool     ( "TRTRawDataProviderTool",this ),
  m_CablingSvc      ( "TRT_CablingSvc", name ),
  m_trt_id          ( nullptr )
{
  declareProperty ( "RDOKey"      , m_RDO_Key = "TRT_RDOs" );
  declareProperty ( "ProviderTool", m_rawDataTool );
  m_first_event = true;
}

// --------------------------------------------------------------------
// Initialize

StatusCode TRTRawDataProvider::initialize() {

  ATH_MSG_INFO( "TRTRawDataProvider::initialize" );

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) 
  {
    ATH_MSG_FATAL( "Failed to retrieve serive " << m_robDataProvider );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO( "Retrieved service " << m_robDataProvider );
 

  /*
   * Get TRTRawDataProviderTool
   */
  if ( m_rawDataTool.retrieve().isFailure() )
  {
    ATH_MSG_FATAL( "Failed to retrieve serive " << m_rawDataTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO( "Retrieved service " << m_rawDataTool );


  // Get TRT Detector Manager 
  const InDetDD::TRT_DetectorManager* indet_mgr;
  StatusCode sc = detStore()->retrieve(indet_mgr,"TRT"); 
  if (sc.isFailure()) 
  {
    ATH_MSG_FATAL( "Cannot retrieve TRT_DetectorManager!" );
    return StatusCode::FAILURE;
  } 

  // Get the TRT Helper
  if (detStore()->retrieve(m_trt_id, "TRT_ID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }



  
  // Retrieve id mapping 
  if (m_CablingSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_CablingSvc );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO( "Retrieved service " << m_CablingSvc );

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------
// Execute

StatusCode TRTRawDataProvider::execute() 
{
  TRT_RDO_Container *container = new TRT_RDO_Container(m_trt_id->straw_layer_hash_max()); 

  if (evtStore()->record(container, m_RDO_Key).isFailure()) 
  {
    ATH_MSG_FATAL( "Unable to record TRT RDO Container." );
    return StatusCode::FAILURE;
  }
  
  // ask ROBDataProviderSvc for the vector of ROBFragment for all TRT ROBIDs
  std::vector<const ROBFragment*> listOfRobf;
  m_robDataProvider->getROBData( m_CablingSvc->getAllRods(), listOfRobf);

  ATH_MSG_DEBUG( "Number of ROB fragments " << listOfRobf.size() );

  // ask TRTRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert(listOfRobf,container).isFailure())
    ATH_MSG_WARNING( "BS conversion into RDOs failed" );

  ATH_MSG_DEBUG( "Number of Collections in IDC " << container->numberOfCollections() );

  return StatusCode::SUCCESS;
}

