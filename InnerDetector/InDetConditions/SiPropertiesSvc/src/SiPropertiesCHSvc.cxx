/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SiPropertiesSvc/SiPropertiesCHSvc.h"

#include "Identifier/IdentifierHash.h"

SiPropertiesCHSvc::SiPropertiesCHSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_propertiesVector{"SCTSiliconPropertiesVector"},
  m_defaultProperties{}
{
  declareProperty("DetectorName", m_detectorName="SCT", "Dectector name: Pixel or SCT");
  declareProperty("ReadKey", m_propertiesVector, "Key of SiliconPropertiesVector");
}

SiPropertiesCHSvc::~SiPropertiesCHSvc()
{}

StatusCode 
SiPropertiesCHSvc::initialize()
{ 
  ATH_MSG_INFO("SiPropertiesCHSvc Initialized");
  
  StatusCode sc{AthService::initialize()};
  if (sc.isFailure()) { 
    ATH_MSG_FATAL("Unable to initialize the service!");
    return sc;
  } 
  
  if ((m_detectorName != "Pixel") and (m_detectorName != "SCT")) {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be Pixel or SCT.");
    return StatusCode::FAILURE;
  }

  bool isPixel{m_detectorName == "Pixel"};
  if (isPixel) {
    ATH_MSG_FATAL("PIXEL case is not implemented yet!!!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_propertiesVector.initialize());

  return StatusCode::SUCCESS;
}

StatusCode 
SiPropertiesCHSvc::finalize()
{
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode
SiPropertiesCHSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ISiPropertiesSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISiPropertiesSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode
SiPropertiesCHSvc::callBack(IOVSVC_CALLBACK_ARGS)
{  
  ATH_MSG_FATAL("SiPropertiesCHSvc::callBack is not implemented!!!");
  return StatusCode::FAILURE;
}

const InDet::SiliconProperties&
SiPropertiesCHSvc::getSiProperties(const IdentifierHash& elementHash) {
  SG::ReadCondHandle<InDet::SiliconPropertiesVector> handle{m_propertiesVector};
  if (handle.isValid()) {
    const InDet::SiliconPropertiesVector* vector{*handle};
    if (vector) {
      return vector->getSiProperties(elementHash);
    }
  }

  ATH_MSG_WARNING("SG::ReadCondHandle<InDet::SiliconPropertiesVector> is not valid. " << 
                  "Return default InDet::SiliconProperties for IdentifierHash for " <<
                  elementHash);
  return m_defaultProperties;
}
