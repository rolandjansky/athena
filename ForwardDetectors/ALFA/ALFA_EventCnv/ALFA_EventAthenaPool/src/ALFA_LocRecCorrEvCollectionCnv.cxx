/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollectionCnv_p1.h"
#include "ALFA_LocRecCorrEvCollectionCnv.h"

 

ALFA_LocRecCorrEvCollection_PERS* ALFA_LocRecCorrEvCollectionCnv::createPersistent(ALFA_LocRecCorrEvCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_LocRecCorrEvCollectionConverter" );
    ALFA_LocRecCorrEvCollectionCnv_p1   TPConverter;
    ALFA_LocRecCorrEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_LocRecCorrEvCollection* ALFA_LocRecCorrEvCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_LocRecCorrEvCollectionConverter" );
    
    ALFA_LocRecCorrEvCollectionCnv_p1   TPConverter_p1;

    ALFA_LocRecCorrEvCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("E6C77BF6-011B-4A7F-847A-E34A0B402976");

    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_LocRecCorrEvCollection_p1 >   col_vect( this->poolReadObject< ALFA_LocRecCorrEvCollection_p1 >() );
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
