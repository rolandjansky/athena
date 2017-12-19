/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscSimDataCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimData/CscSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollection_p1.h"

CscSimDataCollectionCnv::CscSimDataCollectionCnv(ISvcLocator* svcloc) :
    CscSimDataCollectionCnvBase(svcloc)
{
}

CscSimDataCollectionCnv::~CscSimDataCollectionCnv() {
}

CscSimDataCollection_PERS*    CscSimDataCollectionCnv::createPersistent (CscSimDataCollection* transCont) {
    MsgStream log(msgSvc(), "CscSimDataCollectionCnv" );
    ATH_MSG_DEBUG("createPersistent(): main converter");
    CscSimDataCollection_PERS *pixdc_p= m_TPConverter_p1.createPersistent( transCont, log );
    return pixdc_p;
}

CscSimDataCollection* CscSimDataCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "CscSimDataCollectionCnv" );
    static pool::Guid   p0_guid("250EC949-F98B-4F74-9034-178847D1B622");
    static pool::Guid   p1_guid("DD2A8397-4435-4DA2-AD14-ADD7294694B2");
    static pool::Guid   p2_guid("023993E1-BAAA-4F36-8CD4-8F03E3983E8D");
    ATH_MSG_DEBUG("createTransient(): main converter");
    CscSimDataCollection* p_collection(0);
    if( compareClassGuid(p2_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
      std::unique_ptr< Muon::CscSimDataCollection_p2 >   col_vect( this->poolReadObject< Muon::CscSimDataCollection_p2 >() );
      p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p1_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
      std::unique_ptr< Muon::CscSimDataCollection_p1 >   col_vect( this->poolReadObject< Muon::CscSimDataCollection_p1 >() );
      p_collection = m_TPConverter_p1.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ){
       if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
       std::unique_ptr< CscSimDataCollection >   col_vect( poolReadObject< CscSimDataCollection >() );
       p_collection = col_vect.release();
    }
  //----------------------------------------------------------------
    else {
        throw std::runtime_error("Unsupported persistent version of CscSimDataCollection");

    }
    return p_collection;
}
