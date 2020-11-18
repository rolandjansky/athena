/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArAccumulatedDigitContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"

#include "CaloIdentifier/CaloGain.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "LArRawEvent/LArAccumulatedDigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArAccumulatedDigitContByteStreamCnv::LArAccumulatedDigitContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"LArAccumulatedDigitContByteStreamCnv"),
  m_tool("LArRawDataContByteStreamTool"),
  m_rdpSvc("ROBDataProviderSvc", name())
{}

const CLID& LArAccumulatedDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArAccumulatedDigitContainer>::ID() ;
}

LArAccumulatedDigitContByteStreamCnv::~LArAccumulatedDigitContByteStreamCnv() {
}


StatusCode
LArAccumulatedDigitContByteStreamCnv::initialize()
{
  ATH_CHECK( AthConstConverter::initialize() );

  if ( m_rdpSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING(  " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " );
  }

  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;

}

StatusCode
LArAccumulatedDigitContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj)  const
{//Convert Accumulated  Digits from ByteStream to StoreGate
  ATH_MSG_VERBOSE(  "Executing CreateObj method for LArAccumulatedDigitContainer ");

  if (!m_rdpSvc) {
    ATH_MSG_ERROR( " ROBDataProviderSvc not loaded. Can't read ByteStream." );
    return StatusCode::FAILURE;
  }
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); //Cast from OpaqueAddress to ByteStreamAddress
  if (!pRE_Addr) {
    ATH_MSG_ERROR( "dynamic_cast of OpaqueAdress to ByteStreamAddress failed!" );
    return StatusCode::FAILURE;
  }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re) {
    ATH_MSG_ERROR( "Could not get raw event from ByteStreamInputSvc" );
    return StatusCode::FAILURE;
  }
  const std::string& key = *(pAddr->par()); // Get key used in the StoreGateSvc::retrieve function
  // get gain and pass to convert function.
  CaloGain::CaloGain gain=CaloGain::LARNGAIN; //At this place, LARNGAINS means Automatic gain.
  if (key=="HIGH")
    gain=CaloGain::LARHIGHGAIN;
  else if (key=="MEDIUM")
    gain=CaloGain::LARMEDIUMGAIN;
  else if (key=="LOW")
    gain=CaloGain::LARLOWGAIN;

  // Convert the RawEvent to  LArAccumulatedDigitContainer
  ATH_MSG_DEBUG(  "Converting LArAccumulatedDigits (from ByteStream). key=" << key << " ,gain=" << gain );
  LArAccumulatedDigitContainer *DigitContainer=new LArAccumulatedDigitContainer(SG::VIEW_ELEMENTS);
  StatusCode sc=m_tool->convert(re,DigitContainer,gain);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Conversion tool returned an error. LArAccumulatedDigitContainer might be empty." );
  }
  pObj = SG::asStorable(DigitContainer) ;

  return StatusCode::SUCCESS;
}
// IWS 19.07.2005 
 StatusCode 
 LArAccumulatedDigitContByteStreamCnv::createRepConst(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/)  const
{// convert LArAccumulatedDigits from StoreGate into ByteStream
  ATH_MSG_ERROR( "CreateRep method of LArAccumulatedDigitContainer not implemented" );
  return StatusCode::SUCCESS;
}

