/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/PileUpEventInfo.h"
#include "EventTPCnv/PileUpEventInfoCnv_p4.h"
#include "EventTPCnv/EventInfoCnv_p3.h"

static const EventInfoCnv_p3		evInfoConv;

void PileUpEventInfoCnv_p4::transToPers(const PileUpEventInfo* trans, PileUpEventInfo_p4* pers, MsgStream &log) {
   evInfoConv.transToPers(trans, pers, log); 

   PileUpEventInfo::SubEvent::const_iterator	sub_iter = trans->beginSubEvt();
   size_t	sub_ev_n = std::distance (sub_iter, trans->endSubEvt());
   pers->m_subEvents.resize(sub_ev_n);
   PileUpEventInfo_p4::SubEvVect_t::iterator	p_sub_iter = pers->m_subEvents.begin();
   while( sub_iter!=trans->endSubEvt() ) {
      p_sub_iter->m_time  = sub_iter->time();
      p_sub_iter->m_index = sub_iter->index();
      p_sub_iter->m_type  = static_cast<short>(sub_iter->type());
      evInfoConv.transToPers(sub_iter->pSubEvt, &p_sub_iter->m_subEventInfo, log);
      ++p_sub_iter;  ++sub_iter;
   }
}

void PileUpEventInfoCnv_p4::persToTrans(const PileUpEventInfo_p4* pers, PileUpEventInfo* trans, MsgStream &log) 
{
   *trans = PileUpEventInfo();
   evInfoConv.persToTrans(pers, trans, log); 
   
   PileUpEventInfo_p4::SubEvVect_t::const_iterator	p_sub_iter = pers->m_subEvents.begin();
   while( p_sub_iter != pers->m_subEvents.end() ) {
      trans->addSubEvt (p_sub_iter->m_time,
                        //p_sub_iter->m_index,
                        static_cast<PileUpTimeEventIndex::PileUpType>(p_sub_iter->m_type),
                        std::unique_ptr<EventInfo>
                          (evInfoConv.createTransient(&p_sub_iter->m_subEventInfo, log)),
                        nullptr);
      ++p_sub_iter;
   }
}
