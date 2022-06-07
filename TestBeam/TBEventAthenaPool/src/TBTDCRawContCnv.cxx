/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTDCRawContCnv.h"

TBTDCRawCont_PERS* TBTDCRawContCnv::createPersistent(TBTDCRawCont* transCont) {
    MsgStream log(msgSvc(), "TBTDCRawContConverter" );
    TBTDCRawCont_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
TBTDCRawCont* TBTDCRawContCnv::createTransient() {
   MsgStream log(msgSvc(), "TBTDCRawContConverter" );
   static const pool::Guid   p1_guid("C05FD0A4-E5AD-41C8-9EA5-2A71378BB247");  // GUID of the persistent object
   static const pool::Guid   p0_guid("82D6D14E-49FE-4841-9B39-08D66B9DC7CE");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< TBTDCRawCont_p1 > col_vect( poolReadObject< TBTDCRawCont_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTDCRawCont >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

