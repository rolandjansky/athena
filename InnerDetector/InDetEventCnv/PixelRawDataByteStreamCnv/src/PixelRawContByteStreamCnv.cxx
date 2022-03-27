/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "PixelRawContByteStreamTool.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "InDetRawData/PixelRDORawData.h"
#include "AthenaKernel/StorableConversions.h"

//#define PIXEL_DEBUG


////////////////////////
// constructor
////////////////////////
PixelRawContByteStreamCnv::PixelRawContByteStreamCnv(ISvcLocator* svcloc) : 
  AthConstConverter(storageType(), classID(),svcloc, "PixelRawContByteStreamCnv"),
  m_PixelRawContBSTool(nullptr),
  m_ByteStreamEventAccess("ByteStreamCnvSvc", "PixelRawContByteStreamCnv")
{}

////////////////////////
// initialize()
////////////////////////
StatusCode PixelRawContByteStreamCnv::initialize() {

  ATH_CHECK( AthConstConverter::initialize() );

  ATH_CHECK( m_ByteStreamEventAccess.retrieve() );
  ATH_MSG_INFO( "ByteStreamCnvSvc retrieved" );

  ServiceHandle<IToolSvc> toolSvc ("ToolSvc", name());
  ATH_CHECK( toolSvc.retrieve() );

  const std::string toolType = "PixelRawContByteStreamTool"; 
  ATH_CHECK( toolSvc->retrieveTool(toolType,m_PixelRawContBSTool) );

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
StatusCode PixelRawContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const {

  PixelRDO_Container* cont=nullptr; 
  SG::fromStorable(pObj,cont); 

  if(!cont) {
    ATH_MSG_ERROR( "Can not cast to PixelRDO_Container" );
    return StatusCode::FAILURE;
  } 

  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 

  ATH_CHECK( m_PixelRawContBSTool->convert(cont) );

  return StatusCode::SUCCESS ;
}
