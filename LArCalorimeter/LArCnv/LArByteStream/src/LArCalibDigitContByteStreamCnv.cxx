/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArCalibDigitContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "StoreGate/ReadCondHandle.h"
#include "AthenaKernel/StorableConversions.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArCalibDigitContByteStreamCnv::LArCalibDigitContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"LArCalibDigitContByteStreamCnv"),
  m_tool("LArRawDataContByteStreamTool"),
  m_rdpSvc("ROBDataProviderSvc", name()),
  m_byteStreamEventAccess("ByteStreamCnvSvc", name()),
  m_byteStreamCnvSvc(nullptr),
  m_calibLineMappingKey ("LArCalibLineMap"),
  m_onOffIdMappingKey ("LArOnOffIdMap")
{}

LArCalibDigitContByteStreamCnv::~LArCalibDigitContByteStreamCnv() {
}

const CLID& LArCalibDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArCalibDigitContainer>::ID() ;
}


StatusCode
LArCalibDigitContByteStreamCnv::initialize()
{
  ATH_CHECK( AthConstConverter::initialize() );

  if ( m_rdpSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING(  " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " );
  }

  ATH_CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  ATH_CHECK( m_tool.retrieve() );
  ATH_CHECK( m_calibLineMappingKey.initialize() );
  ATH_CHECK( m_onOffIdMappingKey.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode
LArCalibDigitContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const
{//Convert Digits from ByteStream to StoreGate
  ATH_MSG_VERBOSE( "Executing CreateObj method for LArCalibDigitContainer " );
  
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
  // get gain an pass to convert function.
  // get gain and pass to convert function.
  CaloGain::CaloGain gain=CaloGain::LARNGAIN; //At this place, LARNGAINS means Automatic gain.
  if (key=="HIGH")
    gain=CaloGain::LARHIGHGAIN;
  else if (key=="MEDIUM")
    gain=CaloGain::LARMEDIUMGAIN;
  else if (key=="LOW")
    gain=CaloGain::LARLOWGAIN;
  // Convert the RawEvent to  LArCalibDigitContainer
  ATH_MSG_DEBUG( "Converting LArCalibDigits (from ByteStream). key=" << key << " ,gain=" << gain );
 
  SG::ReadCondHandle<LArCalibLineMapping> calibLineMapping (m_calibLineMappingKey);
  SG::ReadCondHandle<LArOnOffIdMapping> onOffIdMapping (m_onOffIdMappingKey);

  LArCalibDigitContainer *CalibDigitContainer=new LArCalibDigitContainer;
  StatusCode sc=m_tool->convert(re,CalibDigitContainer,gain,
                                **calibLineMapping,
                                **onOffIdMapping);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Conversion tool returned an error. LArCalibDigitContainer might be empty." );
  }
    
  pObj = SG::asStorable(CalibDigitContainer) ;
  return StatusCode::SUCCESS;
}

StatusCode 
LArCalibDigitContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const
{// convert LArDigits from StoreGate into ByteStream
  ATH_MSG_VERBOSE( "Execute CreateRep method of LArCalibDigitContainer " );
  // Get Full Event Assembler
  FullEventAssembler<RodRobIdMap> *fea = 0;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea,"LAr") );

  LArCalibDigitContainer* CalibDigitContainer=0;
  if (!SG::fromStorable (pObj, CalibDigitContainer) || !CalibDigitContainer) {
    ATH_MSG_ERROR( "Cannot get LArCalibDigitContainer for DataObject. Key=" << pObj->registry()->name() );
    return StatusCode::FAILURE;    
  }

  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 

  ATH_CHECK( m_tool->WriteLArCalibDigits(CalibDigitContainer, *fea) );

  return StatusCode::SUCCESS;
}
