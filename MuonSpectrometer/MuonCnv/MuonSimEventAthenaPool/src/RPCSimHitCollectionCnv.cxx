/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCSimHitCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEventTPCnv/RPCSimHitCollection_p1.h"
#include "MuonSimEventTPCnv/RPCSimHitCollection_p2.h"
#include "MuonSimEventTPCnv/RPCSimHitCollection_p3.h"
#include "HitManagement/AthenaHitsVector.h" //for back-compatibility


RPCSimHitCollectionCnv::RPCSimHitCollectionCnv(ISvcLocator* svcloc) :
    RPCSimHitCollectionCnvBase(svcloc)
{
}

RPCSimHitCollectionCnv::~RPCSimHitCollectionCnv() {
}

RPCSimHitCollection_PERS*    RPCSimHitCollectionCnv::createPersistent (RPCSimHitCollection* transCont) {
    MsgStream log(msgSvc(), "RPCSimHitCollectionCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endmsg;
    RPCSimHitCollection_PERS *pixdc_p= m_TPConverter_p2.createPersistent( transCont, log );
    return pixdc_p;
}

RPCSimHitCollection* RPCSimHitCollectionCnv::createTransient() {
    MsgStream log(msgSvc(), "RPCSimHitCollectionCnv" );
    static pool::Guid   p0_guid("45EB013E-FC8E-4612-88B7-6E0CAF718F79"); // before t/p split
    static pool::Guid   p1_guid("C4C57487-41DC-4706-9604-721D76F0AA52"); 
    static pool::Guid   p2_guid("1B611C70-CC6F-42AE-9F6D-7DA6A9A22546");
    static pool::Guid   p3_guid("B48E5E17-FB26-4BC0-A0E2-5324925EAE2F");
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endmsg;
    RPCSimHitCollection* p_collection(0);
    if( compareClassGuid(p3_guid) ) {
      if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 3 detected"<<endmsg;
      // poolReadObject< RPCSimHitCollection_PERS >( m_TPConverter );
      // p_collection = m_TPConverter.createTransient( log );
      std::auto_ptr< Muon::RPCSimHitCollection_p3 >   col_vect( this->poolReadObject< Muon::RPCSimHitCollection_p3 >() );
      p_collection = m_TPConverter_p3.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endmsg;
        // poolReadObject< RPCSimHitCollection_PERS >( m_TPConverter );
        // p_collection = m_TPConverter.createTransient( log );
        std::auto_ptr< Muon::RPCSimHitCollection_p2 >   col_vect( this->poolReadObject< Muon::RPCSimHitCollection_p2 >() );
        p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endmsg;
        // poolReadObject< RPCSimHitCollection_PERS >( m_TPConverter );
        // p_collection = m_TPConverter.createTransient( log );
        std::auto_ptr< Muon::RPCSimHitCollection_p1 >   col_vect( this->poolReadObject< Muon::RPCSimHitCollection_p1 >() );
        p_collection = m_TPConverter.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        AthenaHitsVector<RPCSimHit>* oldColl = this->poolReadObject< AthenaHitsVector<RPCSimHit> >();
        size_t size = oldColl->size();
        p_collection=new RPCSimHitCollection("DefaultCollectionName",size);
        //p_collection->reserve(size);
        //do the copy
        AthenaHitsVector<RPCSimHit>::const_iterator it = oldColl->begin(), itEnd=oldColl->end();
        for (;it!=itEnd;it++)  {
            p_collection->push_back(**it); 
        }
        delete oldColl;
    }
    else {
        throw std::runtime_error("Unsupported persistent version of RPCSimHitCollection");

    }
    return p_collection;
}
