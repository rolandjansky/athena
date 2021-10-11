/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventType.h"
#include "EventTPCnv/EventTypeCnv_p1.h"

void EventTypeCnv_p1::transToPers(const EventType* trans, EventType_p1* pers, MsgStream &) const {
    // Deprecated - writing out with _p3 RDS 2013/10
    pers->m_user_type           = trans->user_type_raw();
    pers->m_bit_mask            = trans->bit_mask();
    pers->m_mc_event_weight     = 0;
    if (trans->n_mc_event_weights() > 0)
      pers->m_mc_event_weight     = trans->mc_event_weight(0);
}

void EventTypeCnv_p1::persToTrans(const EventType_p1* pers, EventType* trans, MsgStream &) const {
    *trans = EventType();
    trans->set_user_type         (pers->m_user_type);
    for (size_t i = 0; i < pers->m_bit_mask.size(); i++) {
      if (pers->m_bit_mask[i])
        trans->add_type (i);
    }
    trans->set_mc_event_weight ((float)pers->m_mc_event_weight, 0, 1);
}

EventType* EventTypeCnv_p1::createTransientConst (const EventType_p1* persObj, MsgStream& log) const
{
  auto trans = std::make_unique<EventType>();
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}

