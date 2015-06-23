/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/PileUpEventInfo.h"
#undef private
#undef protected


#include "EventTPCnv/PileUpEventInfoCnv_p4.h"
#include "EventTPCnv/EventInfoCnv_p3.h"

static	EventInfoCnv_p3		evInfoConv;

void PileUpEventInfoCnv_p4::transToPers(const PileUpEventInfo* trans, PileUpEventInfo_p4* pers, MsgStream &log) {
   evInfoConv.transToPers(trans, pers, log); 

   size_t	sub_ev_n = trans->m_subEvents.size();
   pers->m_subEvents.resize(sub_ev_n);
   PileUpEventInfo::SubEvent::const_iterator	sub_iter = trans->beginSubEvt();
   PileUpEventInfo_p4::SubEvVect_t::iterator	p_sub_iter = pers->m_subEvents.begin();
   while( sub_iter!=trans->endSubEvt() ) {
      p_sub_iter->m_time  = sub_iter->m_timeIndex.m_time;
      p_sub_iter->m_index = sub_iter->m_timeIndex.m_index;
      p_sub_iter->m_type  = static_cast<short>(sub_iter->m_timeIndex.m_type);
      evInfoConv.transToPers(sub_iter->pSubEvt, &p_sub_iter->m_subEventInfo, log);
      p_sub_iter++;  sub_iter++;
   }
}

void PileUpEventInfoCnv_p4::persToTrans(const PileUpEventInfo_p4* pers, PileUpEventInfo* trans, MsgStream &log) 
{
   evInfoConv.persToTrans(pers, trans, log); 
   
   PileUpEventInfo_p4::SubEvVect_t::const_iterator	p_sub_iter = pers->m_subEvents.begin();
   trans->m_subEvents.clear();
   while( p_sub_iter != pers->m_subEvents.end() ) {
      PileUpEventInfo::SubEvent	sub_event;
      sub_event.m_timeIndex.m_time  = p_sub_iter->m_time;
      sub_event.m_timeIndex.m_index = p_sub_iter->m_index;
      sub_event.m_timeIndex.m_type  = static_cast<PileUpTimeEventIndex::PileUpType>(p_sub_iter->m_type);
      sub_event.pSubEvt = evInfoConv.createTransient(&p_sub_iter->m_subEventInfo, log);
      trans->m_subEvents.push_back( sub_event );
      p_sub_iter++;
   }
}
