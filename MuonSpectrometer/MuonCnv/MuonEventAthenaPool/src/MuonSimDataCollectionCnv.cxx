/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimDataCollectionCnv.h"

// Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollection_p1.h"

MuonSimDataCollectionCnv::MuonSimDataCollectionCnv(ISvcLocator* svcloc) :
    MuonSimDataCollectionCnvBase(svcloc)
{
}

MuonSimDataCollectionCnv::~MuonSimDataCollectionCnv() {
}

MuonSimDataCollection_PERS*    MuonSimDataCollectionCnv::createPersistent (MuonSimDataCollection* transCont) {
    MsgStream log(msgSvc(), "MuonSimDataCollectionCnv" );
    ATH_MSG_DEBUG("createPersistent(): main converter");
    MuonSimDataCollection_PERS *pixdc_p= m_TPConverter_p2.createPersistent( transCont, log );
    return pixdc_p;
}

MuonSimDataCollection* MuonSimDataCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "MuonSimDataCollectionCnv" );
    static pool::Guid   p0_guid("5B50C32E-A036-4B49-AC97-716E53210BE2");
    static pool::Guid   p1_guid("0605B4A3-3744-4486-B39D-F9C9E809D868");
    static pool::Guid   p2_guid("E0AA3013-4EF7-45B6-BDB1-17B21BF60791");
    ATH_MSG_DEBUG("createTransient(): main converter");
    MuonSimDataCollection* p_collection(0);
    if( compareClassGuid(p2_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
      std::unique_ptr< Muon::MuonSimDataCollection_p2 >   col_vect( this->poolReadObject< Muon::MuonSimDataCollection_p2 >() );
      p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p1_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
      std::unique_ptr< Muon::MuonSimDataCollection_p1 >   col_vect( this->poolReadObject< Muon::MuonSimDataCollection_p1 >() );
      p_collection = m_TPConverter_p1.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ){
       if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
       std::unique_ptr< MuonSimDataCollection >   col_vect( poolReadObject< MuonSimDataCollection >() );
       p_collection = col_vect.release();
    }
  //----------------------------------------------------------------
    else {
        throw std::runtime_error("Unsupported persistent version of MuonSimDataCollection");

    }
    return p_collection;
}
