/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "sTgcPrepDataContainerCnv.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

sTgcPrepDataContainerCnv::sTgcPrepDataContainerCnv(ISvcLocator* svcloc) :
sTgcPrepDataContainerCnvBase(svcloc)
{
}

sTgcPrepDataContainerCnv::~sTgcPrepDataContainerCnv() {
}

StatusCode sTgcPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !sTgcPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "sTgcPrepDataContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "sTgcPrepDataContainerCnv::initialize()" << endmsg;

    return StatusCode::SUCCESS;
}

sTgcPrepDataContainer_PERS*    sTgcPrepDataContainerCnv::createPersistent (Muon::sTgcPrepDataContainer* transCont) {
    MsgStream log(msgSvc(), "sTgcPrepDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    sTgcPrepDataContainer_PERS *pers= m_converter_p1.createPersistent( transCont, log );
    return pers;
}

Muon::sTgcPrepDataContainer* sTgcPrepDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "sTgcPrepDataContainerCnv" );
    static pool::Guid   p1_guid("7AB87DDE-8D7C-11E2-AA7C-001517648C14"); 
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::sTgcPrepDataContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        std::unique_ptr< Muon::sTgcPrepDataContainer_p1 >  p_coll( poolReadObject< Muon::sTgcPrepDataContainer_p1 >() );
        p_collection = m_converter_p1.createTransient( p_coll.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of sTgcPrepDataContainer");
    }
    return p_collection;
}
