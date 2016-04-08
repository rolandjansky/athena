/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEventInfoCnv.h"

TBEventInfo_PERS* TBEventInfoCnv::createPersistent(TBEventInfo* transCont) {
    MsgStream log(messageService(), "TBEventInfoConverter" );
    TBEventInfo_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBEventInfo* TBEventInfoCnv::createTransient() {
   MsgStream log(messageService(), "TBEventInfoConverter" );
   static pool::Guid   p1_guid("1E398439-74B3-457E-BC0B-DD69201AE463");  // GUID of the persistent object
   static pool::Guid   p0_guid("1295BE02-D698-43A8-A2BB-76E5ADCBD228");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBEventInfo_p1 > col_vect( poolReadObject< TBEventInfo_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBEventInfo >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

