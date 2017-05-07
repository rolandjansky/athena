/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollectionCnv_p1.h"
#include "AFP_TDLocRecoEvCollectionCnv.h"

 

AFP_TDLocRecoEvCollection_PERS* AFP_TDLocRecoEvCollectionCnv::createPersistent(AFP_TDLocRecoEvCollection* transCont) {
    MsgStream mlog(msgSvc(), "AFP_TDLocRecoEvCollectionConverter" );
    AFP_TDLocRecoEvCollectionCnv_p1   TPConverter;
    AFP_TDLocRecoEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


AFP_TDLocRecoEvCollection* AFP_TDLocRecoEvCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "AFP_TDLocRecoEvCollectionConverter" );
    
    AFP_TDLocRecoEvCollectionCnv_p1   TPConverter_p1;

    AFP_TDLocRecoEvCollection       *trans_cont(0); // probably inicialization
    
    //GUID of persistence collection class (see selection.xml in AFP_EventTPCnv, class item AFP_TDLocRecoEvCollection_p1)
    static const pool::Guid p1_guid ("3149C8A8-DEED-4922-8705-1D727A280B9E");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< AFP_TDLocRecoEvCollection_p1 >   col_vect( this->poolReadObject< AFP_TDLocRecoEvCollection_p1 >() );
        trans_cont = TPConverter_p1.createTransient( col_vect.get(), mlog );
    }
 
    else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
