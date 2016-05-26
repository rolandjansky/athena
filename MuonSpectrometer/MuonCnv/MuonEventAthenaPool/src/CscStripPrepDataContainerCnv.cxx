/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscStripPrepDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
// #include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
// #include "MuonReadoutGeometry/MuonDetectorManager.h"
// #include "MuonReadoutGeometry/CscReadoutElement.h"

CscStripPrepDataContainerCnv::CscStripPrepDataContainerCnv(ISvcLocator* svcloc) :
CscStripPrepDataContainerCnvBase(svcloc),
    // Must create DataVector that does NOT own elements
    //m_prdCollVec(CscStripPrepDataCollVec(SG::VIEW_ELEMENTS)),
    m_storeGate(0)
    //m_cscId(0) 
{
}

CscStripPrepDataContainerCnv::~CscStripPrepDataContainerCnv() {
}

StatusCode CscStripPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !CscStripPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;

//    messageService()->setOutputLevel( "CscStripPrepDataContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(messageService(), "CscStripPrepDataContainerCnv");
    log << MSG::INFO << "CscStripPrepDataContainerCnv::initialize()" << endreq;

   // get StoreGate service
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::FATAL << "StoreGate service not found !" << endreq;
        return StatusCode::FAILURE;
    }

   // get DetectorStore service
    StoreGateSvc *detStore;
    sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endreq;
        return StatusCode::FAILURE;
    } else {
        log << MSG::DEBUG << "Found DetectorStore." << endreq;
    }


    //m_converter_p0.initialize(log);

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;

    return StatusCode::SUCCESS;
}

CscStripPrepDataContainer_PERS*    CscStripPrepDataContainerCnv::createPersistent (Muon::CscStripPrepDataContainer* transCont) {
    MsgStream log(messageService(), "CscStripPrepDataContainerCnv" );
    log<<MSG::DEBUG<<"createPersistent(): main converter"<<endreq;
    CscStripPrepDataContainer_PERS *pixdc_p= m_TPConverter.createPersistent( transCont, log );
    return pixdc_p;
}

Muon::CscStripPrepDataContainer* CscStripPrepDataContainerCnv::createTransient() {
    MsgStream log(messageService(), "CscStripPrepDataContainerCnv" );
    static pool::Guid   p0_guid("A41C9D99-F977-43B5-8DFC-819F057A9136"); // before t/p split
    static pool::Guid   p1_guid("6075244C-C6BB-4E24-B711-E7E4ED0F7462"); // with CscStripPrepData_tlp1
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endreq;
    Muon::CscStripPrepDataContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endreq;
        usingTPCnvForReading( m_TPConverter );
        std::unique_ptr< CscStripPrepDataContainer_PERS >  p_coll( poolReadObject< CscStripPrepDataContainer_PERS >() );
        p_collection = m_TPConverter.createTransient( p_coll.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        throw std::runtime_error("Not currently supporting reading non TP-split PRDs");
        //std::unique_ptr< CscStripPrepDataContainer_p0 >   col_vect( poolReadObject< CscStripPrepDataContainer_p0 >() );
        //p_collection = m_converter_p0.createTransient( col_vect.get(), log );
    }
    else {
        throw std::runtime_error("Unsupported persistent version of CscStripPrepDataContainer");

    }
    return p_collection;
}
