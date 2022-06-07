/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTDCCnv.h"

TBTDC_PERS* TBTDCCnv::createPersistent(TBTDC* transCont) {
    MsgStream log(msgSvc(), "TBTDCConverter" );
    TBTDC_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
TBTDC* TBTDCCnv::createTransient() {
   MsgStream log(msgSvc(), "TBTDCConverter" );
   static const pool::Guid   p1_guid("D909CCF6-CE91-4401-98FC-C1C41BD06513");  // GUID of the persistent object
   static const pool::Guid   p0_guid("51CEFACC-47E5-4BF1-8BF5-FA48FCA15B43");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< TBTDC_p1 > col_vect( poolReadObject< TBTDC_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTDC >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}
