/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ZDC_SimStripHit_CollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by ZDC_SimStripHit_CollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "ZDC_SimStripHit_CollectionCnv.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_CollectionCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

ZDC_SimStripHit_Collection_PERS* ZDC_SimStripHit_CollectionCnv::createPersistent(ZDC_SimStripHit_Collection* transCont) {

    MsgStream mlog(msgSvc(), "ZDC_SimStripHit_CollectionConverter" );
    ZDC_SimStripHit_CollectionCnv_p1   converter;
    ZDC_SimStripHit_Collection_PERS *persObj = converter.createPersistent( transCont, mlog );


    return persObj;
}

ZDC_SimStripHit_Collection* ZDC_SimStripHit_CollectionCnv::createTransient() {


    MsgStream mlog(msgSvc(), "ZDC_SimStripHit_CollectionConverter" );
    ZDC_SimStripHit_CollectionCnv_p1   converter_p1;

    ZDC_SimStripHit_Collection       *trans_cont(0);

    static const pool::Guid   p1_guid("1082F8C1-BA19-47AF-83D7-4669BD12E9AC");
    if( this->compareClassGuid(p1_guid)) {
      std::auto_ptr< ZDC_SimStripHit_Collection_PERS >   col_vect( this->poolReadObject< ZDC_SimStripHit_Collection_PERS >() );
      trans_cont = converter_p1.createTransient(col_vect.get(), mlog );
    }
    else {
      throw std::runtime_error("Unsupported persistent version of Data collection");
    }


    return trans_cont;
}

