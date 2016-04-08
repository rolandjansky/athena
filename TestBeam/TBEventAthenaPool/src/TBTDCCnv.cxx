/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTDCCnv.h"

TBTDC_PERS* TBTDCCnv::createPersistent(TBTDC* transCont) {
    MsgStream log(messageService(), "TBTDCConverter" );
    TBTDC_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBTDC* TBTDCCnv::createTransient() {
   MsgStream log(messageService(), "TBTDCConverter" );
   static pool::Guid   p1_guid("D909CCF6-CE91-4401-98FC-C1C41BD06513");  // GUID of the persistent object
   static pool::Guid   p0_guid("51CEFACC-47E5-4BF1-8BF5-FA48FCA15B43");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBTDC_p1 > col_vect( poolReadObject< TBTDC_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTDC >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}
