/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MmDigitContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/MmDigitContainer.h"

MmDigitContainerCnv::MmDigitContainerCnv(ISvcLocator* svcloc) :
MmDigitContainerCnvBase(svcloc)
{
}

MmDigitContainerCnv::~MmDigitContainerCnv() {
}

StatusCode MmDigitContainerCnv::initialize() {
   // Call base clase initialize
    if( !MmDigitContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    // messageService()->setOutputLevel( "MmDigitContainerCnv", MSG::DEBUG );

    std::cout<<"MmDigitContainerCnv:initialize"<<std::endl;
   // Get the messaging service, print where you are
    MsgStream log(messageService(), "MmDigitContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "MmDigitContainerCnv::initialize()" << endreq;

    return StatusCode::SUCCESS;
}

MM_DigitContainer_PERS*    MmDigitContainerCnv::createPersistent (MmDigitContainer* transCont) {
    MsgStream log(messageService(), "MmDigitContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endreq;
    MM_DigitContainer_PERS *pers= m_converter.createPersistent( transCont, log );
    return pers;
}

MmDigitContainer* MmDigitContainerCnv::createTransient() {
    MsgStream log(messageService(), "MmDigitContainerCnv" );
    static pool::Guid   p1_guid("444DEE03-9A1D-44BA-8880-968C78EA51D1"); 
    log<<MSG::INFO<<"createTransient(): main converter"<<endreq;
    MmDigitContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endreq;
        std::unique_ptr< Muon::MM_DigitContainer_p1 >  p_coll( poolReadObject< Muon::MM_DigitContainer_p1 >() );
        p_collection = m_converter.createTransient( p_coll.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of MM_DigitContainer");
    }
    return p_collection;
}
