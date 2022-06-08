/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBBPCContCnv.h"

TBBPCCont_PERS* TBBPCContCnv::createPersistent(TBBPCCont* transCont) {
    MsgStream log(msgSvc(), "TBBPCContConverter" );
    TBBPCCont_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
TBBPCCont* TBBPCContCnv::createTransient() {
   MsgStream log(msgSvc(), "TBBPCContConverter" );
   static const pool::Guid   p1_guid("89A09260-F284-4694-8687-3C6C72989A96");  // GUID of the persistent object
   static const pool::Guid   p0_guid("05E6C927-F43C-4C2A-B654-C671BF8FDB67");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< TBBPCCont_p1 > col_vect( poolReadObject< TBBPCCont_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBBPCCont >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}
