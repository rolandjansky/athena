/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_ODHitCollectionCnv_p1.h"
#include "ALFA_ODHitCollectionCnv.h"
#include "ALFA_EventTPCnv/ALFA_ODHit_p1.h"
 

ALFA_ODHitCollection_PERS* ALFA_ODHitCollectionCnv::createPersistent(ALFA_ODHitCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_ODHitCollectionConverter" );
    ALFA_ODHitCollectionCnv_p1   TPConverter;
    ALFA_ODHitCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_ODHitCollection* ALFA_ODHitCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_ODHitCollectionConverter" );
    
    ALFA_ODHitCollectionCnv_p1   TPConverter_p1;

    ALFA_ODHitCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid   p1_guid("11D7B0C9-54FF-44BF-A673-E9691004488B");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_ODHitCollection_p1 >   col_vect( this->poolReadObject< ALFA_ODHitCollection_p1 >() );
        trans_cont = TPConverter_p1.createTransient( col_vect.get(), mlog );
    }
//    else if( m_token.find("CLID=35722E01-C4E3-420E-8A7E-E375C5E7989D") != std::string::npos) {
        // old version from before TP separation, just return it
//        trans_cont = this->poolReadObject<ALFA_HitCollection>();
//    }  
       else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
