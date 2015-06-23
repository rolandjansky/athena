/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/PileUpEventInfo.h"
#undef private
#undef protected


#include "EventTPCnv/PileUpEventInfoCnv_p1.h"
#include "EventTPCnv/EventInfoCnv_p1.h"

static	EventInfoCnv_p1		evInfoConv;

namespace {
  const int MODULO(100000);
  const double MINTIME(-2000.0); //ns
}

void PileUpEventInfoCnv_p1::transToPers(const PileUpEventInfo* trans, PileUpEventInfo_p1* pers, MsgStream &log) {
   evInfoConv.transToPers(trans, pers, log); 

   size_t	sub_ev_n = trans->m_subEvents.size();
   pers->m_subEvents.resize(sub_ev_n);
   PileUpEventInfo::SubEvent::const_iterator	sub_iter = trans->beginSubEvt();
   PileUpEventInfo_p1::SubEvVect_t::iterator	p_sub_iter = pers->m_subEvents.begin();
   while( sub_iter!=trans->endSubEvt() ) {
      short time = sub_iter->m_timeIndex.m_time;
      short index = sub_iter->m_timeIndex.m_index;
      p_sub_iter->m_timeIndex = (time - MINTIME) + MODULO * index;
      evInfoConv.transToPers(sub_iter->pSubEvt, &p_sub_iter->m_subEventInfo, log);
      p_sub_iter++;  sub_iter++;
   }
}

void PileUpEventInfoCnv_p1::persToTrans(const PileUpEventInfo_p1* pers, PileUpEventInfo* trans, MsgStream &log) 
{
   evInfoConv.persToTrans(pers, trans, log); 
   
   PileUpEventInfo_p1::SubEvVect_t::const_iterator	p_sub_iter = pers->m_subEvents.begin();
   trans->m_subEvents.clear();
   while( p_sub_iter != pers->m_subEvents.end() ) {
      PileUpEventInfo::SubEvent	sub_event;
      sub_event.m_timeIndex.m_index =  static_cast<short>( (p_sub_iter->m_timeIndex) * (1./MODULO) );
      unsigned long index = MODULO*sub_event.m_timeIndex.m_index;
      sub_event.m_timeIndex.m_time  = static_cast<short>( MINTIME+(p_sub_iter->m_timeIndex-index) );
      sub_event.m_timeIndex.m_type  = PileUpTimeEventIndex::Unknown;
      sub_event.pSubEvt = evInfoConv.createTransient(&p_sub_iter->m_subEventInfo, log);
      trans->m_subEvents.push_back( sub_event );
      p_sub_iter++;
   }
}
