/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    
  msgSvc()->setOutputLevel( "MM_RawDataContainerCnv", MSG::DEBUG );

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
    m_TPConverter.initialize(idHelper); 
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the ID helper and passed to the TP convertor." << endmsg;
  }

  return StatusCode::SUCCESS;
}

MM_RawDataContainer_PERS*    MM_RawDataContainerCnv::createPersistent (Muon::MM_RawDataContainer* transCont) {
    MsgStream log(msgSvc(), "MM_RawDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    return m_TPConverter.createPersistent( transCont, log );;
}

Muon::MM_RawDataContainer*
MM_RawDataContainerCnv::createTransient()
{
  using namespace Muon;
  MsgStream log(msgSvc(), "MM_RawDataContainerCnv" );
 
  MM_RawDataContainer *transCont = nullptr;
  static pool::Guid	p1_guid("5F202045-CE2C-4AD4-96BA-7DA18053B90F");

  if( compareClassGuid(p1_guid) ) {
    std::unique_ptr< MM_RawDataContainer_p1 >  cont( this->poolReadObject<MM_RawDataContainer_p1>() );
    const MM_RawDataContainer_p1* constCont = cont.get();
    transCont =  m_TPConverter.createTransient( constCont, log );    
  } else {
    throw std::runtime_error("Unsupported persistent version of MM Raw Data (RDO) container");
  }
  return transCont;
}