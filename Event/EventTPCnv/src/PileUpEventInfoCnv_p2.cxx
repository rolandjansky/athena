/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/PileUpEventInfo.h"
#include "EventTPCnv/PileUpEventInfoCnv_p2.h"
#include "EventTPCnv/EventInfoCnv_p2.h"

static const EventInfoCnv_p2		evInfoConv;

namespace {
  const int MODULO(100000);
  const double MINTIME(-2000.0); //ns
}

void PileUpEventInfoCnv_p2::transToPers(const PileUpEventInfo* trans, PileUpEventInfo_p2* pers, MsgStream &log) {
   evInfoConv.transToPers(trans, pers, log); 

   PileUpEventInfo::SubEvent::const_iterator	sub_iter = trans->beginSubEvt();
   size_t	sub_ev_n = std::distance (sub_iter, trans->endSubEvt());
   pers->m_subEvents.resize(sub_ev_n);
   PileUpEventInfo_p2::SubEvVect_t::iterator	p_sub_iter = pers->m_subEvents.begin();
   while( sub_iter!=trans->endSubEvt() ) {
      short time = sub_iter->time();
      short index = sub_iter->index();
      p_sub_iter->m_timeIndex = (time - MINTIME) + MODULO * index;
      evInfoConv.transToPers(sub_iter->pSubEvt, &p_sub_iter->m_subEventInfo, log);
      ++p_sub_iter;  ++sub_iter;
   }
}

void PileUpEventInfoCnv_p2::persToTrans(const PileUpEventInfo_p2* pers, PileUpEventInfo* trans, MsgStream &log) 
{
   *trans = PileUpEventInfo();
   evInfoConv.persToTrans(pers, trans, log); 
   
   PileUpEventInfo_p2::SubEvVect_t::const_iterator	p_sub_iter = pers->m_subEvents.begin();
   while( p_sub_iter != pers->m_subEvents.end() ) {
     short index1 =  static_cast<short>( (p_sub_iter->m_timeIndex)*(1./MODULO) );
      unsigned long index = MODULO*index1;

      trans->addSubEvt (static_cast<short>( MINTIME+(p_sub_iter->m_timeIndex-index) ),
                        //index1
                        PileUpTimeEventIndex::Unknown,
                        std::unique_ptr<EventInfo>
                          (evInfoConv.createTransient(&p_sub_iter->m_subEventInfo, log)),
                        nullptr);
      ++p_sub_iter;
   }
}
