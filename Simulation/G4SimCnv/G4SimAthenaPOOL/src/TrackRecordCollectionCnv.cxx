/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4SimTPCnv/TrackRecordCollectionCnv_p1.h"
#include "G4SimTPCnv/TrackRecordCollectionCnv_p2.h"
#include "TrackRecordCollectionCnv.h"
#include "G4SimTPCnv/TrackRecord_p0.h"

typedef AtlasHitsVector<TrackRecord> TrackRecordCollection_p0;

TrackRecordCollection_PERS* TrackRecordCollectionCnv::createPersistent(TrackRecordCollection* transCont) {
   MsgStream mlog(msgSvc(), "TrackRecordCollectionConverter" );
   TrackRecordCollectionCnv_p2   converter;
   TrackRecordCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
   return persObj;
}


TrackRecordCollection* TrackRecordCollectionCnv::createTransient() {

    MsgStream mlog(msgSvc(), "TrackRecordCollectionConverter" );
    TrackRecordCollectionCnv_p1   converter;
    TrackRecordCollectionCnv_p2   converter_p2;
    TrackRecordCollection       *trans_cont(0);
    
   // GUIDs are here:
    static const pool::Guid   p1_guid("1B1EEE3B-4647-41B4-B1D4-495DF77F0D3C");
    static const pool::Guid   p2_guid("22D044AD-A13A-42BF-B2A4-BDAF5BE2D819");
    static const pool::Guid   old_guid("0F77F941-6019-4FE5-B6F4-E90BE9F7ACC9");

     if( this->compareClassGuid(p1_guid))  {
      std::auto_ptr< TrackRecordCollection_p1 >   col_vect( this->poolReadObject< TrackRecordCollection_p1 >() );
      trans_cont = converter.createTransient( col_vect.get(), mlog );
   }
   else  if( this->compareClassGuid(old_guid)) {
      // old version from before TP separation
        TrackRecordCollection_p0 *oldObj = this->poolReadObject<TrackRecordCollection_p0>();
        trans_cont = oldObj;
   }  
   // New _p2 version faster and smaller
   else  if( this->compareClassGuid(p2_guid))  {
      std::auto_ptr< TrackRecordCollection_p2 >   col_vect( this->poolReadObject< TrackRecordCollection_p2 >() );
      trans_cont = converter_p2.createTransient( col_vect.get(), mlog );
   }
   else 
   {
      throw std::runtime_error("Unsupported persistent version of Data container");
   }
   return trans_cont;
}


