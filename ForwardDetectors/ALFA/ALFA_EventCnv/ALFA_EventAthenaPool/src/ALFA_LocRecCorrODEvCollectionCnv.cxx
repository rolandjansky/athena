/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvCollectionCnv_p1.h"
#include "ALFA_LocRecCorrODEvCollectionCnv.h"

 

ALFA_LocRecCorrODEvCollection_PERS* ALFA_LocRecCorrODEvCollectionCnv::createPersistent(ALFA_LocRecCorrODEvCollection* transCont) {
    MsgStream mlog(messageService(), "ALFA_LocRecCorrODEvCollectionConverter" );
    ALFA_LocRecCorrODEvCollectionCnv_p1   TPConverter;
    ALFA_LocRecCorrODEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


ALFA_LocRecCorrODEvCollection* ALFA_LocRecCorrODEvCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "ALFA_LocRecCorrODEvCollectionConverter" );
    
    ALFA_LocRecCorrODEvCollectionCnv_p1   TPConverter_p1;

    ALFA_LocRecCorrODEvCollection       *trans_cont(0); // probably inicialization
    static const pool::Guid p1_guid ("5EB65543-BEC9-4920-B545-F1C6AB88C289");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< ALFA_LocRecCorrODEvCollection_p1 >   col_vect( this->poolReadObject< ALFA_LocRecCorrODEvCollection_p1 >() );
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
