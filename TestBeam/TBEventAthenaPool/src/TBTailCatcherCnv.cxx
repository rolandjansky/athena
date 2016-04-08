/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTailCatcherCnv.h"

TBTailCatcher_PERS* TBTailCatcherCnv::createPersistent(TBTailCatcher* transCont) {
    MsgStream log(messageService(), "TBTailCatcherConverter" );
    TBTailCatcher_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBTailCatcher* TBTailCatcherCnv::createTransient() {
   MsgStream log(messageService(), "TBTailCatcherConverter" );
   static pool::Guid   p1_guid("B304A3B8-5FC1-415D-AE97-E3E7B6769213");  // GUID of the persistent object
   static pool::Guid   p0_guid("7AA6F3C3-3FF2-4732-B0CB-4ECC32FEF06D");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBTailCatcher_p1 > col_vect( poolReadObject< TBTailCatcher_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTailCatcher >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

