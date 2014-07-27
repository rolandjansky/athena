/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileHid2RESrcID.h"

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

#include "TileIdentifier/TileTBFrag.h"
#include "TileEvent/TileBeamElemContainer.h"

#include "StoreGate/StoreGate.h"
#include "CLIDSvc/CLASS_DEF.h"


TileBeamElemContByteStreamCnv::TileBeamElemContByteStreamCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(),svcloc),
    m_container(0),
    m_storeGate(0),
    m_event(0),
    m_robFrag(0),
    m_RobSvc(0),
    m_ROBID(),
    m_decoder(0),
    m_hid2re(0)
{
}

const CLID& TileBeamElemContByteStreamCnv::classID(){
return ClassID_traits<TileBeamElemContainer>::ID() ;
}


StatusCode
TileBeamElemContByteStreamCnv::initialize()
{
  StatusCode sc = Converter::initialize(); 
  if (sc.isFailure()) {
    return sc; 
  } 

  MsgStream log(msgSvc(), "TileBeamElemContByteStreamCnv");
  log << MSG::DEBUG<< " initialize " <<endreq; 

  StoreGateSvc* detStore;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get pointer to Detector Store Service"
	<< endreq;
    return sc;
  }

  // retrieve Tool
  
  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    log << MSG::ERROR << " Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }

  if(StatusCode::SUCCESS !=toolSvc->retrieveTool("TileROD_Decoder",m_decoder)) {
    log << MSG::ERROR << "Can't get TileROD_Decoder"
	<< endreq;
    return StatusCode::FAILURE; 
  }

  m_hid2re = m_decoder->getHid2re();

  sc = serviceLocator()->service("ROBDataProviderSvc",m_RobSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR <<" Can't get ROBDataProviderSvc " << endreq;
  }

  m_ROBID.clear();
  m_ROBID.push_back( m_hid2re->getRobFromFragID(DIGI_PAR_FRAG) );
  m_ROBID.push_back( m_hid2re->getRobFromFragID(LASER_OBJ_FRAG) );

  // create empty TileBeamElemContainer and all collections inside
  m_container = new TileBeamElemContainer(true); 
  m_container->addRef(); // make sure it's not deleted at the end of event

  return service("StoreGateSvc",m_storeGate) ;
}


StatusCode
TileBeamElemContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  MsgStream log(msgSvc(), "TileBeamElemContByteStreamCnv");
  log << MSG::DEBUG << " Executing createObj method" << endreq;

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq ; 
    return StatusCode::FAILURE;    
  }

  std::vector<uint32_t> robid(1); robid[0] = 0;
  std::vector<const ROBDataProviderSvc::ROBF*> robf;

  // keep pointer to whole event and to CIS PAR frag internally
  m_event = m_RobSvc->getEvent();
  m_RobSvc->getROBData(m_ROBID, robf);
  m_robFrag = (robf.size() > 0 ) ? robf[0] : 0;

  // iterate over all collections in a container and fill them
  TileBeamElemContainer::const_iterator collItr=m_container->begin();
  TileBeamElemContainer::const_iterator lastColl=m_container->end();

  for(; collItr!=lastColl; ++collItr) {

    const TileBeamElemCollection * constColl = (*collItr);
    TileBeamElemCollection * coll = (TileBeamElemCollection *)constColl;
    coll->clear();
    TileBeamElemCollection::ID collID = coll->identify();  

    // find ROB
    uint32_t newrob = m_hid2re->getRobFromFragID(collID);
    if (newrob != robid[0]) {
      robid[0] = m_hid2re->getRobFromFragID(collID);
      robf.clear();
      m_RobSvc->getROBData(robid, robf);
    }
    
    // unpack ROB data
    if (robf.size() > 0 ) {
      m_decoder->fillCollection(robf[0],*coll);
    }
  }

  log << MSG::DEBUG<<" Creating Container " << *(pRE_Addr->par()) <<endreq; 
  pObj = StoreGateSvc::asStorable( m_container ) ; 
  return StatusCode::SUCCESS;  
}

StatusCode 
TileBeamElemContByteStreamCnv::createRep(DataObject* /* pObj */, IOpaqueAddress*& /* pAddr */)
{
  // No conversion from TileBeamElem to BS 

  MsgStream log(msgSvc(), "TileBeamElemContByteStreamCnv");

  log << MSG::ERROR <<" Can not create BS from TileBeamElem   "<<endreq; 

  return   StatusCode::FAILURE ; 


}

StatusCode
TileBeamElemContByteStreamCnv::finalize()
{
  MsgStream log(msgSvc(), "TileBeamElemContByteStreamCnv");
  log << MSG::DEBUG<<" Clearing Container " <<endreq; 

  // iterate over all collections in a container and clear them
  TileBeamElemContainer::const_iterator collItr=m_container->begin();
  TileBeamElemContainer::const_iterator lastColl=m_container->end();

  for(; collItr!=lastColl; ++collItr) {

    const TileBeamElemCollection * constColl = (*collItr);
    TileBeamElemCollection * coll = (TileBeamElemCollection *)constColl;
    coll->clear();
  }
  m_container->release(); 

  return Converter::finalize(); 

}
