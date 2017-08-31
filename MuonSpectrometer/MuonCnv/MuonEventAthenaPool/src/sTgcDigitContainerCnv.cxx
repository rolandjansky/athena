/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTgcDigitContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonEventAthenaPool/CreateTransientTemplates.h"


sTgcDigitContainerCnv::sTgcDigitContainerCnv(ISvcLocator* svcloc) :
sTgcDigitContainerCnvBase(svcloc)
{
  std::cout<<"sTgcDigitContainerCnv ctor"<<std::endl;
}

sTgcDigitContainerCnv::~sTgcDigitContainerCnv() {
}

StatusCode sTgcDigitContainerCnv::initialize() {
   // Call base clase initialize
    if( !sTgcDigitContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    // msgSvc()->setOutputLevel( "sTgcDigitContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "sTgcDigitContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "sTgcDigitContainerCnv::initialize()" << endmsg;

    return StatusCode::SUCCESS;
}

STGC_DigitContainer_PERS*    sTgcDigitContainerCnv::createPersistent (sTgcDigitContainer* transCont) {
    MsgStream log(msgSvc(), "sTgcDigitContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    STGC_DigitContainer_PERS *pers= m_converter.createPersistent( transCont, log );
    return pers;
}

sTgcDigitContainer* sTgcDigitContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "sTgcDigitContainerCnv" );
    static pool::Guid   p1_guid("F3581998-EC4F-4F34-A153-9F3415C4087A"); 
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    sTgcDigitContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        std::unique_ptr< Muon::STGC_DigitContainer_p1 >  p_coll( poolReadObject< Muon::STGC_DigitContainer_p1 >() );
        p_collection = m_converter.createTransient( p_coll.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of STGC_DigitContainer");
    }
    return p_collection;
}
