/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawContByteStreamCnv.h"

// Gaudi
#include "GaudiKernel/DataObject.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamService.h"

/// ------------------------------------------------------
/// constructor

SCTRawContByteStreamCnv::SCTRawContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(ByteStream_StorageType, classID(),svcloc),
  m_service{"SCTRawContByteStreamService", "SCTRawContByteStreamCnv"},
  m_byteStreamEventAccess{"ByteStreamCnvSvc", "SCTRawContByteStreamCnv"},
  m_log{msgSvc(), "SCTRawContByteStreamCnv"}
{}
 
/// ------------------------------------------------------
/// destructor

SCTRawContByteStreamCnv::~SCTRawContByteStreamCnv( )
{}

/// ------------------------------------------------------
/// initialize

StatusCode
SCTRawContByteStreamCnv::initialize()
{
  StatusCode sc{Converter::initialize()};
  if(StatusCode::SUCCESS!=sc) return sc; 
  
  m_log << MSG::DEBUG<< " initialize " <<endmsg; 

  /** Retrieve ByteStreamCnvSvc */
  if (m_byteStreamEventAccess.retrieve().isFailure()) {
    m_log << MSG::FATAL << "Failed to retrieve service " << m_byteStreamEventAccess << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_log << MSG::INFO << "Retrieved service " << m_byteStreamEventAccess << endmsg;
  }

  /** Retrieve byte stream Service */
  if (m_service.retrieve().isFailure()) {
    m_log << MSG::FATAL << "Failed to retrieve service " << m_service << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_log << MSG::INFO << "Retrieved service " << m_service << endmsg;
  }

  return StatusCode::SUCCESS; 
   
}

/// ------------------------------------------------------
/// convert SCT Raw Data in the container into ByteStream

StatusCode 
SCTRawContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) 
{
  
  /** get RawEvent pointer */
  RawEventWrite* re{m_byteStreamEventAccess->getRawEvent()};
  
  /** get IDC for SCT Raw Data */
  SCT_RDO_Container* cont{nullptr};
  StoreGateSvc::fromStorable(pObj, cont); 
  if (!cont) {
    m_log << MSG::ERROR << " Can not cast to SCTRawContainer " << endmsg;
    return StatusCode::FAILURE;    
  } 
  
  /** set up the IOpaqueAddress for Storegate */
  std::string nm{pObj->registry()->name()};
  pAddr = new ByteStreamAddress(classID(), nm, ""); 

  /** now use the service to do the conversion */
  StatusCode sc{m_service->convert(cont, re, m_log)};
  if (sc.isFailure()) {
    m_log << MSG::ERROR
          << " Could not convert rdo with SCTRawContByteStreamSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
