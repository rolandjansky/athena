/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
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
  m_trt_id          ( nullptr ),
  m_rdoContainerKey("")
{
  declareProperty("RDOKey", m_rdoContainerKey = std::string("TRT_RDOs"));
  declareProperty ( "ProviderTool", m_rawDataTool );
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

  ATH_CHECK( m_rdoContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------
// Execute

StatusCode TRTRawDataProvider::execute() 
{
  SG::WriteHandle<TRT_RDO_Container> rdoContainer(m_rdoContainerKey);
  rdoContainer = std::make_unique<TRT_RDO_Container>(m_trt_id->straw_hash_max()); 
  ATH_CHECK(rdoContainer.isValid());

  
  // ask ROBDataProviderSvc for the vector of ROBFragment for all TRT ROBIDs
  std::vector<const ROBFragment*> listOfRobf;
  m_robDataProvider->getROBData( m_CablingSvc->getAllRods(), listOfRobf);

  ATH_MSG_DEBUG( "Number of ROB fragments " << listOfRobf.size() );

  // ask TRTRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert(listOfRobf,&(*rdoContainer)).isFailure())
    ATH_MSG_WARNING( "BS conversion into RDOs failed" );

  ATH_MSG_DEBUG( "Number of Collections in IDC " << rdoContainer->numberOfCollections() );

  return StatusCode::SUCCESS;
}

