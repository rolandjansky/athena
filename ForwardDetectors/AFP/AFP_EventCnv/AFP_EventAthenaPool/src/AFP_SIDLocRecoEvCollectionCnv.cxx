/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollectionCnv_p1.h"
#include "AFP_SIDLocRecoEvCollectionCnv.h"

 

AFP_SIDLocRecoEvCollection_PERS* AFP_SIDLocRecoEvCollectionCnv::createPersistent(AFP_SIDLocRecoEvCollection* transCont) {
    MsgStream mlog(msgSvc(), "AFP_SIDLocRecoEvCollectionConverter" );
    AFP_SIDLocRecoEvCollectionCnv_p1   TPConverter;
    AFP_SIDLocRecoEvCollection_PERS *persObj = TPConverter.createPersistent( transCont, mlog );
    return persObj;
}


AFP_SIDLocRecoEvCollection* AFP_SIDLocRecoEvCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "AFP_SIDLocRecoEvCollectionConverter" );
    
    AFP_SIDLocRecoEvCollectionCnv_p1   TPConverter_p1;

    AFP_SIDLocRecoEvCollection       *trans_cont(0); // probably inicialization
    
    //GUID of persistence collection class (see selection.xml in AFP_EventTPCnv, class item AFP_SIDLocRecoEvCollection_p1)
    static const pool::Guid p1_guid ("D0C144F2-E82E-44B3-84BC-22C7D10F262E");
    
    if( this->compareClassGuid(p1_guid)) {
         std::auto_ptr< AFP_SIDLocRecoEvCollection_p1 >   col_vect( this->poolReadObject< AFP_SIDLocRecoEvCollection_p1 >() );
        trans_cont = TPConverter_p1.createTransient( col_vect.get(), mlog );
    }
 
    else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
