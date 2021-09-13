/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/MergedEventInfo.h"
#include "EventInfo/EventID.h"

#include "EventTPCnv/MergedEventInfoCnv_p2.h"
#include "EventTPCnv/EventInfoCnv_p3.h"
#include "EventTPCnv/EventIDCnv_p2.h"


static const EventIDCnv_p2		idConv;
static const EventInfoCnv_p3		baseConv;

void MergedEventInfoCnv_p2::persToTrans(const MergedEventInfo_p2* pers, MergedEventInfo* trans, MsgStream &log) const {
   EventInfo base;
   baseConv.persToTrans(pers, &base, log);
   std::vector<unsigned int>::const_iterator i=pers->m_newEventIDdata.begin();
   EventID newid;
   idConv.persToTrans(i, &newid);
   *trans = MergedEventInfo (base,
                             newid.run_number(),
                             newid.event_number(),
                             newid.time_stamp());
}

void MergedEventInfoCnv_p2::transToPers(const MergedEventInfo* trans, MergedEventInfo_p2* pers, MsgStream &log) const {
	baseConv.transToPers(trans, pers, log); 
	idConv.transToPers(trans->event_ID(), pers->m_newEventIDdata);
}

