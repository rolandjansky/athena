/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/MergedEventInfo.h"
#undef private
#undef protected

#include "EventInfo/EventID.h"

#include "EventTPCnv/MergedEventInfoCnv_p1.h"
#include "EventTPCnv/EventInfoCnv_p1.h"
#include "EventTPCnv/EventIDCnv_p1.h"


static	EventIDCnv_p1		idConv;
static	EventInfoCnv_p1		baseConv;


void MergedEventInfoCnv_p1::transToPers(const MergedEventInfo* trans, MergedEventInfo_p1* pers, MsgStream &log) {
   baseConv.transToPers(trans, pers, log); 
   idConv.transToPers(&trans->m_newEventID, &pers->m_newEventID, log);
}

void MergedEventInfoCnv_p1::persToTrans(const MergedEventInfo_p1* pers, MergedEventInfo* trans, MsgStream &log) {
   baseConv.persToTrans(pers, trans, log);
   idConv.persToTrans(&pers->m_newEventID, &trans->m_newEventID, log);
}
