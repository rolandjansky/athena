/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_RawDataContainerCnv.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MmIdHelper.h"


MM_RawDataContainerCnv::MM_RawDataContainerCnv(ISvcLocator* svcloc) :
  MM_RawDataContainerCnvBase(svcloc)
{
}

MM_RawDataContainerCnv::~MM_RawDataContainerCnv() {
}

StatusCode MM_RawDataContainerCnv::initialize() {
  // Call base clase initialize
  if( !MM_RawDataContainerCnvBase::initialize().isSuccess() )
    return StatusCode::FAILURE;  
    
  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "MM_RawDataContainerCnv");
  if (log.level() <= MSG::INFO) log << MSG::INFO << "MM_RawDataContainerCnv::initialize()" << endmsg;
  // get DetectorStore service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  
  StoreGateSvc *detStore;
  StatusCode sc = svcLocator->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
  }   
   
  // Get the helper from the detector store
  const MmIdHelper *idHelper;
  sc = detStore->retrieve(idHelper);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_TPConverter_p1.initialize(idHelper); 
    m_TPConverter_p2.initialize(idHelper); 
    m_TPConverter_p3.initialize(idHelper); 
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the ID helper and passed to the TP convertor." << endmsg;
  }

  return StatusCode::SUCCESS;
}

MM_RawDataContainer_PERS*    MM_RawDataContainerCnv::createPersistent (Muon::MM_RawDataContainer* transCont) {
    MsgStream log(msgSvc(), "MM_RawDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    return m_TPConverter_p3.createPersistent( transCont, log );;
}

Muon::MM_RawDataContainer*
MM_RawDataContainerCnv::createTransient()
{
  using namespace Muon;
  MsgStream log(msgSvc(), "MM_RawDataContainerCnv" );
 
  MM_RawDataContainer *transCont = nullptr;
  static const pool::Guid	p1_guid("5F202045-CE2C-4AD4-96BA-7DA18053B90F");
  static const pool::Guid	p2_guid("A49EBDAC-A190-4198-95DF-BF75FBBB487F");
  static const pool::Guid	p3_guid("229DDB7E-59D3-4BE5-B3D5-B873EBC5C9AA");

  if( compareClassGuid(p1_guid) ) {
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<std::endl;
    std::unique_ptr< MM_RawDataContainer_p1 >  cont( this->poolReadObject<MM_RawDataContainer_p1>() );
    const MM_RawDataContainer_p1* constCont = cont.get();
    transCont =  m_TPConverter_p1.createTransient( constCont, log );    
  }
  // ----------------------------------------------
  // p2 has the relBCID included
  else  if( compareClassGuid(p2_guid) ) {
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<std::endl;
    std::unique_ptr< MM_RawDataContainer_p2 >  cont( this->poolReadObject<MM_RawDataContainer_p2>() );
    const MM_RawDataContainer_p2* constCont = cont.get();
    transCont =  m_TPConverter_p2.createTransient( constCont, log );    
  } 
  // ----------------------------------------------
  // p3 has timeAndChargeInCounts switch included
  else  if( compareClassGuid(p3_guid) ) {
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 3 detected"<<std::endl;
    std::unique_ptr< MM_RawDataContainer_p3 >  cont( this->poolReadObject<MM_RawDataContainer_p3>() );
    const MM_RawDataContainer_p3* constCont = cont.get();
    transCont =  m_TPConverter_p3.createTransient( constCont, log );    
  } 
  // ---------------------------------------------- 
  else {
    throw std::runtime_error("Unsupported persistent version of MM Raw Data (RDO) container");
  }
  return transCont;
}
