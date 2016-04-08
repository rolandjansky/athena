/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecEvCollectionCnv_p1.h"
#include "ALFA_LocRecEvCollectionCnv.h"

 

ALFA_LocRecEvCollection_PERS* ALFA_LocRecEvCollectionCnv::createPersistent(ALFA_LocRecEvCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_LocRecEvCollectionConverter" );
    ALFA_LocRecEvCollectionCnv_p1   TPConverter;
    ALFA_LocRecEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_LocRecEvCollection* ALFA_LocRecEvCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_LocRecEvCollectionConverter" );
    
    ALFA_LocRecEvCollectionCnv_p1   TPConverter_p1;

    ALFA_LocRecEvCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("C392157A-4519-44B5-A472-16D1F74F4CAD");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_LocRecEvCollection_p1 >   col_vect( this->poolReadObject< ALFA_LocRecEvCollection_p1 >() );
        trans_cont = TPConverter_p1.createTransient( col_vect.get(), mlog );
    }
//    else if( m_token.find("CLID=35722E01-C4E3-420E-8A7E-E375C5E7989D") != std::string::npos) {
        // old version from before TP separation, just return it
//        trans_cont = this->poolReadObject<ALFA_DigitCollection>();
//    }  
       else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
