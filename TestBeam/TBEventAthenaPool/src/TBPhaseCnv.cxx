/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBPhaseCnv.h"

TBPhase_PERS* TBPhaseCnv::createPersistent(TBPhase* transCont) {
    MsgStream log(messageService(), "TBPhaseConverter" );
    TBPhase_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBPhase* TBPhaseCnv::createTransient() {
   MsgStream log(messageService(), "TBPhaseConverter" );
   static pool::Guid   p1_guid("40443E91-053F-41C1-84F8-0A7F03F1DEC4");  // GUID of the persistent object
   static pool::Guid   p0_guid("C044C9BE-E371-4AC3-95ED-CCA05984EDE4");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBPhase_p1 > col_vect( poolReadObject< TBPhase_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBPhase >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

