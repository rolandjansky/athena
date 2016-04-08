/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBScintillatorContCnv.h"

TBScintillatorCont_PERS* TBScintillatorContCnv::createPersistent(TBScintillatorCont* transCont) {
    MsgStream log(messageService(), "TBScintillatorContConverter" );
    TBScintillatorCont_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBScintillatorCont* TBScintillatorContCnv::createTransient() {
   MsgStream log(messageService(), "TBScintillatorContConverter" );
   static pool::Guid   p1_guid("5887098D-DBFE-4634-92AB-CC9968CCB09A");  // GUID of the persistent object
   static pool::Guid   p0_guid("AE688514-B192-426D-A971-5CD072593272");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBScintillatorCont_p1 > col_vect( poolReadObject< TBScintillatorCont_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBScintillatorCont >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

