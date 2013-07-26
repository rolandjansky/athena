/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTriggerPatternUnitCnv.h"

TBTriggerPatternUnit_PERS* TBTriggerPatternUnitCnv::createPersistent(TBTriggerPatternUnit* transCont) {
    MsgStream log(messageService(), "TBTriggerPatternUnitConverter" );
    TBTriggerPatternUnit_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}
    
TBTriggerPatternUnit* TBTriggerPatternUnitCnv::createTransient() {
   MsgStream log(messageService(), "TBTriggerPatternUnitConverter" );
   static pool::Guid   p1_guid("7630C108-3B5F-4ED3-97C8-F3148AF1B84F");  // GUID of the persistent object
   static pool::Guid   p0_guid("0B82A5B3-0808-4B78-B47B-BFE9388EEEEB");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TBTriggerPatternUnit_p1 > col_vect( poolReadObject< TBTriggerPatternUnit_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTriggerPatternUnit >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

