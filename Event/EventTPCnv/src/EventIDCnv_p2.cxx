/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventID.h"
#include "EventTPCnv/EventIDCnv_p2.h"


void EventIDCnv_p2::persToTrans(std::vector<unsigned int>::const_iterator &i, EventID* trans) const {
    trans->set_run_number           (*i); ++i;
    trans->set_event_number         (*i); ++i;
    trans->set_time_stamp           (*i); ++i;
    trans->set_time_stamp_ns_offset (*i); ++i; 
    trans->set_lumi_block           (*i); ++i;
    trans->set_bunch_crossing_id    (*i); ++i;
    EventID::number_type detector_mask0       = (*i); ++i;
    EventID::number_type detector_mask1       = (*i); ++i;
    trans->set_detector_mask (detector_mask0, detector_mask1);
    // std::cout<<"persToTrans of EventID \t"<<trans->m_event_number<<std::endl;
}

void EventIDCnv_p2::transToPers(const EventID* trans, std::vector<unsigned int>& pers) const {
    // Deprecated - writing out with _p1 RDS 2013/03

    pers.push_back(trans->run_number());
    pers.push_back(trans->event_number());
    pers.push_back(trans->time_stamp());
    pers.push_back(trans->time_stamp_ns_offset()); 
    pers.push_back(trans->lumi_block());
    pers.push_back(trans->bunch_crossing_id());
    pers.push_back(trans->detector_mask0());
    pers.push_back(trans->detector_mask1());
    // std::cout<<"transToPers of EventID \t"<<trans->m_event_number<<std::endl;
}
