/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/MergedEventInfo.h"
#undef private
#undef protected

#include "EventInfo/EventID.h"

#include "EventTPCnv/MergedEventInfoCnv_p2.h"
#include "EventTPCnv/EventInfoCnv_p3.h"
#include "EventTPCnv/EventIDCnv_p2.h"


static	EventIDCnv_p2		idConv;
static	EventInfoCnv_p3		baseConv;

void MergedEventInfoCnv_p2::persToTrans(const MergedEventInfo_p2* pers, MergedEventInfo* trans, MsgStream &log) {
	baseConv.persToTrans(pers, trans, log);
	std::vector<unsigned int>::const_iterator i=pers->m_newEventIDdata.begin();
	idConv.persToTrans(i, &trans->m_newEventID);
}

void MergedEventInfoCnv_p2::transToPers(const MergedEventInfo* trans, MergedEventInfo_p2* pers, MsgStream &log) {
	baseConv.transToPers(trans, pers, log); 
	idConv.transToPers(&trans->m_newEventID, pers->m_newEventIDdata);
}

