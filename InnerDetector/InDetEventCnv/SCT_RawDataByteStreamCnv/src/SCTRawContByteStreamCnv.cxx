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

SCTRawContByteStreamCnv::SCTRawContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(ByteStream_StorageType, classID(),svcloc),
  m_tool{"SCTRawContByteStreamTool"},
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
  ATH_CHECK(m_tool.retrieve());
  m_log << MSG::INFO << "Retrieved tool " << m_tool << endmsg;

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
  if (cont==nullptr) {
    m_log << MSG::ERROR << " Can not cast to SCTRawContainer " << endmsg;
    return StatusCode::FAILURE;
  }

  /** set up the IOpaqueAddress for Storegate */
  std::string nm{pObj->registry()->name()};
  pAddr = new ByteStreamAddress(classID(), nm, "");

  /** now use the tool to do the conversion */
  ATH_CHECK(m_tool->convert(cont, re, m_log));

  return StatusCode::SUCCESS;
}
