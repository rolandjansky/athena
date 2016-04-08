/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_HitCollectionCnv_p1.h"
#include "ALFA_HitCollectionCnv.h"
#include "ALFA_EventTPCnv/ALFA_Hit_p1.h"
 

ALFA_HitCollection_PERS* ALFA_HitCollectionCnv::createPersistent(ALFA_HitCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_HitCollectionConverter" );
    ALFA_HitCollectionCnv_p1   TPConverter;
    ALFA_HitCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_HitCollection* ALFA_HitCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_HitCollectionConverter" );
    
    ALFA_HitCollectionCnv_p1   TPConverter_p1;

    ALFA_HitCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid   p1_guid("96320556-17C7-4E8D-9D15-DF955193F002");

    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_HitCollection_p1 >   col_vect( this->poolReadObject< ALFA_HitCollection_p1 >() );
        trans_cont = TPConverter_p1.createTransient( col_vect.get(), mlog );
    }
//    else if( m_token.find("CLID=35722E01-C4E3-420E-8A7E-E375C5E7989D") != std::string::npos) {
        // old version from before TP separation, just return it
//        trans_cont = this->poolReadObject<ALFA_HitCollection>();
//    }  
       else {
        throw std::runtime_error("nazdar, Unsupported persistent version of Data container");
    }
    return trans_cont;
}
