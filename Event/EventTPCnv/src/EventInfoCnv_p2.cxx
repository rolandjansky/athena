/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventInfo.h"
#undef private
#undef protected

#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "EventTPCnv/EventInfoCnv_p2.h"
#include "EventTPCnv/EventIDCnv_p1.h"
#include "EventTPCnv/EventTypeCnv_p1.h"
#include "EventTPCnv/TriggerInfoCnv_p2.h"


static	EventIDCnv_p1		idConv;
static	EventTypeCnv_p1		typeConv;
static	TriggerInfoCnv_p2	trigInfoCnv;


void EventInfoCnv_p2::transToPers(const EventInfo* trans, EventInfo_p2* pers, MsgStream &log) {
    idConv.transToPers(trans->m_event_ID, &pers->m_event_ID, log);
    typeConv.transToPers(trans->m_event_type, &pers->m_event_type, log);
    if( trans->m_trigger_info ) {
        trigInfoCnv.transToPers(trans->m_trigger_info, &pers->m_trigger_info, log);
    }
    // Don't write out event flags if they are all == 0
    if (trans->m_event_flags.size()) {
        bool copy = false;
        for (unsigned int i = 0; i < trans->m_event_flags.size(); ++i) {
            if (trans->m_event_flags[1]) { copy = true; break; }
        }
        if (copy) pers->m_event_flags = trans->m_event_flags;
    }
}

void EventInfoCnv_p2::persToTrans(const EventInfo_p2* pers, EventInfo* trans, MsgStream &log)  {
    delete trans->m_event_ID;
    trans->m_event_ID     = idConv.createTransient(&pers->m_event_ID, log);
    delete trans->m_event_type;
    trans->m_event_type   = typeConv.createTransient(&pers->m_event_type, log);
    delete trans->m_trigger_info;
    trans->m_trigger_info = trigInfoCnv.createTransient(&pers->m_trigger_info, log);
    if (pers->m_event_flags.size()) trans->m_event_flags = pers->m_event_flags;
}

// work around the default constructor of EventInfo allocating memory
EventInfo* EventInfoCnv_p2::createTransient( const EventInfo_p2* persObj, MsgStream &log) {
    std::auto_ptr<EventInfo> trans( new EventInfo(0,0,0) );
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}
