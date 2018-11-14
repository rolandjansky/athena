/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawContByteStreamCnv.h"

// Gaudi
#include "GaudiKernel/DataObject.h"

// Athena
#include "AthenaBaseComps/AthCheckMacros.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h"
#include "StoreGate/StoreGateSvc.h"

/// ------------------------------------------------------
/// constructor

SCTRawContByteStreamCnv::SCTRawContByteStreamCnv(ISvcLocator* svcLoc) :
  Converter(ByteStream_StorageType, classID(),svcLoc),
  m_rawContByteStreamTool{"SCTRawContByteStreamTool"},
  m_byteStreamEventAccess{"ByteStreamCnvSvc", "SCTRawContByteStreamCnv"},
  m_log{msgSvc(), "SCTRawContByteStreamCnv"}
{
}

/// ------------------------------------------------------
/// initialize

StatusCode
SCTRawContByteStreamCnv::initialize()
{
  ATH_CHECK(Converter::initialize());
  m_log << MSG::DEBUG<< " initialize " << endmsg;

  /** Retrieve ByteStreamCnvSvc */
  ATH_CHECK(m_byteStreamEventAccess.retrieve());
  m_log << MSG::INFO << "Retrieved service " << m_byteStreamEventAccess << endmsg;

  /** Retrieve byte stream tool */
  ATH_CHECK(m_rawContByteStreamTool.retrieve());
  m_log << MSG::INFO << "Retrieved tool " << m_rawContByteStreamTool << endmsg;

  return StatusCode::SUCCESS;
}

/// ------------------------------------------------------
/// convert SCT Raw Data in the container into ByteStream

StatusCode
SCTRawContByteStreamCnv::createRep(DataObject* pDataObject, IOpaqueAddress*& pOpaqueAddress)
{
  /** get RawEvent pointer */
  RawEventWrite* rawEvtWrite{m_byteStreamEventAccess->getRawEvent()};

  /** get IDC for SCT Raw Data */
  SCT_RDO_Container* sctRDOCont{nullptr};
  StoreGateSvc::fromStorable(pDataObject, sctRDOCont);
  if (sctRDOCont==nullptr) {
    m_log << MSG::ERROR << " Can not cast to SCTRawContainer " << endmsg;
    return StatusCode::FAILURE;
  }

  /** set up the IOpaqueAddress for Storegate */
  std::string dataObjectName{pDataObject->registry()->name()};
  pOpaqueAddress = new ByteStreamAddress(classID(), dataObjectName, "");

  /** now use the tool to do the conversion */
  ATH_CHECK(m_rawContByteStreamTool->convert(sctRDOCont, rawEvtWrite, m_log));

  return StatusCode::SUCCESS;
}
