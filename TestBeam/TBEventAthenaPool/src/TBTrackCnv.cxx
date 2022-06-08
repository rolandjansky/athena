/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTrackCnv.h"

TBTrack_PERS* TBTrackCnv::createPersistent(TBTrack* transCont) {
    MsgStream log(msgSvc(), "TBTrackConverter" );
    TBTrack_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
TBTrack* TBTrackCnv::createTransient() {
   MsgStream log(msgSvc(), "TBTrackConverter" );
   static const pool::Guid   p1_guid("9E50CAA3-CCDF-4BB9-96E9-E376B16963E8");  // GUID of the persistent object
   static const pool::Guid   p0_guid("37E880D7-EBCB-4F53-8B14-490A4CEB71BD");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< TBTrack_p1 > col_vect( poolReadObject< TBTrack_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTrack >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}


