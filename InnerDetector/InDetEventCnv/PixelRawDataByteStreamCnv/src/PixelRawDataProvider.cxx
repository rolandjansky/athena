/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRawDataProvider.h"

#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"
#include "InDetIdentifier/PixelID.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "PixelCabling/IPixelCablingSvc.h"
  

// --------------------------------------------------------------------
// Constructor

PixelRawDataProvider::PixelRawDataProvider(const std::string& name,
				       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pixelCabling    ("PixelCablingSvc",name),
  m_robDataProvider ("ROBDataProviderSvc",name),
  m_rawDataTool     ("PixelRawDataProviderTool"),
  m_pixel_id        (NULL)
{
  declareProperty ("RDOKey"      , m_RDO_Key = "PixelRDOs");
  declareProperty ("ROBDataProvider", m_robDataProvider);
  declareProperty ("ProviderTool", m_rawDataTool);
}

// Destructor

PixelRawDataProvider::~PixelRawDataProvider(){
}

// --------------------------------------------------------------------
// Initialize

StatusCode PixelRawDataProvider::initialize() {
  msg(MSG::INFO) << "PixelRawDataProvider::initialize" << endreq;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endreq;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved service " << m_robDataProvider << endreq;
 
  // Get PixelRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_rawDataTool << endreq;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved tool " << m_rawDataTool << endreq;
 

 if (detStore()->retrieve(m_pixel_id, "PixelID").isFailure()) {
     msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
 }

  // Get the cabling service
  if (m_pixelCabling.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_pixelCabling << endreq;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved tool " << m_pixelCabling << endreq;
 
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------
// Execute

StatusCode PixelRawDataProvider::execute() {

#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Create Pixel RDO Container" << endreq;
#endif

  // now create the container and register the collections
  PixelRDO_Container *container = new PixelRDO_Container(m_pixel_id->wafer_hash_max()); 

  // write into StoreGate
  if (evtStore()->record(container, m_RDO_Key).isFailure()) {
    msg(MSG::FATAL) << "Unable to record Pixel RDO Container" << endreq;
    return StatusCode::FAILURE;
  }

  // Print ROB map in m_robDataProvider
  //m_robDataProvider->print_robmap();

  // ask ROBDataProviderSvc for the vector of ROBFragment for all Pixel ROBIDs
  std::vector<uint32_t> listOfRobs = m_pixelCabling->getAllRobs();  // need ROB id (not ROD)
  std::vector<const ROBFragment*> listOfRobf;
  m_robDataProvider->getROBData( listOfRobs, listOfRobf);


#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Number of ROB fragments " << listOfRobf.size() 
		    << " (out of=" << listOfRobs.size() << " expected)" << endreq;
#endif

  // ask PixelRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert(listOfRobf,container).isFailure())
    msg(MSG::ERROR) << "BS conversion into RDOs failed" << endreq;

#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Number of Collections in IDC " << container->numberOfCollections() << endreq;
#endif

  return StatusCode::SUCCESS;
}

