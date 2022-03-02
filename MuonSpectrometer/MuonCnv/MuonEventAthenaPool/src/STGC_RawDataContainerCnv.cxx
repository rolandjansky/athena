/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "STGC_RawDataContainerCnv.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

STGC_RawDataContainerCnv::STGC_RawDataContainerCnv(ISvcLocator* svcloc) :
  STGC_RawDataContainerCnvBase(svcloc)
{
}

STGC_RawDataContainerCnv::~STGC_RawDataContainerCnv() {
}

StatusCode STGC_RawDataContainerCnv::initialize() {
  // Call base clase initialize
  if( !STGC_RawDataContainerCnvBase::initialize().isSuccess() )
    return StatusCode::FAILURE;
    
  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "STGC_RawDataContainerCnv");
  if (log.level() <= MSG::INFO) log << MSG::INFO << "STGC_RawDataContainerCnv::initialize()" << endmsg;

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
  const sTgcIdHelper *idHelper;
  sc = detStore->retrieve(idHelper);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_TPConverter_p1.initialize(idHelper); 
    m_TPConverter_p2.initialize(idHelper); 
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the ID helper and passed to the TP convertor." << endmsg;
  }

  return StatusCode::SUCCESS;
}

STGC_RawDataContainer_PERS*    STGC_RawDataContainerCnv::createPersistent (Muon::STGC_RawDataContainer* transCont) {
    MsgStream log(msgSvc(), "STGC_RawDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    return m_TPConverter_p2.createPersistent( transCont, log );
}

Muon::STGC_RawDataContainer*
STGC_RawDataContainerCnv::createTransient()
{
  using namespace Muon;
  MsgStream log(msgSvc(), "STGC_RawDataContainerCnv" );
 
  STGC_RawDataContainer *transCont = nullptr;
  static pool::Guid	p2_guid("F66FDF31-1BFD-43DE-B793-93635D98597E");
  static pool::Guid	p1_guid("E9229710-DB8A-447E-9546-4BAB079C7547");

  if( compareClassGuid(p2_guid) ) {
    std::unique_ptr< STGC_RawDataContainer_p2 >  cont( this->poolReadObject<STGC_RawDataContainer_p2>() );
    const STGC_RawDataContainer_p2* constCont = cont.get();
    transCont =  m_TPConverter_p2.createTransient( constCont, log );
    
  } else if( compareClassGuid(p1_guid) ) {
    std::unique_ptr< STGC_RawDataContainer_p1 >  cont( this->poolReadObject<STGC_RawDataContainer_p1>() );
    const STGC_RawDataContainer_p1* constCont = cont.get();
    transCont =  m_TPConverter_p1.createTransient( constCont, log );
    // virtual Muon::STGC_RawDataContainer* createTransient(const Muon::STGC_RawDataContainer_p1* persObj, MsgStream& log) override final;
    
  } else {
    throw std::runtime_error("Unsupported persistent version of STGC Raw Data (RDO) container");
  }
  return transCont;
}
