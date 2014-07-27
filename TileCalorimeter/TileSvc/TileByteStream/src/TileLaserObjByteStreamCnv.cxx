/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//author Renato Febbraro
//renato.febbraro@cern.ch
//date February 2008

#include "TileByteStream/TileLaserObjByteStreamCnv.h"
#include "TileByteStream/TileLaserObjByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"

#include "TileEvent/TileLaserObject.h"

#include "StoreGate/StoreGate.h"
#include "CLIDSvc/CLASS_DEF.h"


TileLaserObjByteStreamCnv::TileLaserObjByteStreamCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(),svcloc),
    m_tool(0),
    m_decoder(0),
    m_ByteStreamEventAccess(0),
    m_container(0),
    m_storeGate(0),
    m_robFrag(0),
    m_rdpSvc(0),
    m_ROBID()
{
}

const CLID& TileLaserObjByteStreamCnv::classID(){
return ClassID_traits<TileLaserObject>::ID() ;
}


StatusCode
TileLaserObjByteStreamCnv::initialize()
{
  StatusCode sc = Converter::initialize(); 
  if (sc.isFailure()) {
    return sc; 
  } 

  MsgStream log(msgSvc(), "TileLaserObjByteStreamCnv");
  log << MSG::DEBUG<< " initialize " <<endreq; 

  //Get ByteStreamInputSvc 
  IService* svc;
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    log << MSG::WARNING << " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " << endreq;
    m_rdpSvc=0;
  }
  else {
    m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
    if(m_rdpSvc == 0 ) {
      log<<MSG::ERROR<< " Can't cast to  ByteStreamInputSvc " <<endreq; 
      return StatusCode::FAILURE;
    }
  }

  // Get ByteStreamCnvSvc
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    log << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endreq;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      log << MSG::ERROR << "  TileLaserObjByteStreamCnv: Can't cast to  ByteStreamCnvSvc " << endreq; 
      return StatusCode::FAILURE ;
    }

  // retrieve Tool
  
  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    log << MSG::ERROR << " Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }

//   std::string toolType = "TileLaserObjByteStreamTool" ;
//   if(StatusCode::SUCCESS !=toolSvc->retrieveTool(toolType,m_tool))
//   {
//     log << MSG::ERROR << " Can't get ByteStreamTool " << endreq;
//     return StatusCode::FAILURE;
//   }

  if(StatusCode::SUCCESS !=toolSvc->retrieveTool("TileROD_Decoder",m_decoder)) {
    log << MSG::ERROR << "Can't get TileROD_Decoder"
	<< endreq;
    return StatusCode::FAILURE; 
  }

  m_ROBID.clear();
  //  m_ROBID.push_back( 0x500000 );
  m_ROBID.push_back( 0x520010 );

  return service("StoreGateSvc",m_storeGate) ;
}


StatusCode
TileLaserObjByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  MsgStream log(msgSvc(), "TileLaserObjByteStreamCnv");
  log << MSG::DEBUG << " Executing createObj method" << endreq;

  if (!m_rdpSvc) {
    log << MSG::ERROR << " ROBDataProviderSvc not loaded. Can't read ByteStream." << endreq;
    return StatusCode::FAILURE;
  }

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq ; 
    return StatusCode::FAILURE;    
  }

  //  const RawEvent* re = m_rdpSvc->getEvent();
  //if (!re) {
  //  log << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endreq;
  //  return StatusCode::FAILURE;
  //}

  /*FIND ROB*/
  std::vector<const ROBDataProviderSvc::ROBF*> robf;
  m_rdpSvc->getROBData(m_ROBID, robf);

  // create TileLaserObject
  m_container = new TileLaserObject() ; 

  if (robf.size() > 0 ) {
    m_robFrag = robf[0];
    m_decoder->fillTileLaserObj(m_robFrag,*m_container);
  } else {
    log << MSG::DEBUG << " No LASTROD fragment in BS, TileLaserObject will be empty." << endreq;
    m_robFrag = 0;
  }

  pObj = StoreGateSvc::asStorable( m_container ) ; 
  return StatusCode::SUCCESS;  
}

StatusCode 
TileLaserObjByteStreamCnv::createRep(DataObject* /* pObj */, IOpaqueAddress*& /* pAddr */)
{
  // No conversion from TileLaserObj to BS 

  MsgStream log(msgSvc(), "TileLaserObjByteStreamCnv");

  log << MSG::ERROR <<" Can not create BS from TileLaserObject "<<endreq; 

  return   StatusCode::FAILURE ; 
}

