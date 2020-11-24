/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTRawContByteStreamCnv.h"

#include "ByteStreamData/RawEvent.h"
#include "AthenaKernel/StorableConversions.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"

// ------------------------------------------------------
// constructor

TRTRawContByteStreamCnv::TRTRawContByteStreamCnv(ISvcLocator* svcloc) :
    AthConstConverter(storageType(), classID(),svcloc,"TRTRawContByteStreamCnv"),
    m_tool                 ("TRTRawContByteStreamTool")                  // init tool handles
{}
 
// ------------------------------------------------------
// initialize

StatusCode
TRTRawContByteStreamCnv::initialize()
{
   ATH_CHECK( AthConstConverter::initialize() );

   ATH_MSG_DEBUG( " initialize " );

   // Retrieve byte stream tool
   ATH_CHECK( m_tool.retrieve() );
   ATH_MSG_INFO( "Retrieved tool " << m_tool );

   ATH_MSG_INFO( "Leaving TRTRawContByteStreamCnv::initialize()" );
   return StatusCode::SUCCESS; 

}

// ------------------------------------------------------
// convert TRT Raw Data in the container into ByteStream

StatusCode 
TRTRawContByteStreamCnv::createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr)  const
{
  // get IDC for TRT Raw Data
  TRT_RDO_Container* cont=0; 
  SG::fromStorable(pObj, cont); 
  if(!cont){
    ATH_MSG_ERROR( " Can not cast to TRTRawContainer " );
    return StatusCode::FAILURE;    
  } 
  
  // set up the IOpaqueAddress for Storegate
  std::string nm = pObj->registry()->name(); 
  pAddr = new ByteStreamAddress(classID(),nm,""); 

  // now use the tool to do the conversion
  ATH_CHECK( m_tool->convert(cont) );

  return StatusCode::SUCCESS ;
}
