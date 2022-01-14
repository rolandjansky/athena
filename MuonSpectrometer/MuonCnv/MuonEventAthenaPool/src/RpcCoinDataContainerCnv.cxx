/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcCoinDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

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
    
    //msgSvc()->setOutputLevel( "RpcCoinDataContainerCnv", MSG::INFO );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "RpcCoinDataContainerCnv");
    if (log.level() <= MSG::DEBUG) log << MSG::INFO << "RpcCoinDataContainerCnv::initialize()" << endmsg;

    return StatusCode::SUCCESS;
}

Muon::RpcCoinDataContainerCnv_p1::PERS*    RpcCoinDataContainerCnv::createPersistent (Muon::RpcCoinDataContainer* transCont) {
    MsgStream log(msgSvc(), "RpcCoinDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    Muon::RpcCoinDataContainerCnv_p1::PERS *rpc_p= m_converter_p1.createPersistent( transCont, log );
    return rpc_p;
}

Muon::RpcCoinDataContainer* RpcCoinDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "RpcCoinDataContainerCnv" );
    static pool::Guid   p1_guid("AF0DB103-E825-45E5-9C29-9C32342756DD"); 
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::RpcCoinDataContainer* p_collection(nullptr);
    if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        std::unique_ptr< Muon::RpcCoinDataContainerCnv_p1::PERS >   col_vect( poolReadObject< Muon::RpcCoinDataContainerCnv_p1::PERS >() );
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Delegate TP converter " << endmsg;
        p_collection = m_converter_p1.createTransient( col_vect.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of RpcCoinDataContainer");
    }
    return p_collection;
}
