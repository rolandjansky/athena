/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MMSimHitCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimEvent/MMSimHitCollection.h"
#include "MuonSimEventTPCnv/MMSimHitCollection_p1.h"
#include "HitManagement/AthenaHitsVector.h" //for back-compatibility


MMSimHitCollectionCnv::MMSimHitCollectionCnv(ISvcLocator* svcloc) :
    MMSimHitCollectionCnvBase(svcloc)
{
}

MMSimHitCollectionCnv::~MMSimHitCollectionCnv() {
}

MMSimHitCollection_PERS*    MMSimHitCollectionCnv::createPersistent (MMSimHitCollection* transCont) {
    MsgStream log(msgSvc(), "MMSimHitCollectionCnv" );
    ATH_MSG_DEBUG("createPersistent(): main converter");
    MMSimHitCollection_PERS *pixdc_p= m_TPConverter_p2.createPersistent( transCont, log );
    return pixdc_p;
}

MMSimHitCollection* MMSimHitCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "MMSimHitCollectionCnv" );
    static pool::Guid   p1_guid("ac0b677c-fe08-11e8-b174-02163e018187");
    static pool::Guid   p2_guid("b9bdd436-fe08-11e8-a40f-02163e018187");
    ATH_MSG_DEBUG("createTransient(): main converter");
    MMSimHitCollection* p_collection(0);
    if( compareClassGuid(p2_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
      std::unique_ptr< Muon::MMSimHitCollection_p2 >   col_vect( this->poolReadObject< Muon::MMSimHitCollection_p2 >() );
      p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
      std::unique_ptr< Muon::MMSimHitCollection_p1 >   col_vect( this->poolReadObject< Muon::MMSimHitCollection_p1 >() );
      p_collection = m_TPConverter_p1.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else {
        throw std::runtime_error("Unsupported persistent version of MMSimHitCollection");

    }
    return p_collection;
}
