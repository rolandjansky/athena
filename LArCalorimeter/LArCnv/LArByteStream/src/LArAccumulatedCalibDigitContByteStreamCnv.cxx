/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArAccumulatedCalibDigitContByteStreamCnv.h"
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

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRecConditions/LArCalibLineMapping.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArAccumulatedCalibDigitContByteStreamCnv::LArAccumulatedCalibDigitContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"LArAccumulatedCalibDigitContByteStreamCnv"),
  m_tool("LArRawDataContByteStreamTool"),
  m_rdpSvc("ROBDataProviderSvc", name()),
  m_calibLineMappingKey ("LArCalibLineMap")
{}

const CLID& LArAccumulatedCalibDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArAccumulatedCalibDigitContainer>::ID() ;
}

LArAccumulatedCalibDigitContByteStreamCnv::~LArAccumulatedCalibDigitContByteStreamCnv() {
}


StatusCode
LArAccumulatedCalibDigitContByteStreamCnv::initialize()
{
  ATH_CHECK( AthConstConverter::initialize() );

  if ( m_rdpSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING(  " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " );
  }

  ATH_CHECK( m_tool.retrieve() );
  ATH_CHECK( m_calibLineMappingKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
LArAccumulatedCalibDigitContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const
{//Convert Accumulated Calib Digits from ByteStream to StoreGate
  ATH_MSG_VERBOSE(  "Executing CreateObj method for LArAccumulatedCalibDigitContainer " );
  

  if (!m_rdpSvc) {
    ATH_MSG_ERROR( " ROBDataProviderSvc not loaded. Can't read ByteStream." );
    return StatusCode::FAILURE;
  }
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); //Cast from OpaqueAddress to ByteStreamAddress
  if (!pRE_Addr) {
    ATH_MSG_ERROR(  "dynamic_cast of OpaqueAdress to ByteStreamAddress failed!" );
    return StatusCode::FAILURE;
  }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re) {
    ATH_MSG_ERROR(  "Could not get raw event from ByteStreamInputSvc" );
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

  SG::ReadCondHandle<LArCalibLineMapping> calibLineMapping (m_calibLineMappingKey);

  // Convert the RawEvent to  LArAccumulatedCalibDigitContainer
  ATH_MSG_DEBUG(  "Converting LArAccumulatedCalibDigits (from ByteStream). key=" << key << " ,gain=" << gain );
  LArAccumulatedCalibDigitContainer *DigitContainer=new LArAccumulatedCalibDigitContainer;
  StatusCode sc=m_tool->convert(re,DigitContainer,gain,
                                **calibLineMapping);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING(  "Conversion tool returned an error. LArAccumulatedCalibDigitContainer might be empty." );
  }
  DigitContainer->setDelayScale(25./240.);  //FIXME should not be hardcoded! 
  pObj = SG::asStorable(DigitContainer) ;

  return StatusCode::SUCCESS;
}

// IWS 19.07.2005 
 StatusCode 
 LArAccumulatedCalibDigitContByteStreamCnv::createRepConst(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/)  const
{// convert LArAccumulatedCalibDigits from StoreGate into ByteStream
  ATH_MSG_ERROR( "CreateRep method of LArAccumulatedCalibDigitContainer not implemented" );
  return StatusCode::SUCCESS;
}

