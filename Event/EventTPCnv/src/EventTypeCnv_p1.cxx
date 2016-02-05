/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventType.h"
#undef private
#undef protected

#include "EventTPCnv/EventTypeCnv_p1.h"

void EventTypeCnv_p1::transToPers(const EventType* /*trans*/, EventType_p1* /*pers*/, MsgStream &) {
    // Deprecated - writing out with _p3 RDS 2013/10
    // pers->m_user_type           = trans->m_user_type;
    // pers->m_bit_mask            = trans->m_bit_mask;
    // pers->m_mc_event_weight     = trans->m_mc_event_weights[0];
}

void EventTypeCnv_p1::persToTrans(const EventType_p1* pers, EventType* trans, MsgStream &) {
    trans->m_user_type           = pers->m_user_type;
    trans->m_bit_mask            = pers->m_bit_mask;
    trans->m_mc_event_weights[0] = (float)pers->m_mc_event_weight;
}
