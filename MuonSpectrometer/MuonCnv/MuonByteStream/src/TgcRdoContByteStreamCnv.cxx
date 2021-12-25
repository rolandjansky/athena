/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/TgcRdoContByteStreamCnv.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IToolSvc.h"

#include "MuonRDO/TgcRdoIdHash.h"

#include "AthenaKernel/StorableConversions.h"

#include <sstream>
#include <inttypes.h>

const std::string const_cnvName = "TgcRdoContByteStreamCnv";


// constructor
TgcRdoContByteStreamCnv::TgcRdoContByteStreamCnv(ISvcLocator* svcloc) 
  : AthConstConverter(storageType(), classID(), svcloc, "TgcRdoContByteStreamCnv"),
  m_tool("Muon::TgcRdoContByteStreamTool")
{}


// class ID
const CLID& TgcRdoContByteStreamCnv::classID()
{
  return ClassID_traits<TgcRdoContainer>::ID();
}

long TgcRdoContByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

// initialize
StatusCode TgcRdoContByteStreamCnv::initialize() 
{
  ATH_MSG_DEBUG( " initialize "  );
  ATH_CHECK( Converter::initialize() );
  ATH_CHECK( m_tool.retrieve() );
  return StatusCode::SUCCESS;
}

// convert TGC RDOs in the container into ByteStream
StatusCode
TgcRdoContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const
{
   // retrieve TGC RDO container
   TgcRdoContainer * cont(nullptr); 
   SG::fromStorable(pObj, cont);
   if (!cont)
     {
       ATH_MSG_ERROR( " Can not cast to TgcRdoContainer"  );
       return StatusCode::FAILURE;    
     } 

   // create address
   std::string nm = pObj->registry()->name(); 
   pAddr = new ByteStreamAddress(classID(),nm,""); 

   // convert
   return m_tool->convert(cont); 
}
