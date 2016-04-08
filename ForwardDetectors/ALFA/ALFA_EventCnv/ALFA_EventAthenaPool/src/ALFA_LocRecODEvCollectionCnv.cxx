/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollectionCnv_p1.h"
#include "ALFA_LocRecODEvCollectionCnv.h"

 

ALFA_LocRecODEvCollection_PERS* ALFA_LocRecODEvCollectionCnv::createPersistent(ALFA_LocRecODEvCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_LocRecODEvCollectionConverter" );
    ALFA_LocRecODEvCollectionCnv_p1   TPConverter;
    ALFA_LocRecODEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_LocRecODEvCollection* ALFA_LocRecODEvCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_LocRecODEvCollectionConverter" );
    
    ALFA_LocRecODEvCollectionCnv_p1   TPConverter_p1;

    ALFA_LocRecODEvCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("D6688847-9903-4FDE-B709-879B0E470073");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_LocRecODEvCollection_p1 >   col_vect( this->poolReadObject< ALFA_LocRecODEvCollection_p1 >() );
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
