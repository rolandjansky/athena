/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArLATOMEHeaderContByteStreamCnv.h"
#include "LArByteStream/LArLATOMEDecoder.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "AthenaKernel/CLASS_DEF.h"


LArLATOMEHeaderContByteStreamCnv::LArLATOMEHeaderContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"LArLATOMEHeaderContByteStreamCnv"),
  m_scTool("LArLATOMEDecoder"),
  m_rdpSvc("ROBDataProviderSvc", name()) {};


LArLATOMEHeaderContByteStreamCnv::~LArLATOMEHeaderContByteStreamCnv() {}

const CLID& LArLATOMEHeaderContByteStreamCnv::classID(){
  return ClassID_traits<LArLATOMEHeaderContainer>::ID() ;
}


StatusCode
LArLATOMEHeaderContByteStreamCnv::initialize() {
  ATH_MSG_DEBUG("initialize ");
  ATH_CHECK( AthConstConverter::initialize());
  ATH_CHECK( m_rdpSvc.retrieve());
  ATH_CHECK( m_scTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode
LArLATOMEHeaderContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const {
  ATH_MSG_VERBOSE("Executing CreateObj method for  LArLATOMEHeaderContainer");

  ByteStreamAddress *pRE_Addr= dynamic_cast<ByteStreamAddress*>(pAddr); //Cast from OpaqueAddress to ByteStreamAddress
  if (!pRE_Addr)
    {ATH_MSG_ERROR("dynamic_cast of OpaqueAdress to ByteStreamAddress failed!");
     return StatusCode::FAILURE;
    }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re)
    {ATH_MSG_ERROR("Could not get raw event from ByteStreamInputSvc");
     return StatusCode::FAILURE;
    }
  const std::string& key = *(pAddr->par()); // Get key used in the StoreGateSvc::retrieve function
  // get gain and pass to convert function.
  std::unique_ptr<LArLATOMEHeaderContainer> header_coll=std::make_unique<LArLATOMEHeaderContainer>(); 
  
  // Convert the RawEvent to  LArDigitContainer
  ATH_MSG_VERBOSE("Converting LATOME_HEADER (from ByteStream). key=" << key);
  

  //Supercell readout
  StatusCode sc=m_scTool->convert(re,nullptr,nullptr, nullptr, nullptr, nullptr, header_coll.get());
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING("Conversion tool returned an error. LAr SC containers might be empty.");
  }
  pObj = SG::asStorable(std::move(header_coll));
  
  return StatusCode::SUCCESS;

}

 StatusCode 
LArLATOMEHeaderContByteStreamCnv::createRepConst(DataObject* , IOpaqueAddress*& ) const 
{
   return StatusCode::SUCCESS;
}
