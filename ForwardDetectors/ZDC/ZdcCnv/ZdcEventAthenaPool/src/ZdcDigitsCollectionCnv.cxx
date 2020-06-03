/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcEventTPCnv/ZdcDigitsCollectionCnv_p1.h"
#include "ZdcDigitsCollectionCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

ZdcDigitsCollection_PERS* ZdcDigitsCollectionCnv::createPersistent(ZdcDigitsCollection* transCont) {

    MsgStream mlog(msgSvc(), "ZdcDigitsCollectionConverter" );
    ZdcDigitsCollectionCnv_p1   converter;
    ZdcDigitsCollection_PERS *persObj = converter.createPersistent( transCont, mlog );


    return persObj;
}

ZdcDigitsCollection* ZdcDigitsCollectionCnv::createTransient() {


    MsgStream mlog(msgSvc(), "ZdcDigitsCollectionConverter" );
    ZdcDigitsCollectionCnv_p1   converter_p1;

    ZdcDigitsCollection       *trans_cont(0);

    static const pool::Guid   p1_guid("B77B5F9A-1C29-4D74-A107-B3C71680C029");
    if( this->compareClassGuid(p1_guid)) {
      std::auto_ptr< ZdcDigitsCollection_p1 >   col_vect( this->poolReadObject< ZdcDigitsCollection_p1 >() );
      trans_cont = converter_p1.createTransient(col_vect.get(), mlog );
    }
    else {
      throw std::runtime_error("Unsupported persistent version of Data collection");
    }


    return trans_cont;
}
