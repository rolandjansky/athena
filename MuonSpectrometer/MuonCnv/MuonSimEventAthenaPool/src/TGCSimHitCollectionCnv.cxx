/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSimHitCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEventTPCnv/TGCSimHitCollection_p1.h"
#include "HitManagement/AthenaHitsVector.h" //for back-compatibility


TGCSimHitCollectionCnv::TGCSimHitCollectionCnv(ISvcLocator* svcloc) :
    TGCSimHitCollectionCnvBase(svcloc)
{
}

TGCSimHitCollectionCnv::~TGCSimHitCollectionCnv() {
}

TGCSimHitCollection_PERS*    TGCSimHitCollectionCnv::createPersistent (TGCSimHitCollection* transCont) {
    MsgStream log(msgSvc(), "TGCSimHitCollectionCnv" );
    log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    TGCSimHitCollection_PERS *pixdc_p= m_TPConverter_p3.createPersistent( transCont, log );
    return pixdc_p;
}

TGCSimHitCollection* TGCSimHitCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "TGCSimHitCollectionCnv" );
    static pool::Guid   p0_guid("D7C91818-4730-4717-A650-107F52C0221E"); // before t/p split
    static pool::Guid   p1_guid("BD569381-2489-4402-BBD3-91EE8C009F26"); 
    static pool::Guid   p2_guid("710241EE-D5F7-4CB6-A1D7-6E80D040E637"); 
    static pool::Guid   p3_guid("BACD19A8-4737-4857-A22D-B9DF833A0A80");
    static pool::Guid   p4_guid("F8F77562-BBD9-45DB-A276-6A21018EA7FA");
    log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    TGCSimHitCollection* p_collection(0);
    if(compareClassGuid(p4_guid) ) {
      log<<MSG::DEBUG<<"createTransient(): T/P version 4 detected"<<endmsg;
      std::auto_ptr< Muon::TGCSimHitCollection_p4 >   col_vect( this->poolReadObject< Muon::TGCSimHitCollection_p4 >() );
      p_collection = m_TPConverter_p4.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if(compareClassGuid(p3_guid) ) {
        log<<MSG::DEBUG<<"createTransient(): T/P version 3 detected"<<endmsg;
        std::auto_ptr< Muon::TGCSimHitCollection_p3 >   col_vect( this->poolReadObject< Muon::TGCSimHitCollection_p3 >() );
        p_collection = m_TPConverter_p3.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p2_guid) ) {
        log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        std::auto_ptr< Muon::TGCSimHitCollection_p2 >   col_vect( this->poolReadObject< Muon::TGCSimHitCollection_p2 >() );
        p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p1_guid) ) {
        log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        std::auto_ptr< Muon::TGCSimHitCollection_p1 >   col_vect( this->poolReadObject< Muon::TGCSimHitCollection_p1 >() );
        p_collection = m_TPConverter_p1.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        AthenaHitsVector<TGCSimHit>* oldColl = this->poolReadObject< AthenaHitsVector<TGCSimHit> >();
        size_t size = oldColl->size();
        p_collection=new TGCSimHitCollection("DefaultCollectionName",size);
        //p_collection->reserve(size);
        //do the copy
        AthenaHitsVector<TGCSimHit>::const_iterator it = oldColl->begin(), itEnd=oldColl->end();
        for (;it!=itEnd;it++)  {
            p_collection->push_back(**it); 
        }
        delete oldColl;
    }
    else {
        throw std::runtime_error("Unsupported persistent version of TGCSimHitCollection");

    }
    return p_collection;
}
