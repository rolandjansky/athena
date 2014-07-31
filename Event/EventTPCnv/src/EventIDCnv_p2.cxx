/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventID.h"
#undef private
#undef protected

#include "EventTPCnv/EventIDCnv_p2.h"


void EventIDCnv_p2::persToTrans(std::vector<unsigned int>::const_iterator &i, EventID* trans) {
    trans->m_run_number           = (*i); ++i;
    trans->m_event_number         = (*i); ++i;
    trans->m_time_stamp           = (*i); ++i;
    trans->m_time_stamp_ns_offset = (*i); ++i; 
    trans->m_lumiBlock            = (*i); ++i;
    trans->m_bunch_crossing_id    = (*i); ++i;
    trans->m_detector_mask0       = (*i); ++i;
    trans->m_detector_mask1       = (*i); ++i;
    // std::cout<<"persToTrans of EventID \t"<<trans->m_event_number<<std::endl;
}

void EventIDCnv_p2::transToPers(const EventID* /*trans*/, std::vector<unsigned int>& /*pers*/) {
    // Deprecated - writing out with _p1 RDS 2013/03

    // pers.push_back(trans->m_run_number);
    // pers.push_back(trans->m_event_number);
    // pers.push_back(trans->m_time_stamp);
    // pers.push_back(trans->m_time_stamp_ns_offset); 
    // pers.push_back(trans->m_lumiBlock);
    // pers.push_back(trans->m_bunch_crossing_id);
    // pers.push_back(trans->m_detector_mask0);
    // pers.push_back(trans->m_detector_mask1);
    // // std::cout<<"transToPers of EventID \t"<<trans->m_event_number<<std::endl;
}
