/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscPrepDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
// #include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonEventTPCnv/CscPrepDataContainerCnv_tlp1.h"
#include "MuonEventAthenaPool/CreateTransientTemplates.h"

// #include "MuonReadoutGeometry/MuonDetectorManager.h"
// #include "MuonReadoutGeometry/CscReadoutElement.h"

CscPrepDataContainerCnv::CscPrepDataContainerCnv(ISvcLocator* svcloc) :
CscPrepDataContainerCnvBase(svcloc)
{
}

CscPrepDataContainerCnv::~CscPrepDataContainerCnv() {
}

StatusCode CscPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !CscPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;
    
    // msgSvc()->setOutputLevel( "CscPrepDataContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "CscPrepDataContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "CscPrepDataContainerCnv::initialize()" << endmsg;

    return StatusCode::SUCCESS;
}

CscPrepDataContainer_PERS*    CscPrepDataContainerCnv::createPersistent (Muon::CscPrepDataContainer* transCont) {
    MsgStream log(msgSvc(), "CscPrepDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    CscPrepDataContainer_PERS *pers= m_TPConverter.createPersistent( transCont, log );
    // COMPRESS CscPrepDataContainer_PERS *pers= m_converter_p2.createPersistent( transCont, log );
    return pers;
}

Muon::CscPrepDataContainer* CscPrepDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "CscPrepDataContainerCnv" );
    static pool::Guid   p0_guid("C48250B1-7575-DFA1-1313-01AAAF759AEA"); // before t/p split
    static pool::Guid   p1_guid("B941657D-1ABF-4A88-B23C-6C4212CD04B3"); // with CscPrepData_tlp1
    static pool::Guid   p2_guid("BF5DA875-6D5B-4DCA-9CD8-E0ABC4FD92F5"); // with CscPrepDataContainer_p2
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::CscPrepDataContainer* p_collection(0);
    if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        std::unique_ptr< Muon::CscPrepDataContainer_p2 >  p_coll( poolReadObject< Muon::CscPrepDataContainer_p2 >() );
        p_collection = m_converter_p2.createTransient( p_coll.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
      CscPrepDataContainerCnv_tlp1 tpConvertor_p1;
      std::unique_ptr< Muon::CscPrepDataContainer_tlp1 > col_vect( poolReadObject< Muon::CscPrepDataContainer_tlp1 >() );
      p_collection = m_TPConverter.createTransient( col_vect.get(), log );
      if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        throw std::runtime_error("Not currently supporting reading non TP-split PRDs");
        //std::unique_ptr< CscPrepDataContainer_p0 >   col_vect( poolReadObject< CscPrepDataContainer_p0 >() );
        //p_collection = m_converter_p0.createTransient( col_vect.get(), log );
    }
    else {
        throw std::runtime_error("Unsupported persistent version of CscPrepDataContainer");

    }
    return p_collection;
}
