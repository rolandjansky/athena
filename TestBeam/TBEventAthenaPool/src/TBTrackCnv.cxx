/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTrackCnv.h"

TBTrack_PERS* TBTrackCnv::createPersistent(TBTrack* transCont) {
    MsgStream log(messageService(), "TBTrackConverter" );
    TBTrack_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBTrack* TBTrackCnv::createTransient() {
   MsgStream log(messageService(), "TBTrackConverter" );
   static pool::Guid   p1_guid("9E50CAA3-CCDF-4BB9-96E9-E376B16963E8");  // GUID of the persistent object
   static pool::Guid   p0_guid("37E880D7-EBCB-4F53-8B14-490A4CEB71BD");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBTrack_p1 > col_vect( poolReadObject< TBTrack_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTrack >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}


