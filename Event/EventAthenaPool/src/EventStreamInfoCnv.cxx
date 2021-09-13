/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventStreamInfoCnv.cxx
 *  @brief This file contains the implementation for the EventStreamInfoCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv.cxx,v 1.3 2009-03-18 17:48:03 gemmeren Exp $
 **/

#include "EventStreamInfoCnv.h"
#include "EventTPCnv/EventStreamInfo_p1.h"
#include "EventTPCnv/EventStreamInfo_p2.h"
#include "EventTPCnv/EventStreamInfo_p3.h"
#include "EventTPCnv/EventStreamInfoCnv_p1.h"
#include "EventTPCnv/EventStreamInfoCnv_p2.h"
#include "EventTPCnv/EventStreamInfoCnv_p3.h"
#include "EventTPCnv/EventInfoCnv_p1.h"

//______________________________________________________________________________
EventStreamInfo_PERS* EventStreamInfoCnv::createPersistent(EventStreamInfo* transObj) {
   MsgStream log(msgSvc(), "EventStreamInfoConverter");
   EventStreamInfoCnv_p3 tPconverter_p3;
   EventStreamInfo_PERS *persObj = tPconverter_p3.createPersistent(transObj, log);
   log << MSG::DEBUG << "Success" << endmsg;
   return(persObj);
}
//______________________________________________________________________________
EventStreamInfo* EventStreamInfoCnv::createTransient() {
   static const pool::Guid p3_guid("11DF1B8C-0DEE-4687-80D7-E74B520ACBB4");
   static const pool::Guid p2_guid("9AD4889D-C8EB-4F3B-9920-D37FF092CAB6");
   static const pool::Guid p1_guid("9998C282-403B-4EA5-A795-C8C9BA9909C3");
   if (compareClassGuid(p3_guid)) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<EventStreamInfo_p3> esi_p3(poolReadObject<EventStreamInfo_p3>());
      MsgStream log(msgSvc(), "EventStreamInfoConverter");
      EventStreamInfoCnv_p3 tPconverter_p3;
      return(tPconverter_p3.createTransient(esi_p3.get(), log));
   } else if (compareClassGuid(p2_guid)) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<EventStreamInfo_p2> esi_p2(poolReadObject<EventStreamInfo_p2>());
      MsgStream log(msgSvc(), "EventStreamInfoConverter");
      EventStreamInfoCnv_p2 tPconverter_p2;
      return(tPconverter_p2.createTransient(esi_p2.get(), log));
   } else if (compareClassGuid(p1_guid)) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<EventStreamInfo_p1> esi_p1(poolReadObject<EventStreamInfo_p1>());
      MsgStream log(msgSvc(), "EventStreamInfoConverter");
      EventStreamInfoCnv_p1 tPconverter_p1;
      return(tPconverter_p1.createTransient(esi_p1.get(), log));
   }
   EventStreamInfo* esi = poolReadObject<EventStreamInfo>();
   return(esi);
}
