/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "EventTPCnv/EventInfoCnv_p1.h"
#include "EventTPCnv/EventIDCnv_p1.h"
#include "EventTPCnv/EventTypeCnv_p1.h"
#include "EventTPCnv/TriggerInfoCnv_p1.h"
#include <math.h>

static const EventIDCnv_p1		idConv;
static const EventTypeCnv_p1		typeConv;
static const TriggerInfoCnv_p1	trigInfoCnv;


void EventInfoCnv_p1::transToPers(const EventInfo* trans, EventInfo_p1* pers, MsgStream &log) const {
    idConv.transToPers(trans->event_ID(), &pers->m_event_ID, log);
    typeConv.transToPers(trans->event_type(), &pers->m_event_type, log);
    if( trans->trigger_info() ) {
      pers->m_trigger_info = trigInfoCnv.createPersistentConst(trans->trigger_info(), log);
    }
}

void EventInfoCnv_p1::persToTrans(const EventInfo_p1* pers, EventInfo* trans, MsgStream &log) const {
    trans->setEventID (idConv.createTransientConst(&pers->m_event_ID, log));
    EventType* event_type = typeConv.createTransientConst(&pers->m_event_type, log);
    if( pers->m_trigger_info) {
        trans->setTriggerInfo (trigInfoCnv.createTransientConst(pers->m_trigger_info, log));

        // Fill MC event weight in transient EventType from persistent
        // TriggerInfo. This is only needed for MC events: if the
        // weight is zero, and TriggerInfo exists - was stored at end
        // of eventFilterInfo

        static const double MIN_WEIGHT = -1.0e+6;
        static const double MAX_WEIGHT = +1.0e+6;
        static const double BIN_WIDTH( (MAX_WEIGHT - MIN_WEIGHT) / 
                                 std::numeric_limits<unsigned int>::max() );
        if (event_type->mc_event_weight() == 0 && 
            trans->trigger_info()->eventFilterInfo().size()) {
            double weight = (MIN_WEIGHT + BIN_WIDTH * (double(trans->trigger_info()->eventFilterInfo().back()) + 0.5));
            if (fabs(weight - 0.999775) < 0.0001) weight = 1.0;
            else if (fabs(weight + 0.999775) < 0.0001 ) weight = -1.0;
            event_type->set_mc_event_weight(weight);
            //trans->m_event_type->m_mc_event_weight = trans->m_trigger_info->eventFilterInfo().back();
        }
    }
    trans->setEventType (event_type);
}

// work around the default constructor of EventInfo allocating memory
EventInfo* EventInfoCnv_p1::createTransientConst( const EventInfo_p1* persObj, MsgStream &log) const {
    std::unique_ptr<EventInfo> trans( new EventInfo(0,0,0) );
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}
