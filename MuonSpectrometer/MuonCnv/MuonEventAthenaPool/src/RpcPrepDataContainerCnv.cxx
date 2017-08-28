/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcPrepDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonEventAthenaPool/CreateTransientTemplates.h"


RpcPrepDataContainerCnv::RpcPrepDataContainerCnv(ISvcLocator* svcloc) :
RpcPrepDataContainerCnvBase(svcloc)
{}

RpcPrepDataContainerCnv::~RpcPrepDataContainerCnv() {
}

StatusCode RpcPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !RpcPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    // msgSvc()->setOutputLevel( "RpcPrepDataContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "RpcPrepDataContainerCnv");
    log << MSG::INFO << "RpcPrepDataContainerCnv::initialize()" << endmsg;

    return StatusCode::SUCCESS;
}

RpcPrepDataContainer_PERS*    RpcPrepDataContainerCnv::createPersistent (Muon::RpcPrepDataContainer* transCont) {
    MsgStream log(msgSvc(), "RpcPrepDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    RpcPrepDataContainer_PERS *pers= m_converter_p3.createPersistent( transCont, log );
    return pers;
}

Muon::RpcPrepDataContainer* RpcPrepDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "RpcPrepDataContainerCnv" );
    static pool::Guid   p0_guid("CCD4055C-2A39-CAFF-3F82-5A9116ADAFCB"); // before t/p split
    static pool::Guid   p1_guid("F29233F8-F488-4D6D-97F9-4BCA1976BD9F"); // with RpcPrepData_tlp1
    // static pool::Guid   p2_guid("F29233F8-F488-4D6D-97F9-4BCA1976BD9F"); // with RpcPrepData_tlp2
    static pool::Guid   p3_guid("65496D1F-E660-42A9-84C9-2002D39B87CB"); // with RpcPrepDataContainer_p3
    
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::RpcPrepDataContainer* p_collection(0);
    if( compareClassGuid(p3_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 3 detected"<<endmsg;
        std::unique_ptr< Muon::RpcPrepDataContainer_p3 >  p_coll( poolReadObject< Muon::RpcPrepDataContainer_p3 >() );
        p_collection = m_converter_p3.createTransient( p_coll.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
      RpcPrepDataContainerCnv_tlp1 tpConvertor_p1;
      std::unique_ptr< Muon::RpcPrepDataContainer_tlp1 > col_vect( poolReadObject< Muon::RpcPrepDataContainer_tlp1 >() );
      p_collection = m_TPConverter.createTransient( col_vect.get(), log );
      if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        throw std::runtime_error("Not supporting reading non TP-split PRDs");
    }
    else {
        throw std::runtime_error("Unsupported persistent version of RpcPrepDataContainer");

    }
    return p_collection;
}
