/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventID.h"
#include "EventTPCnv/EventIDCnv_p1.h"

void EventIDCnv_p1::transToPers(const EventID* trans, EventID_p1* pers, MsgStream &) const {
   pers->m_run_number            = trans->run_number();
   pers->m_event_number          = trans->event_number();
   pers->m_time_stamp            = trans->time_stamp();
   pers->m_time_stamp_ns_offset  = trans->time_stamp_ns_offset(); 
   pers->m_lumiBlock             = trans->lumi_block();
   pers->m_bunch_crossing_id     = trans->bunch_crossing_id();
   pers->m_detector_mask0        = trans->detector_mask0();
   pers->m_detector_mask1        = trans->detector_mask1();
   pers->m_detector_mask2        = trans->detector_mask2();
   pers->m_detector_mask3        = trans->detector_mask3();
}

void EventIDCnv_p1::persToTrans(const EventID_p1* pers, EventID* trans, MsgStream &) const {
   trans->set_run_number           (pers->m_run_number);
   trans->set_event_number         (pers->m_event_number);
   trans->set_time_stamp           (pers->m_time_stamp);
   trans->set_time_stamp_ns_offset (pers->m_time_stamp_ns_offset);
   trans->set_lumi_block           (pers->m_lumiBlock);
   trans->set_bunch_crossing_id    (pers->m_bunch_crossing_id);
   trans->set_detector_mask        (pers->m_detector_mask0,
                                    pers->m_detector_mask1,
                                    pers->m_detector_mask2,
                                    pers->m_detector_mask3);
}

EventID* EventIDCnv_p1::createTransientConst (const EventID_p1* persObj, MsgStream& log) const
{
  auto trans = std::make_unique<EventID>();
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}

