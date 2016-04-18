/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcCoinDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

RpcCoinDataContainerCnv::RpcCoinDataContainerCnv(ISvcLocator* svcloc) :
RpcCoinDataContainerCnvBase(svcloc)
{}

RpcCoinDataContainerCnv::~RpcCoinDataContainerCnv() {
}

StatusCode RpcCoinDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !RpcCoinDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    //messageService()->setOutputLevel( "RpcCoinDataContainerCnv", MSG::INFO );

   // Get the messaging service, print where you are
    MsgStream log(messageService(), "RpcCoinDataContainerCnv");
    if (log.level() <= MSG::DEBUG) log << MSG::INFO << "RpcCoinDataContainerCnv::initialize()" << endreq;

    return StatusCode::SUCCESS;
}

Muon::RpcCoinDataContainerCnv_p1::PERS*    RpcCoinDataContainerCnv::createPersistent (Muon::RpcCoinDataContainer* transCont) {
    MsgStream log(messageService(), "RpcCoinDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endreq;
    Muon::RpcCoinDataContainerCnv_p1::PERS *rpc_p= m_converter_p1.createPersistent( transCont, log );
    return rpc_p;
}

Muon::RpcCoinDataContainer* RpcCoinDataContainerCnv::createTransient() {
    MsgStream log(messageService(), "RpcCoinDataContainerCnv" );
    static pool::Guid   p1_guid("AF0DB103-E825-45E5-9C29-9C32342756DD"); 
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endreq;
    Muon::RpcCoinDataContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endreq;
        std::unique_ptr< Muon::RpcCoinDataContainerCnv_p1::PERS >   col_vect( poolReadObject< Muon::RpcCoinDataContainerCnv_p1::PERS >() );
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Delegate TP converter " << endreq;
        p_collection = m_converter_p1.createTransient( col_vect.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of RpcCoinDataContainer");
    }
    return p_collection;
}
