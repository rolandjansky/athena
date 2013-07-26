/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBADCRawContCnv.h"

TBADCRawCont_PERS* TBADCRawContCnv::createPersistent(TBADCRawCont* transCont) {
    MsgStream log(messageService(), "TBADCRawContConverter" );
    TBADCRawCont_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBADCRawCont* TBADCRawContCnv::createTransient() {
   MsgStream log(messageService(), "TBADCRawContConverter" );
   static pool::Guid   p1_guid("4D0407FD-A534-4595-B5BD-4A8DB797A094");  // GUID of the persistent object
   static pool::Guid   p0_guid("5D407302-FD4E-42BC-A6E5-F5C9ECCDCBF1");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBADCRawCont_p1 > col_vect( poolReadObject< TBADCRawCont_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBADCRawCont >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

