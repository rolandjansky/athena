/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GenericMuonSimHitCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEventTPCnv/GenericMuonSimHitCollection_p1.h"
#include "HitManagement/AthenaHitsVector.h" //for back-compatibility


GenericMuonSimHitCollectionCnv::GenericMuonSimHitCollectionCnv(ISvcLocator* svcloc) :
    GenericMuonSimHitCollectionCnvBase(svcloc)
{
}

GenericMuonSimHitCollectionCnv::~GenericMuonSimHitCollectionCnv() {
}

GenericMuonSimHitCollection_PERS*    GenericMuonSimHitCollectionCnv::createPersistent (GenericMuonSimHitCollection* transCont) {
    MsgStream log(msgSvc(), "GenericMuonSimHitCollectionCnv" );
    ATH_MSG_DEBUG("createPersistent(): main converter");
    GenericMuonSimHitCollection_PERS *pixdc_p= m_TPConverter_p1.createPersistent( transCont, log );
    return pixdc_p;
}

GenericMuonSimHitCollection* GenericMuonSimHitCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "GenericMuonSimHitCollectionCnv" );
    static pool::Guid   p1_guid("C2D5D558-2671-11E1-AAFA-001E4F3E5646"); 
    ATH_MSG_DEBUG("createTransient(): main converter");
    GenericMuonSimHitCollection* p_collection(0);
    if( compareClassGuid(p1_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
      std::auto_ptr< GenericMuonSimHitCollection_PERS >   col_vect( this->poolReadObject< GenericMuonSimHitCollection_PERS >() );
      p_collection = m_TPConverter_p1.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else {
        throw std::runtime_error("Unsupported persistent version of GenericMuonSimHitCollection");

    }
    return p_collection;
}
