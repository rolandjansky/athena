/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PileUpEventInfo.cxx
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: PileUpEventInfo.cxx,v 1.9 2008-04-23 20:26:04 calaf Exp $
 */
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventID.h"
#include "EventInfo/PileUpEventInfo.h"

PileUpEventInfo::PileUpEventInfo() : 
  EventInfo() 
{}    

PileUpEventInfo::PileUpEventInfo(EventID* id, EventType* type) :
  EventInfo(id, type) 
{} 

PileUpEventInfo::PileUpEventInfo(EventID* id, EventType* type,
				 TriggerInfo* trig_info) :
  EventInfo(id, type, trig_info) 
{} 

PileUpEventInfo::~PileUpEventInfo() 
{} 

PileUpEventInfo::SubEvent::SubEvent() 
    :
  m_timeIndex(0), //FIXME why 0?
  pSubEvt(0), pSubEvtSG(0)

{}

PileUpEventInfo::SubEvent::SubEvent(const SubEvent& rhs) :
  m_timeIndex(rhs.m_timeIndex), 
  pSubEvt(0==rhs.pSubEvt ? 0 :new EventInfo(*rhs.pSubEvt)), 
  pSubEvtSG(rhs.pSubEvtSG)

{}
PileUpEventInfo::SubEvent::~SubEvent() 
{ 
  delete pSubEvt; 
}

PileUpEventInfo::SubEvent::SubEvent(time_type t, 
				    const EventInfo* pse, 
				    StoreGateSvc* psg) :
  m_timeIndex(t),
  pSubEvt(0==pse ? 0 : new EventInfo(*pse)), pSubEvtSG(psg)
{}

PileUpEventInfo::SubEvent::SubEvent(time_type t, index_type index,
				    const EventInfo* pse, 
				    StoreGateSvc* psg) :
  m_timeIndex(t, index), 
  pSubEvt(0==pse ? 0 : new EventInfo(*pse)), pSubEvtSG(psg)
{}
PileUpEventInfo::SubEvent::SubEvent(time_type t, index_type index,
				    PileUpTimeEventIndex::PileUpType typ,
				    const EventInfo* pse, 
				    StoreGateSvc* psg) :
  m_timeIndex(t, index, typ),
  pSubEvt(0==pse ? 0 : new EventInfo(*pse)), pSubEvtSG(psg)
{}
PileUpEventInfo::SubEvent::SubEvent(time_type t, unsigned int BCID,
				    index_type index,
				    PileUpTimeEventIndex::PileUpType typ,
				    const EventInfo& rse, 
				    StoreGateSvc* psg) :
  m_timeIndex(t, index, typ),
  pSubEvt(new EventInfo(rse)),
  pSubEvtSG(psg)
{
  pSubEvt->event_ID()->set_bunch_crossing_id(BCID);
}

unsigned int 
PileUpEventInfo::SubEvent::BCID() const {
  return (0==pSubEvt ? 0 :  pSubEvt->event_ID()->bunch_crossing_id());
}
