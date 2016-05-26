/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RingerRingsContainerCnv.h"
#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_p2.h"

static RingerRingsContainerCnv_tlp1 TPConverter_tlp1;
static RingerRingsContainerCnv_p2 TPConverter;
 
// createPersistent
RingerRingsContainer_PERS *RingerRingsContainerCnv::createPersistent(RingerRingsContainer *transObj) {
  MsgStream mlog(messageService(), "RingerRingsContainerConverter");

  mlog << MSG::DEBUG << "RingerRingsContainerCnv::createPersistent called" << endreq;

  RingerRingsContainer_PERS *p_RingerRingsCont = TPConverter.createPersistent(transObj, mlog);
 
  return p_RingerRingsCont;
 
}
 
// createTransient
RingerRingsContainer *RingerRingsContainerCnv::createTransient() {

  MsgStream mlog(messageService(), "RingerRingsContainerConverter");
  
  mlog << MSG::DEBUG << "RingerRingsContainerCnv::createTransient called" << endreq;

  static pool::Guid p2_guid("30D29B68-6D41-429E-AC16-930ACF71280D");
  static pool::Guid tlp1_guid("B5587828-50D8-4DAA-97F3-5E41A7F3E3FE");

  if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< RingerRingsContainer_p2 > col_vect( poolReadObject< RingerRingsContainer_p2 >() );
         //         std::cout << "Reading IMFC p2" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;
  } else if (compareClassGuid(tlp1_guid)) {
         std::auto_ptr< RingerRingsContainer_tlp1 > col_vect( poolReadObject< RingerRingsContainer_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return TPConverter_tlp1.createTransient( col_vect.get(), mlog );  
  } else { throw std::runtime_error("Unsupported persistent version of RingerRingsContainer");  }

}//end of create transient method
