/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#include "TgcCoinDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonEventAthenaPool/CreateTransientTemplates.h"


TgcCoinDataContainerCnv::TgcCoinDataContainerCnv(ISvcLocator* svcloc) :
TgcCoinDataContainerCnvBase(svcloc),
    // Must create DataVector that does NOT own elements
    //m_prdCollVec(TgcCoinDataCollVec(SG::VIEW_ELEMENTS)),
    m_storeGate(0)
    //m_cscId(0) 
{
}

TgcCoinDataContainerCnv::~TgcCoinDataContainerCnv() {
}

StatusCode TgcCoinDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !TgcCoinDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;

//    msgSvc()->setOutputLevel( "TgcCoinDataContainerCnv", MSG::INFO );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "TgcCoinDataContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "TgcCoinDataContainerCnv::initialize()" << endmsg;

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
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }


    //m_converter_p0.initialize(log);

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;

    return StatusCode::SUCCESS;
}

TgcCoinDataContainer_PERS*    TgcCoinDataContainerCnv::createPersistent (Muon::TgcCoinDataContainer* transCont) {
    MsgStream log(msgSvc(), "TgcCoinDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    TgcCoinDataContainer_PERS *pixdc_p= m_TPConverter_tlp3.createPersistent( transCont, log );
    return pixdc_p;
}

Muon::TgcCoinDataContainer* TgcCoinDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "TgcCoinDataContainerCnv" );
    static pool::Guid   p0_guid("F81C4564-B1C5-4053-A6F6-E0ED77907BE5"); // before t/p split
    static pool::Guid   p1_guid("C312D3F5-60DB-41D5-895B-9FD4EF443E0B"); // with TgcCoinData_tlp1
    static pool::Guid   p2_guid("524775D8-A66F-4AD3-912E-7D05389C1011"); // with TgcCoinData_tlp2
    static pool::Guid   p3_guid("95BF89C7-1FFC-464F-A14D-742F9E874E56"); // with TgcCoinData_tlp3
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::TgcCoinDataContainer* p_collection(0);
    if( compareClassGuid(p3_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 3 detected"<<endmsg;
        poolReadObject< TgcCoinDataContainer_PERS >( m_TPConverter_tlp3 );
        p_collection = m_TPConverter_tlp3.createTransient( log );
    }
    else if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        poolReadObject< TgcCoinDataContainer_PERS >( m_TPConverter_tlp2 );
        p_collection = m_TPConverter_tlp2.createTransient( log );
    }
    else if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        poolReadObject< Muon::TgcCoinDataContainer_tlp1 >( m_TPConverter_tlp1 );
        p_collection = m_TPConverter_tlp1.createTransient( log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        throw std::runtime_error("Not currently supporting reading non TP-split CoinDatas");
        //std::unique_ptr< TgcCoinDataContainer_p0 >   col_vect( poolReadObject< TgcCoinDataContainer_p0 >() );
        //p_collection = m_converter_p0.createTransient( col_vect.get(), log );
    }
    else {
        throw std::runtime_error("Unsupported persistent version of TgcCoinDataContainer");

    }
    return p_collection;
}
