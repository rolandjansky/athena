/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_DigitCollectionCnv_p1.h"
#include "ALFA_DigitCollectionCnv.h"

 

ALFA_DigitCollection_PERS* ALFA_DigitCollectionCnv::createPersistent(ALFA_DigitCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_DigitCollectionConverter" );
    ALFA_DigitCollectionCnv_p1   TPConverter;
    ALFA_DigitCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_DigitCollection* ALFA_DigitCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_DigitCollectionConverter" );
    
    ALFA_DigitCollectionCnv_p1   TPConverter_p1;

    ALFA_DigitCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("0F21F6BB-6719-41D3-8219-3ABF523826CD");   
 
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_DigitCollection_p1 >   col_vect( this->poolReadObject< ALFA_DigitCollection_p1 >() );
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
