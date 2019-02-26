/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawContByteStreamCnv.h"

#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

#include "AthenaBaseComps/AthCheckMacros.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"

// Constructor

SCTRawContByteStreamCnv::SCTRawContByteStreamCnv(ISvcLocator* svcLoc) :
  Converter(storageType(), classID(),svcLoc),
  m_rawContByteStreamTool{"SCTRawContByteStreamTool"},
  m_byteStreamEventAccess{"ByteStreamCnvSvc", "SCTRawContByteStreamCnv"},
  m_log{msgSvc(), "SCTRawContByteStreamCnv"}
{
}

// Initialize

StatusCode SCTRawContByteStreamCnv::initialize()
{
  ATH_CHECK(Converter::initialize());
  m_log << MSG::DEBUG<< " initialize " << endmsg;

  // Retrieve ByteStreamCnvSvc
  ATH_CHECK(m_byteStreamEventAccess.retrieve());
  m_log << MSG::INFO << "Retrieved service " << m_byteStreamEventAccess << endmsg;

  // Retrieve byte stream tool
  ATH_CHECK(m_rawContByteStreamTool.retrieve());
  m_log << MSG::INFO << "Retrieved tool " << m_rawContByteStreamTool << endmsg;

  return StatusCode::SUCCESS;
}

// Method to create RawEvent fragments

StatusCode SCTRawContByteStreamCnv::createRep(DataObject* pDataObject, IOpaqueAddress*& pOpaqueAddress)
{
  // Get RawEvent pointer
  RawEventWrite* rawEvtWrite{m_byteStreamEventAccess->getRawEvent()};

  // Get IDC for SCT Raw Data
  SCT_RDO_Container* sctRDOCont{nullptr};
  StoreGateSvc::fromStorable(pDataObject, sctRDOCont);
  if (sctRDOCont == nullptr) {
    m_log << MSG::ERROR << " Can not cast to SCTRawContainer " << endmsg;
    return StatusCode::FAILURE;
  }

  // Set up the IOpaqueAddress for Storegate
  std::string dataObjectName{pDataObject->registry()->name()};
  pOpaqueAddress = new ByteStreamAddress(classID(), dataObjectName, "");

  // Use the tool to do the conversion
  ATH_CHECK(m_rawContByteStreamTool->convert(sctRDOCont, rawEvtWrite, m_log));

  return StatusCode::SUCCESS;
}
