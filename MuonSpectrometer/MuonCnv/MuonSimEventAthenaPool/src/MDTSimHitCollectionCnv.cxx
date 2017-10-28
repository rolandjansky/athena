/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTSimHitCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEventTPCnv/MDTSimHitCollection_p1.h"
#include "HitManagement/AthenaHitsVector.h" //for back-compatibility


MDTSimHitCollectionCnv::MDTSimHitCollectionCnv(ISvcLocator* svcloc) :
    MDTSimHitCollectionCnvBase(svcloc)
{
}

MDTSimHitCollectionCnv::~MDTSimHitCollectionCnv() {
}

MDTSimHitCollection_PERS*    MDTSimHitCollectionCnv::createPersistent (MDTSimHitCollection* transCont) {
    MsgStream log(msgSvc(), "MDTSimHitCollectionCnv" );
    ATH_MSG_DEBUG("createPersistent(): main converter");
    MDTSimHitCollection_PERS *pixdc_p= m_TPConverter_p2.createPersistent( transCont, log );
    return pixdc_p;
}

MDTSimHitCollection* MDTSimHitCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "MDTSimHitCollectionCnv" );
    static pool::Guid   p0_guid("D76D06CC-C15F-43E6-BBC3-480DE5DA065D"); // before t/p split
    static pool::Guid   p1_guid("EA781971-65C5-4B30-9D22-EEFB764BA0B3"); 
    static pool::Guid   p2_guid("92880B97-75BB-4C5D-8183-577338059FCC");
    static pool::Guid   p3_guid("0E9EEEE2-304F-44B8-B1DF-E75297183A02");
    ATH_MSG_DEBUG("createTransient(): main converter");
    MDTSimHitCollection* p_collection(0);
    if( compareClassGuid(p3_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 3 detected");
      std::auto_ptr< Muon::MDTSimHitCollection_p3 >   col_vect( this->poolReadObject< Muon::MDTSimHitCollection_p3 >() );
      p_collection = m_TPConverter_p3.createTransient( col_vect.get(), log );
    } else if( compareClassGuid(p2_guid) ) {
      ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
      std::auto_ptr< Muon::MDTSimHitCollection_p2 >   col_vect( this->poolReadObject< Muon::MDTSimHitCollection_p2 >() );
      p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
        ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
        // poolReadObject< MDTSimHitCollection_PERS >( m_TPConverter );
        // p_collection = m_TPConverter.createTransient( log );
        std::auto_ptr< Muon::MDTSimHitCollection_p1 >   col_vect( this->poolReadObject< Muon::MDTSimHitCollection_p1 >() );
        p_collection = m_TPConverter_p1.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        AthenaHitsVector<MDTSimHit>* oldColl = this->poolReadObject< AthenaHitsVector<MDTSimHit> >();
        size_t size = oldColl->size();
        p_collection=new MDTSimHitCollection("DefaultCollectionName",size);
        //p_collection->reserve(size);
        //do the copy
        AthenaHitsVector<MDTSimHit>::const_iterator it = oldColl->begin(), itEnd=oldColl->end();
        for (;it!=itEnd;it++)  {
            p_collection->push_back(**it); 
        }
        delete oldColl;
    }
    else {
        throw std::runtime_error("Unsupported persistent version of MDTSimHitCollection");

    }
    return p_collection;
}
