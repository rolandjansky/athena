/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventType.h"
#include "EventTPCnv/EventTypeCnv_p3.h"

void EventTypeCnv_p3::transToPers(const EventType* trans, EventType_p3* pers, MsgStream &) const {
   pers->m_bit_mask          = trans->bit_mask();
   pers->m_user_type         = trans->user_type_raw();

   {
     unsigned int nwt = trans->n_mc_event_weights();
     pers->m_mc_event_weights.resize (nwt);
     for (unsigned int i=0; i < nwt; i++)
       pers->m_mc_event_weights[i]  = trans->mc_event_weight(i);
   }
   pers->m_mc_channel_number = trans->mc_channel_number();
   pers->m_mc_event_number   = trans->mc_event_number();
}

void EventTypeCnv_p3::persToTrans(const EventType_p3* pers, EventType* trans, MsgStream &) const {
   *trans = EventType();

    for (size_t i = 0; i < pers->m_bit_mask.size(); i++) {
      if (pers->m_bit_mask[i])
        trans->add_type (i);
    }

    size_t nwt = pers->m_mc_event_weights.size();
    for (size_t i = 0; i < nwt; i++) {
      trans->set_mc_event_weight (pers->m_mc_event_weights[i], i, nwt);
    }

    trans->set_user_type         (pers->m_user_type);
    trans->set_mc_channel_number (pers->m_mc_channel_number);
    trans->set_mc_event_number   (pers->m_mc_event_number);
}

EventType* EventTypeCnv_p3::createTransientConst (const EventType_p3* persObj, MsgStream& log) const
{
  auto trans = std::make_unique<EventType>();
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
