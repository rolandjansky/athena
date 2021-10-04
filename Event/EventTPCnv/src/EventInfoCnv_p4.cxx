/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "EventTPCnv/EventInfoCnv_p4.h"
#include "EventTPCnv/EventIDCnv_p1.h"
#include "EventTPCnv/EventTypeCnv_p3.h"
#include "EventTPCnv/TriggerInfoCnv_p2.h"


static const EventIDCnv_p1		idConv;
static const EventTypeCnv_p3		typeConv;
static const TriggerInfoCnv_p2	trigInfoCnv;


void EventInfoCnv_p4::transToPers(const EventInfo* trans, EventInfo_p4* pers, MsgStream &log) const {
    idConv.transToPers(trans->m_event_ID, &pers->m_event_ID, log);
    typeConv.transToPers(trans->m_event_type, &pers->m_event_type, log);
    if( trans->m_trigger_info ) {
        trigInfoCnv.transToPers(trans->m_trigger_info, &pers->m_trigger_info, log);
    }
    // Don't write out event flags if they are all == 0
    if (trans->m_event_flags.size()) {
        bool copy = false;
        for (unsigned int i = 0; i < trans->m_event_flags.size(); ++i) {
            if (trans->m_event_flags[i]) { copy = true; break; }
        }
        if (copy) pers->m_event_flags = trans->m_event_flags;
    }
}

void EventInfoCnv_p4::persToTrans(const EventInfo_p4* pers, EventInfo* trans, MsgStream &log) const {
    delete trans->m_event_ID;
    trans->m_event_ID     = idConv.createTransientConst(&pers->m_event_ID, log);
    delete trans->m_event_type;
    trans->m_event_type   = typeConv.createTransientConst(&pers->m_event_type, log);
    delete trans->m_trigger_info;
    trans->m_trigger_info = trigInfoCnv.createTransientConst(&pers->m_trigger_info, log);
    if (pers->m_event_flags.size()) trans->m_event_flags = pers->m_event_flags;
}

// work around the default constructor of EventInfo allocating memory
EventInfo* EventInfoCnv_p4::createTransientConst( const EventInfo_p4* persObj, MsgStream &log) const {
    std::unique_ptr<EventInfo> trans( new EventInfo(0,0,0) );
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

