/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  PixelRawContByteStreamCnv.cxx
//   Implementation file for class PixelRawContByteStreamCnv
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// classID() - returning PixelRDO_Container ID
// createObj() - creates PixelRDO_Container
// createRep() - convert Pixel_RDO in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "PixelRawContByteStreamCnv.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelRawContByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "InDetRawData/PixelRDORawData.h"

//#define PIXEL_DEBUG


////////////////////////
// constructor
////////////////////////
PixelRawContByteStreamCnv::PixelRawContByteStreamCnv(ISvcLocator* svcloc) : 
  Converter(storageType(), classID(),svcloc),
  m_PixelRawContBSTool(nullptr),
  m_ByteStreamEventAccess(nullptr),
  m_StoreGate(nullptr),
  m_log(msgSvc(), "PixelRawContByteStreamCnv")
{}

////////////////////////
// initialize()
////////////////////////
StatusCode PixelRawContByteStreamCnv::initialize() {

  StatusCode sc = Converter::initialize(); 
  if(StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  // Check ByteStreamCnvSvc
  if (StatusCode::SUCCESS != service("ByteStreamCnvSvc", m_ByteStreamEventAccess) || !m_ByteStreamEventAccess) {
    m_log << MSG::ERROR << "Can't get ByteStreamEventAccess interface" << endmsg;
    return StatusCode::FAILURE;
  }
  m_log << MSG::INFO << "ByteStreamCnvSvc retrieved" << endmsg;

  // retrieve Tool
  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)) {
    m_log << MSG::ERROR << "Can't get ToolSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  std::string toolType;

  toolType = "PixelRawContByteStreamTool"; 
  if(StatusCode::SUCCESS != toolSvc->retrieveTool(toolType,m_PixelRawContBSTool)) {
    m_log << MSG::ERROR << "Can't get PixelRawContByteStreamTool Tool" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get an Identifier helper object
  sc = service("DetectorStore",m_StoreGate);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "Detector service not found" << endmsg;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

////////////////////////
// classID() - returning PixelRDO_Container ID
////////////////////////
const CLID& PixelRawContByteStreamCnv::classID() {

  return ClassID_traits<PixelRDO_Container>::ID() ;

}

long PixelRawContByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

////////////////////////
// createRep() - convert Pixel_RDO in the container into ByteStream
////////////////////////
StatusCode PixelRawContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {

  //  StatusCode sc = StatusCode::SUCCESS; 
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent(); 
  PixelRDO_Container* cont=0; 
  StoreGateSvc::fromStorable(pObj,cont); 

  if(!cont) {
    m_log << MSG::ERROR << "Can not cast to PixelRDO_Container" << endmsg; 
    return StatusCode::FAILURE;    
  } 

  std::string nm = pObj->registry()->name(); 
  ByteStreamAddress* addr = new ByteStreamAddress(classID(),nm,""); 
  pAddr = addr; 
  //  return m_PixelRawContBSTool->convert(cont,re,m_log); 

  StatusCode sc = m_PixelRawContBSTool->convert(cont, re) ;
  if(sc.isFailure()){
    m_log << MSG::ERROR<< " Could not convert rdo with m_PixelRawContBSTool " << endmsg ;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS ;
}
