/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p1.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p2.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p3.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p4.h"
#include "TRTUncompressedHitCollectionCnv.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_Hit_p1.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_Hit_p2.h"


TRT_HitCollection_PERS* TRTUncompressedHitCollectionCnv::createPersistent(TRTUncompressedHitCollection* transCont) {
    MsgStream mlog(messageService(), "TRT_HitCollectionConverter" );
    TRT_HitCollectionCnv_PERS   converter;
    TRT_HitCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}


TRTUncompressedHitCollection* TRTUncompressedHitCollectionCnv::createTransient() {
    MsgStream mlog(messageService(), "TRT_HitCollectionConverter" );
    TRT_HitCollectionCnv_p1   converter_p1;
    TRT_HitCollectionCnv_p2   converter_p2;
    TRT_HitCollectionCnv_p3   converter_p3;
    TRT_HitCollectionCnv_p4   converter_p4;
    static const pool::Guid   p1_guid("6688E934-157E-421A-B6D1-A35FC8BD651C");
    static const pool::Guid   p2_guid("473FF621-3466-4D87-9469-4780A6A77023");
    static const pool::Guid   p3_guid("FB5F5BFC-43E5-44E1-B79C-C330C1480E2E");
    static const pool::Guid   p4_guid("73BECF03-4C45-491E-A973-A1C4402AD018");
    static const pool::Guid   old_guid("35722E01-C4E3-420E-8A7E-E375C5E7989D");


    TRTUncompressedHitCollection       *trans_cont(0);
    if( this->compareClassGuid(p4_guid)) {
        std::auto_ptr< TRT_HitCollection_p4 >   col_vect( this->poolReadObject< TRT_HitCollection_p4 >() );
        trans_cont = converter_p4.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< TRT_HitCollection_p1 >   col_vect( this->poolReadObject< TRT_HitCollection_p1 >() );
        trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p2_guid)) {
        std::auto_ptr< TRT_HitCollection_p2 >   col_vect( this->poolReadObject< TRT_HitCollection_p2 >() );
        trans_cont = converter_p2.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p3_guid)) {
        std::auto_ptr< TRT_HitCollection_p3 >   col_vect( this->poolReadObject< TRT_HitCollection_p3 >() );
        trans_cont = converter_p3.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(old_guid)) {
      // old version from before TP separation - convert to new transient format
      AthenaHitsVector<TRTUncompressedHit>* oldColl = this->poolReadObject< AthenaHitsVector<TRTUncompressedHit> >();
      size_t size = oldColl->size();
      trans_cont=new TRTUncompressedHitCollection("DefaultCollectionName",size);
      //do the copy
      AthenaHitsVector<TRTUncompressedHit>::const_iterator it = oldColl->begin(), itEnd=oldColl->end();
      for (;it!=itEnd;++it)  {
	trans_cont->push_back(**it);
      }
      delete oldColl;
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
