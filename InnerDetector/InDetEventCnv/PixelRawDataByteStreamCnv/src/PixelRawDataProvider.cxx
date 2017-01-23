/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRawDataProvider.h"

#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"
#include "InDetIdentifier/PixelID.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "PixelCabling/IPixelCablingSvc.h"
  

// --------------------------------------------------------------------
// Constructor

PixelRawDataProvider::PixelRawDataProvider(const std::string& name,
				       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pixelCabling    ("PixelCablingSvc",name),
  m_robDataProvider ("ROBDataProviderSvc",name),
  m_rawDataTool     ("PixelRawDataProviderTool"),
  m_pixel_id        (NULL),
  m_rdoContainerKey("")
{
  declareProperty("RDOKey", m_rdoContainerKey = std::string("PixelRDOs"));
  declareProperty ("ROBDataProvider", m_robDataProvider);
  declareProperty ("ProviderTool", m_rawDataTool);
}

// Destructor

PixelRawDataProvider::~PixelRawDataProvider(){
}

// --------------------------------------------------------------------
// Initialize

StatusCode PixelRawDataProvider::initialize() {
  msg(MSG::INFO) << "PixelRawDataProvider::initialize" << endmsg;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved service " << m_robDataProvider << endmsg;
 
  // Get PixelRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_rawDataTool << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved tool " << m_rawDataTool << endmsg;
 

 if (detStore()->retrieve(m_pixel_id, "PixelID").isFailure()) {
     msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
     return StatusCode::FAILURE;
 }

  // Get the cabling service
  if (m_pixelCabling.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_pixelCabling << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved tool " << m_pixelCabling << endmsg;

  ATH_CHECK( m_rdoContainerKey.initialize() );
 
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------
// Execute

StatusCode PixelRawDataProvider::execute() {

#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Create Pixel RDO Container" << endmsg;
#endif

  // now create the container and register the collections

  // write into StoreGate
  SG::WriteHandle<PixelRDO_Container> rdoContainer(m_rdoContainerKey);
  rdoContainer = CxxUtils::make_unique<PixelRDO_Container>(m_pixel_id->wafer_hash_max()); 
  ATH_CHECK(rdoContainer.isValid());

  // Print ROB map in m_robDataProvider
  //m_robDataProvider->print_robmap();

  // ask ROBDataProviderSvc for the vector of ROBFragment for all Pixel ROBIDs
  std::vector<uint32_t> listOfRobs = m_pixelCabling->getAllRobs();  // need ROB id (not ROD)
  std::vector<const ROBFragment*> listOfRobf;
  m_robDataProvider->getROBData( listOfRobs, listOfRobf);


#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Number of ROB fragments " << listOfRobf.size() 
		    << " (out of=" << listOfRobs.size() << " expected)" << endmsg;
#endif

  // ask PixelRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert(listOfRobf,&(*rdoContainer)).isFailure())
    msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;


#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Number of Collections in IDC " << rdoContainer->numberOfCollections() << endmsg;
#endif

  return StatusCode::SUCCESS;
}

