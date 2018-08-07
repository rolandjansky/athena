/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ZDC_SimPixelHit_CollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by ZDC_SimPixelHit_CollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "ZDC_SimPixelHit_CollectionCnv.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_CollectionCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

ZDC_SimPixelHit_Collection_PERS* ZDC_SimPixelHit_CollectionCnv::createPersistent(ZDC_SimPixelHit_Collection* transCont) {

    MsgStream mlog(msgSvc(), "ZDC_SimPixelHit_CollectionConverter" );
    ZDC_SimPixelHit_CollectionCnv_p1   converter;
    ZDC_SimPixelHit_Collection_PERS *persObj = converter.createPersistent( transCont, mlog );


    return persObj;
}

ZDC_SimPixelHit_Collection* ZDC_SimPixelHit_CollectionCnv::createTransient() {


    MsgStream mlog(msgSvc(), "ZDC_SimPixelHit_CollectionConverter" );
    ZDC_SimPixelHit_CollectionCnv_p1   converter_p1;

    ZDC_SimPixelHit_Collection       *trans_cont(0);

    static const pool::Guid   p1_guid("EBCA37BF-38FF-4F19-B577-E5D0EFFA51D0");
    if( this->compareClassGuid(p1_guid)) {
      std::auto_ptr< ZDC_SimPixelHit_Collection_PERS >   col_vect( this->poolReadObject< ZDC_SimPixelHit_Collection_PERS >() );
      trans_cont = converter_p1.createTransient(col_vect.get(), mlog );
    }
    else {
      throw std::runtime_error("Unsupported persistent version of Data collection");
    }


    return trans_cont;
}

