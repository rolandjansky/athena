/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArRawChannelContByteStreamCnv.h"
#include "LArByteStream/LArRawDataContByteStreamTool.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "AthenaKernel/CLASS_DEF.h"

//STL-Stuff
#include <map> 
#include <iostream>
#include <string>


LArRawChannelContByteStreamCnv::LArRawChannelContByteStreamCnv(ISvcLocator* svcloc) :
  AthConstConverter(storageType(), classID(),svcloc,"LArRawChannelContByteStreamCnv"),
  m_tool("LArRawDataContByteStreamTool"),
  m_rdpSvc("ROBDataProviderSvc", name()),
  m_byteStreamEventAccess("ByteStreamCnvSvc", name()),
  m_byteStreamCnvSvc(nullptr),
  m_contSize(0)
{}

const CLID& LArRawChannelContByteStreamCnv::classID(){
  return ClassID_traits<LArRawChannelContainer>::ID() ;
}

LArRawChannelContByteStreamCnv::~LArRawChannelContByteStreamCnv() {
}


StatusCode
LArRawChannelContByteStreamCnv::initialize()
{
  ATH_CHECK( AthConstConverter::initialize() );
   
  if ( m_rdpSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING(  " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " );
  }

  ATH_CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode
LArRawChannelContByteStreamCnv::createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj)  const
{ //Convert RawChannels from ByteStream to StoreGate
    
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;    
  }
  
  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re) {
    ATH_MSG_ERROR( "Could not get raw event from ByteStreamInputSvc" );
    return StatusCode::FAILURE;
  }

  // create LArRawChannelContainer
  LArRawChannelContainer* channelContainer = new LArRawChannelContainer() ;
  if (m_contSize) channelContainer->reserve(m_contSize);

  StatusCode sc=m_tool->convert(re,channelContainer,CaloGain::CaloGain(0)); //Gain is a dummy variable in this case...
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Conversion tool returned an error. LArRawChannelContainer might be empty." );
  }
    
  pObj = StoreGateSvc::asStorable( channelContainer ) ; 

  ATH_MSG_DEBUG( "Created a LArRawChannelContainer of size" << channelContainer->size() );

  if (!m_contSize) {
    m_contSize=channelContainer->size();
    ATH_MSG_DEBUG( "For the following events, we will reserve space for " 
                   << m_contSize << " LArRawchannels" );
  }
    
  return StatusCode::SUCCESS;  
    
  
}

StatusCode 
LArRawChannelContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const
{ // convert LArRawChannels in the container into ByteStream
   
   // Get Full Event Assembler
  FullEventAssembler<RodRobIdMap> *fea = 0;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea,"LAr") );

  LArRawChannelContainer* ChannelContainer = nullptr;
  if (!SG::fromStorable (pObj, ChannelContainer ) || !ChannelContainer) {
    ATH_MSG_ERROR( " Can not cast to LArRawChannelContainer " );
    return StatusCode::FAILURE;    
  } 

  const std::string& nm = pObj->registry()->name(); 

  pAddr = new  ByteStreamAddress(classID(),nm,""); 

  ATH_CHECK( m_tool->WriteLArRawChannels(ChannelContainer,*fea) );
  return StatusCode::SUCCESS;
}
