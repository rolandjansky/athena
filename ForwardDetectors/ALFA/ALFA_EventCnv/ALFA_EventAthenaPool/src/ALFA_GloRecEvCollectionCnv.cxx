/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_GloRecEvCollectionCnv_p1.h"
#include "ALFA_GloRecEvCollectionCnv.h"

 

ALFA_GloRecEvCollection_PERS* ALFA_GloRecEvCollectionCnv::createPersistent(ALFA_GloRecEvCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_GloRecEvCollectionConverter" );
    ALFA_GloRecEvCollectionCnv_p1   TPConverter;
    ALFA_GloRecEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_GloRecEvCollection* ALFA_GloRecEvCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_GloRecEvCollectionConverter" );
    
    ALFA_GloRecEvCollectionCnv_p1   TPConverter_p1;

    ALFA_GloRecEvCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("210D255A-F94F-4009-9A9B-2122597330D5");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_GloRecEvCollection_p1 >   col_vect( this->poolReadObject< ALFA_GloRecEvCollection_p1 >() );
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
