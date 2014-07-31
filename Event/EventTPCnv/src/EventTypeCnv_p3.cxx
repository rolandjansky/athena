/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventType.h"
#undef private
#undef protected

#include "EventTPCnv/EventTypeCnv_p3.h"

void EventTypeCnv_p3::transToPers(const EventType* trans, EventType_p3* pers, MsgStream &) {
   pers->m_bit_mask          = trans->m_bit_mask;
   pers->m_user_type         = trans->m_user_type;
   pers->m_mc_event_weights  = trans->m_mc_event_weights;
   pers->m_mc_channel_number = trans->m_mc_channel_number;
   pers->m_mc_event_number   = trans->m_mc_event_number;
}

void EventTypeCnv_p3::persToTrans(const EventType_p3* pers, EventType* trans, MsgStream &) {
   trans->m_bit_mask          = pers->m_bit_mask;
   trans->m_user_type         = pers->m_user_type;
   trans->m_mc_event_weights  = pers->m_mc_event_weights;
   trans->m_mc_channel_number = pers->m_mc_channel_number;
   trans->m_mc_event_number   = pers->m_mc_event_number;
}
