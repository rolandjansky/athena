/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileSimEventTPCnv/TileHitVectorCnv_p1.h"
#include "TileHitVectorCnv.h"
#include "TileSimEventTPCnv/TileHit_p1.h"


TileHitVector_PERS* TileHitVectorCnv::createPersistent(TileHitVector* transCont) {
    MsgStream mlog(messageService(), "TileHitVectorConverter" );
    TileHitVectorCnv_p1   converter;
    TileHitVector_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}


TileHitVector* TileHitVectorCnv::createTransient() {

    MsgStream mlog(messageService(), "TileHitVectorConverter" );
    TileHitVectorCnv_p1   converter_p1;

    TileHitVector* trans_cont(0);

    static const pool::Guid   p1_guid("65AD597A-BF97-46EE-B9E3-203B35218EA7");
    static const pool::Guid   p0_guid("EA2209D1-C339-453D-AEAD-21C026F0735E");

    if( this->compareClassGuid(p1_guid)) {

        std::auto_ptr< TileHitVector_p1 > col_vect( this->poolReadObject< TileHitVector_p1 >() );
        trans_cont = converter_p1.createTransient( col_vect.get(), mlog );

    } else if( this->compareClassGuid(p0_guid)) {

        // old version from before TP separation
        TileOldHitVector* oldVec = this->poolReadObject<TileOldHitVector>();
        size_t size = oldVec->size();
        trans_cont = new TileHitVector();
        trans_cont->reserve(size);
        // copy all hits to new vector

        TileOldHitVector::const_iterator it = oldVec->begin(), itEnd = oldVec->end();
        for (;it != itEnd; ++it)  {
          trans_cont->push_back(**it);
        }

        delete oldVec;

    } else {
      throw std::runtime_error("Unsupported persistent version of Data container");
    }

    return trans_cont;
}
