/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventID.h"
#undef private
#undef protected

#include "EventTPCnv/EventIDCnv_p1.h"

void EventIDCnv_p1::transToPers(const EventID* trans, EventID_p1* pers, MsgStream &) {
   pers->m_run_number            = trans->m_run_number;
   pers->m_event_number          = trans->m_event_number;
   pers->m_time_stamp            = trans->m_time_stamp;
   pers->m_time_stamp_ns_offset  = trans->m_time_stamp_ns_offset; 
   pers->m_lumiBlock             = trans->m_lumiBlock;
   pers->m_bunch_crossing_id     = trans->m_bunch_crossing_id;
   pers->m_detector_mask0        = trans->m_detector_mask0;
   pers->m_detector_mask1        = trans->m_detector_mask1;
   pers->m_detector_mask2        = trans->m_detector_mask2;
   pers->m_detector_mask3        = trans->m_detector_mask3;
}

void EventIDCnv_p1::persToTrans(const EventID_p1* pers, EventID* trans, MsgStream &) {
   trans->m_run_number           = pers->m_run_number;
   trans->m_event_number         = pers->m_event_number;
   trans->m_time_stamp           = pers->m_time_stamp;
   trans->m_time_stamp_ns_offset = pers->m_time_stamp_ns_offset; 
   trans->m_lumiBlock            = pers->m_lumiBlock;
   trans->m_bunch_crossing_id    = pers->m_bunch_crossing_id;
   trans->m_detector_mask0       = pers->m_detector_mask0;
   trans->m_detector_mask1       = pers->m_detector_mask1;
   trans->m_detector_mask2       = pers->m_detector_mask2;
   trans->m_detector_mask3       = pers->m_detector_mask3;
}

