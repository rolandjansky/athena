/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtPrepDataContainerCnv.h"

#include "MdtPrepDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonEventAthenaPool/CreateTransientTemplates.h"


MdtPrepDataContainerCnv::MdtPrepDataContainerCnv(ISvcLocator* svcloc) :
MdtPrepDataContainerCnvBase(svcloc)
{
}

MdtPrepDataContainerCnv::~MdtPrepDataContainerCnv() {
}

StatusCode MdtPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !MdtPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    // msgSvc()->setOutputLevel( "MdtPrepDataContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "MdtPrepDataContainerCnv");
    log << MSG::INFO << "MdtPrepDataContainerCnv::initialize()" << endmsg;
    return StatusCode::SUCCESS;
}

MdtPrepDataContainer_PERS*    MdtPrepDataContainerCnv::createPersistent (Muon::MdtPrepDataContainer* transCont) {
    MsgStream log(msgSvc(), "MdtPrepDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    MdtPrepDataContainer_PERS *pixdc_p= m_converter_p2.createPersistent( transCont, log );
    return pixdc_p;
}

Muon::MdtPrepDataContainer* MdtPrepDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "MdtPrepDataContainerCnv" );
    static pool::Guid   p0_guid("7479AE0B-3E99-4344-E6A5-3DDF78E1A363"); // before t/p split
    static pool::Guid   p1_guid("7E3F92F5-FAFF-45C3-A8F5-386B3CD56A7B"); // with MdtPrepDataContainerCnv_tlp1
    static pool::Guid   p2_guid("BBC02355-570A-4C28-81A6-65F23779509E"); // with MdtPrepDataContainerCnv_p2
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::MdtPrepDataContainer* p_collection(0);
    if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        std::unique_ptr< MdtPrepDataContainer_PERS >   col_vect( poolReadObject< MdtPrepDataContainer_PERS >() );
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Delegate TP converter " << endmsg;
        p_collection = m_converter_p2.createTransient( col_vect.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        poolReadObject< MdtPrepDataContainer_PERS >( m_converter_tlp1 );
        p_collection = m_converter_tlp1.createTransient( log );
    }
    else {
        throw std::runtime_error("Unsupported persistent version of MdtPrepDataContainer");
    }
    return p_collection;
}
