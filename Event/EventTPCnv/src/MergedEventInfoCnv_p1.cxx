/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/MergedEventInfo.h"
#include "EventInfo/EventID.h"

#include "EventTPCnv/MergedEventInfoCnv_p1.h"
#include "EventTPCnv/EventInfoCnv_p1.h"
#include "EventTPCnv/EventIDCnv_p1.h"


static const EventIDCnv_p1		idConv;
static const EventInfoCnv_p1		baseConv;


void MergedEventInfoCnv_p1::transToPers(const MergedEventInfo* trans, MergedEventInfo_p1* pers, MsgStream &log) const {
   baseConv.transToPers(trans, pers, log); 
   idConv.transToPers(trans->event_ID(), &pers->m_newEventID, log);
}

void MergedEventInfoCnv_p1::persToTrans(const MergedEventInfo_p1* pers, MergedEventInfo* trans, MsgStream &log) const {
   EventInfo base;
   baseConv.persToTrans(pers, &base, log);
   EventID newid;
   idConv.persToTrans(&pers->m_newEventID, &newid, log);
   *trans = MergedEventInfo (base,
                             newid.run_number(),
                             newid.event_number(),
                             newid.time_stamp());
}
