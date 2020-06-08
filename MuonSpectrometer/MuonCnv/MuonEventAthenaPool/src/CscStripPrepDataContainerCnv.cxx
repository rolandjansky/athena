/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscStripPrepDataContainerCnv.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"

CscStripPrepDataContainerCnv::CscStripPrepDataContainerCnv(ISvcLocator* svcloc) :
CscStripPrepDataContainerCnvBase(svcloc),
    // Must create DataVector that does NOT own elements
    m_storeGate(0)
{
}

CscStripPrepDataContainerCnv::~CscStripPrepDataContainerCnv() {
}

StatusCode CscStripPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !CscStripPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "CscStripPrepDataContainerCnv");
    log << MSG::INFO << "CscStripPrepDataContainerCnv::initialize()" << endmsg;

   // get StoreGate service
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::FATAL << "StoreGate service not found !" << endmsg;
        return StatusCode::FAILURE;
    }

   // get DetectorStore service
    StoreGateSvc *detStore;
    sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;

    return StatusCode::SUCCESS;
}

CscStripPrepDataContainer_PERS*    CscStripPrepDataContainerCnv::createPersistent (Muon::CscStripPrepDataContainer* transCont) {
    MsgStream log(msgSvc(), "CscStripPrepDataContainerCnv" );
    log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    CscStripPrepDataContainer_PERS *pixdc_p= m_TPConverter.createPersistent( transCont, log );
    return pixdc_p;
}

Muon::CscStripPrepDataContainer* CscStripPrepDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "CscStripPrepDataContainerCnv" );
    static pool::Guid   p0_guid("A41C9D99-F977-43B5-8DFC-819F057A9136"); // before t/p split
    static pool::Guid   p1_guid("6075244C-C6BB-4E24-B711-E7E4ED0F7462"); // with CscStripPrepData_tlp1
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::CscStripPrepDataContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        std::unique_ptr< CscStripPrepDataContainer_PERS >  p_coll( poolReadObject< CscStripPrepDataContainer_PERS >() );
        p_collection = m_TPConverter.createTransient( p_coll.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        throw std::runtime_error("Not currently supporting reading non TP-split PRDs");
    }
    else {
        throw std::runtime_error("Unsupported persistent version of CscStripPrepDataContainer");

    }
    return p_collection;
}
