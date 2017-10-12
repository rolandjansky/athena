/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamEventAthenaPool/src/ByteStreamMetadataContainerCnv.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadataContainerCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataContainerCnv.cxx,v 1.3 2009-03-18 17:48:03 gemmeren Exp $
 **/

#include "ByteStreamMetadataContainerCnv.h"
#include "ByteStreamEventTPCnv/ByteStreamMetadataCnv_p1.h"

//______________________________________________________________________________
ByteStreamMetadataContainer_PERS* ByteStreamMetadataContainerCnv::createPersistent(ByteStreamMetadataContainer* transObj) {
   MsgStream log(msgSvc(), "ByteStreamMetadataContainerConverter");
   ByteStreamMetadataContainerCnv_p1 tPconverter_p1;
   ByteStreamMetadataContainer_PERS *persObj = tPconverter_p1.createPersistent(transObj, log);
   log << MSG::DEBUG << "Success" << endmsg;
   return(persObj);
}
//______________________________________________________________________________
ByteStreamMetadataContainer* ByteStreamMetadataContainerCnv::createTransient() {
   static pool::Guid p1_guid("E0A5F063-F07D-4D2E-8D3B-49205CD712C1");
   if (compareClassGuid(p1_guid)) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr<ByteStreamMetadataContainer_p1> bsmdc_p1(poolReadObject<ByteStreamMetadataContainer_p1>());
      MsgStream log(msgSvc(), "ByteStreamMetadataContainerConverter");
      ByteStreamMetadataContainerCnv_p1 tPconverter_p1;
      return(tPconverter_p1.createTransient(bsmdc_p1.get(), log));
   }
   ByteStreamMetadataContainer* bsmdc = poolReadObject<ByteStreamMetadataContainer>();
   return(bsmdc);
}
