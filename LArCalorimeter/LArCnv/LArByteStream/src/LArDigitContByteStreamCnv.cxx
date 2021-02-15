/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArDigitContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"
#include "LArByteStream/LArLATOMEDecoder.h"
#include "LArByteStream/LATOMEMapping.h"
#include "LArByteStream/Mon.h"


#include "CaloIdentifier/CaloGain.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "LArRawEvent/LArDigitContainer.h"

#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/CLASS_DEF.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArDigitContByteStreamCnv::LArDigitContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"LArDigitContByteStreamCnv"),
  m_tool("LArRawDataContByteStreamTool"),
  m_scTool("LArLATOMEDecoder"),
  m_rdpSvc("ROBDataProviderSvc", name()),
  m_byteStreamEventAccess("ByteStreamCnvSvc", name()),
  m_byteStreamCnvSvc(nullptr)
{}
    
LArDigitContByteStreamCnv::~LArDigitContByteStreamCnv() {
}

const CLID& LArDigitContByteStreamCnv::classID(){
  return ClassID_traits<LArDigitContainer>::ID() ;
}


StatusCode
LArDigitContByteStreamCnv::initialize()
{
  ATH_CHECK( AthConstConverter::initialize() );

  if ( m_rdpSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING(  " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " );
  }

  ATH_CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  ATH_CHECK( m_tool.retrieve() );
  ATH_CHECK( m_scTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode
LArDigitContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj)  const
{//Convert Digits from ByteStream to StoreGate
  ATH_MSG_VERBOSE( "Executing CreateObj method for LArDigitContainer " );
  
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
  bool isSC=false;
  LArDigitContainer* adc_coll=0;
  LArDigitContainer* adc_bas_coll=0;
  LArRawSCContainer* et_coll=0;
  LArRawSCContainer* et_id_coll=0;
  LArLATOMEHeaderContainer* header_coll=0;
  
  if (key=="HIGH")
    gain=CaloGain::LARHIGHGAIN;
  else if (key=="MEDIUM")
    gain=CaloGain::LARMEDIUMGAIN;
  else if (key=="LOW")
    gain=CaloGain::LARLOWGAIN;
  else if (key=="SC"){
    isSC=true;
    adc_coll=new LArDigitContainer();
  }
  else if(key=="SC_ADC_BAS"){
    isSC=true;
    adc_bas_coll=new LArDigitContainer();
  }
  
  // Convert the RawEvent to  LArDigitContainer
  ATH_MSG_DEBUG( "Converting LArDigits (from ByteStream). key=" << key << " ,gain=" << gain );
  StatusCode sc;
  if (!isSC) {//Regular readout
    LArDigitContainer *DigitContainer=new LArDigitContainer;
    sc=m_tool->convert(re,DigitContainer,gain);
    if (sc!=StatusCode::SUCCESS)
      ATH_MSG_WARNING( "Conversion tool returned an error. LArDigitContainer might be empty." );
    pObj = SG::asStorable(DigitContainer) ;
    return StatusCode::SUCCESS;
  }

  //Supercell readout
  sc=m_scTool->convert(re,adc_coll, adc_bas_coll, et_coll, et_id_coll, header_coll);
  if (sc!=StatusCode::SUCCESS)
    ATH_MSG_WARNING( "Conversion tool returned an error. LAr SC containers might be empty." );

  if (key=="SC"){
    pObj = SG::asStorable(adc_coll);
  }
  else if(key=="SC_ADC_BAS"){
    pObj = SG::asStorable(adc_bas_coll);
  }

  return StatusCode::SUCCESS;

}

StatusCode 
LArDigitContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr)  const
{// convert LArDigits from StoreGate into ByteStream
  ATH_MSG_VERBOSE( "Execute CreateRep method of LArDigitContainer " );

  // Get Full Event Assembler
  FullEventAssembler<RodRobIdMap> *fea = 0;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea,"LAr") );

  LArDigitContainer* DigitContainer=0;
  if (!SG::fromStorable (pObj, DigitContainer) || !DigitContainer) {
    ATH_MSG_ERROR( "Cannot get LArDigitContainer for DataObject. Key=" << pObj->registry()->name() );
    return StatusCode::FAILURE;    
  }

  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 

  ATH_CHECK( m_tool->WriteLArDigits(DigitContainer, *fea) );
  return StatusCode::SUCCESS;
}
