/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcPrepDataContainerCnv.h"

// Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// Id includes
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

TgcPrepDataContainerCnv::TgcPrepDataContainerCnv(ISvcLocator* svcloc) :
TgcPrepDataContainerCnvBase(svcloc)
{
}

TgcPrepDataContainerCnv::~TgcPrepDataContainerCnv() {
}

StatusCode TgcPrepDataContainerCnv::initialize() {
   // Call base clase initialize
    if( !TgcPrepDataContainerCnvBase::initialize().isSuccess() )
       return StatusCode::FAILURE;

//    msgSvc()->setOutputLevel( "TgcPrepDataContainerCnv", MSG::DEBUG );

   // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "TgcPrepDataContainerCnv");
    if (log.level() <= MSG::INFO) log << MSG::INFO << "TgcPrepDataContainerCnv::initialize()" << endmsg;

    return StatusCode::SUCCESS;
}

TgcPrepDataContainer_PERS*    TgcPrepDataContainerCnv::createPersistent (Muon::TgcPrepDataContainer* transCont) {
    MsgStream log(msgSvc(), "TgcPrepDataContainerCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    TgcPrepDataContainer_PERS *pers= m_converter_p3.createPersistent( transCont, log );
    return pers;
}

Muon::TgcPrepDataContainer* TgcPrepDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "TgcPrepDataContainerCnv" );
    static pool::Guid   p0_guid("04E7158E-416B-8E31-D370-616631B8617E"); // before t/p split
    static pool::Guid   p1_guid("8C563637-620B-43A0-9A5D-E0BC09496745"); // with TgcPrepData_tlp1
    static pool::Guid   p2_guid("EBB4DCEC-CE23-4A30-BEF7-F3150E4060CB"); // with TgcPrepDataContainerCnv_p2
    static pool::Guid   p3_guid("215726D6-15F4-41D2-B808-1B9E9FD84166"); // with TgcPrepDataContainerCnv_p3

    
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    Muon::TgcPrepDataContainer* p_collection(nullptr);
    if( compareClassGuid(p3_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 3 detected"<<endmsg;
        std::unique_ptr< Muon::TgcPrepDataContainer_p3 >  p_coll( poolReadObject< Muon::TgcPrepDataContainer_p3 >() );
        p_collection = m_converter_p3.createTransient( p_coll.get(), log );
    } else if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        std::unique_ptr< Muon::TgcPrepDataContainer_p2 >  p_coll( poolReadObject< Muon::TgcPrepDataContainer_p2 >() );
        p_collection = m_converter_p2.createTransient( p_coll.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
      TgcPrepDataContainerCnv_tlp1 tpConvertor_p1;
      std::unique_ptr< Muon::TgcPrepDataContainer_tlp1 > col_vect( poolReadObject< Muon::TgcPrepDataContainer_tlp1 >() );
      p_collection = m_TPConverter.createTransient( col_vect.get(), log );
      if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        throw std::runtime_error("Not currently supporting reading non TP-split PRDs");
        //std::unique_ptr< TgcPrepDataContainer_p0 >   col_vect( poolReadObject< TgcPrepDataContainer_p0 >() );
        //p_collection = m_converter_p0.createTransient( col_vect.get(), log );
    }
    else {
        throw std::runtime_error("Unsupported persistent version of TgcPrepDataContainer");

    }
    return p_collection;
}
