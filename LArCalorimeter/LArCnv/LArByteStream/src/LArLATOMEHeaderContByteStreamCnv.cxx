/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArLATOMEHeaderContByteStreamCnv.h"
#include "LArByteStream/LArLATOMEDecoder.h"


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

#include "LArRawEvent/LArDigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tool 
#include "GaudiKernel/IToolSvc.h"

//STL-Stuff
#include <map> 
#include <iostream>


LArLATOMEHeaderContByteStreamCnv::LArLATOMEHeaderContByteStreamCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(),svcloc), m_log(NULL),m_ByteStreamEventAccess(NULL),m_rdpSvc(NULL),m_storeGate(NULL){}
    
LArLATOMEHeaderContByteStreamCnv::~LArLATOMEHeaderContByteStreamCnv() {
 if (m_log) 
    delete m_log;
}

const CLID& LArLATOMEHeaderContByteStreamCnv::classID(){
  return ClassID_traits<LArLATOMEHeaderContainer>::ID() ;
}


StatusCode
LArLATOMEHeaderContByteStreamCnv::initialize()
{
   StatusCode sc = Converter::initialize(); 
   if(StatusCode::SUCCESS!=sc) 
   { 
    return sc; 
   } 
   m_log = new MsgStream(msgSvc(),"LArLATOMEHeaderBSCnv");
   (*m_log) << MSG::DEBUG<< " initialize " <<endmsg; 
   IService* svc;
  //Get ByteStreamCnvSvc
  if(StatusCode::SUCCESS != serviceLocator()->getService("ByteStreamCnvSvc",svc)){
    (*m_log) << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
  }
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      (*m_log) <<MSG::ERROR<< "  LArLATOMEHeaderContByteStreamCnv: Can't cast to  ByteStreamCnvSvc " <<endmsg; 
      return StatusCode::FAILURE ;
    }

  //Get ByteStreamInputSvc (only necessary for reading of digits, not for writing and for channels)
  
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    (*m_log) << MSG::WARNING << " Can't get ByteStreamInputSvc interface Reading of ByteStream Data not possible. " << endmsg;
    m_rdpSvc=0;
  }
  else {
    m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
    if(m_rdpSvc == 0 ) {
      (*m_log) <<MSG::ERROR<< "  LArLATOMEHeaderContByteStreamCnv: Can't cast to  ByteStreamInputSvc " <<endmsg; 
      return StatusCode::FAILURE;
    }
  }

  // retrieve ToolSvc
  IToolSvc* toolSvc;

  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    (*m_log) << MSG::ERROR << " Can't get ToolSvc " << endmsg;
    return StatusCode::FAILURE;
  }
  
  //Get SuperCellTool
  sc=toolSvc->retrieveTool("LArLATOMEDecoder",m_scTool);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Can't get LArLATOMEDecoder Tool" << endmsg;
    return sc;
  } else {
     (*m_log) << MSG::INFO << "Retrieved LArLATOMEDecoder Tool" << endmsg;
  }

  //Get StoreGateSvc
  return service("StoreGateSvc", m_storeGate); 

}

StatusCode
LArLATOMEHeaderContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{//Convert Digits from ByteStream to StoreGate
  (*m_log) << MSG::VERBOSE << "Executing CreateObj method for  LArLATOMEHeaderContainer" << endmsg;

  if (!m_rdpSvc)
    {(*m_log) << MSG::ERROR << " ROBDataProviderSvc not loaded. Can't read ByteStream." << endmsg;
     return StatusCode::FAILURE;
    }
  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); //Cast from OpaqueAddress to ByteStreamAddress
  if (!pRE_Addr)
    {(*m_log) << MSG::ERROR << "dynamic_cast of OpaqueAdress to ByteStreamAddress failed!" << endmsg;
     return StatusCode::FAILURE;
    }

  const RawEvent* re = m_rdpSvc->getEvent();
  if (!re)
    {(*m_log) << MSG::ERROR << "Could not get raw event from ByteStreamInputSvc" << endmsg;
     return StatusCode::FAILURE;
    }
  const std::string& key = *(pAddr->par()); // Get key used in the StoreGateSvc::retrieve function
  // get gain and pass to convert function.
  LArDigitContainer* adc_coll=0;
  LArDigitContainer* adc_bas_coll=0;
  LArRawSCContainer* et_coll=0;
  LArRawSCContainer* et_id_coll=0;
  LArLATOMEHeaderContainer* header_coll=new LArLATOMEHeaderContainer(); 
  
  // Convert the RawEvent to  LArDigitContainer
  (*m_log) << MSG::DEBUG << "Converting LATOME_HEADER (from ByteStream). key=" << key << endmsg; 
  StatusCode sc;

  //Supercell readout
  sc=m_scTool->convert(re,adc_coll, adc_bas_coll, et_coll, et_id_coll, header_coll);
  if (sc!=StatusCode::SUCCESS)
    (*m_log) << MSG::WARNING << "Conversion tool returned an error. LAr SC containers might be empty." << endmsg;
  pObj = SG::asStorable(header_coll);
  
  return StatusCode::SUCCESS;

}

 StatusCode 
LArLATOMEHeaderContByteStreamCnv::createRep(DataObject* , IOpaqueAddress*& ) 
{
   return StatusCode::SUCCESS;
}
