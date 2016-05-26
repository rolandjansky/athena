/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MMPrepDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
// #include "MuonIdHelpers/MMIdHelper.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"

// #include "MuonReadoutGeometry/MuonDetectorManager.h"
// #include "MuonReadoutGeometry/MMReadoutElement.h"

MMPrepDataContainerCnv::MMPrepDataContainerCnv(ISvcLocator* svcloc) :
MMPrepDataContainerCnvBase(svcloc)
{
}

MMPrepDataContainerCnv::~MMPrepDataContainerCnv() {
}

StatusCode MMPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !MMPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    messageService()->setOutputLevel( "MMPrepDataContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(messageService(), "MMPrepDataContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "MMPrepDataContainerCnv::initialize()" << endreq;

    return StatusCode::SUCCESS;
}

MMPrepDataContainer_PERS*    MMPrepDataContainerCnv::createPersistent (Muon::MMPrepDataContainer* transCont) {
    MsgStream log(messageService(), "MMPrepDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endreq;
    MMPrepDataContainer_PERS *pers= m_converter_p1.createPersistent( transCont, log );
    // COMPRESS MMPrepDataContainer_PERS *pers= m_converter_p1.createPersistent( transCont, log );
    return pers;
}

Muon::MMPrepDataContainer* MMPrepDataContainerCnv::createTransient() {
    MsgStream log(messageService(), "MMPrepDataContainerCnv" );
    static pool::Guid   p1_guid("5E5C97E2-8D7C-11E2-9FCB-001517648C14"); 
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endreq;
    Muon::MMPrepDataContainer* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endreq;
        std::unique_ptr< Muon::MMPrepDataContainer_p1 >  p_coll( poolReadObject< Muon::MMPrepDataContainer_p1 >() );
        p_collection = m_converter_p1.createTransient( p_coll.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of MMPrepDataContainer.");
 
    }
    return p_collection;
}
