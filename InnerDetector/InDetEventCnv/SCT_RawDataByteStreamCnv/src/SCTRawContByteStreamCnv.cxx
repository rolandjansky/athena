/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  AthConstConverter(storageType(), classID(), svcLoc, "SCTRawContByteStreamCnv"),
  m_rawContByteStreamTool{"SCTRawContByteStreamTool"},
  m_byteStreamEventAccess{"ByteStreamCnvSvc", "SCTRawContByteStreamCnv"}
{
}

// Initialize

StatusCode SCTRawContByteStreamCnv::initialize()
{
  ATH_CHECK(AthConstConverter::initialize());
  ATH_MSG_DEBUG( " initialize " );

  // Retrieve ByteStreamCnvSvc
  ATH_CHECK(m_byteStreamEventAccess.retrieve());
  ATH_MSG_INFO( "Retrieved service " << m_byteStreamEventAccess );

  // Retrieve byte stream tool
  ATH_CHECK(m_rawContByteStreamTool.retrieve());
  ATH_MSG_INFO( "Retrieved tool " << m_rawContByteStreamTool );

  return StatusCode::SUCCESS;
}

// Method to create RawEvent fragments

StatusCode SCTRawContByteStreamCnv::createRepConst(DataObject* pDataObject, IOpaqueAddress*& pOpaqueAddress) const
{
  // Get IDC for SCT Raw Data
  SCT_RDO_Container* sctRDOCont{nullptr};
  StoreGateSvc::fromStorable(pDataObject, sctRDOCont);
  if (sctRDOCont == nullptr) {
    ATH_MSG_ERROR( " Can not cast to SCTRawContainer " );
    return StatusCode::FAILURE;
  }

  // Set up the IOpaqueAddress for Storegate
  std::string dataObjectName{pDataObject->registry()->name()};
  pOpaqueAddress = new ByteStreamAddress(classID(), dataObjectName, "");

  // Use the tool to do the conversion
  ATH_CHECK(m_rawContByteStreamTool->convert(sctRDOCont) );

  return StatusCode::SUCCESS;
}
