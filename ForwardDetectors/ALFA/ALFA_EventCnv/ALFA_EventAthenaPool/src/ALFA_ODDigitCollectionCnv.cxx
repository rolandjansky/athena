/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_ODDigitCollectionCnv_p1.h"
#include "ALFA_ODDigitCollectionCnv.h"

 

ALFA_ODDigitCollection_PERS* ALFA_ODDigitCollectionCnv::createPersistent(ALFA_ODDigitCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_ODDigitCollectionConverter" );
    ALFA_ODDigitCollectionCnv_p1   TPConverter;
    ALFA_ODDigitCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_ODDigitCollection* ALFA_ODDigitCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_ODDigitCollectionConverter" );
    
    ALFA_ODDigitCollectionCnv_p1   TPConverter_p1;

    ALFA_ODDigitCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("26807BD7-E90A-4FDF-B1BF-0A7F821C5970");

    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_ODDigitCollection_p1 >   col_vect( this->poolReadObject< ALFA_ODDigitCollection_p1 >() );
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
