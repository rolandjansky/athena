/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBMWPCContCnv.h"

TBMWPCCont_PERS* TBMWPCContCnv::createPersistent(TBMWPCCont* transCont) {
    MsgStream log(messageService(), "TBMWPCContConverter" );
    TBMWPCCont_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBMWPCCont* TBMWPCContCnv::createTransient() {
   MsgStream log(messageService(), "TBMWPCContConverter" );
   static pool::Guid   p1_guid("E1F16B8A-9D8B-4906-83EE-E38EBE63768A");  // GUID of the persistent object
   static pool::Guid   p0_guid("05F6DFD7-EB63-49DB-AA5B-F03BE66365AC");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBMWPCCont_p1 > col_vect( poolReadObject< TBMWPCCont_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBMWPCCont >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

