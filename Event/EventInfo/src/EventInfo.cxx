/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfo/src/EventInfo.cxx
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventInfo.cxx,v 1.14 2009-03-30 17:24:38 schaffer Exp $
 */
//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

EventInfo::EventInfo()
    :
    m_event_ID	  (new EventID(0,0)),
    m_event_type  (new EventType()),
    m_trigger_info(new TriggerInfo()),
    m_event_flags(std::vector<unsigned int>(nDets, 0))
{}

EventInfo::EventInfo(EventID* id, EventType* type)
    :
    m_event_ID	(id),
    m_event_type(type),
    m_trigger_info(new TriggerInfo()),
    m_event_flags(std::vector<unsigned int>(nDets, 0))
{}

EventInfo::EventInfo(EventID* id, EventType* type, TriggerInfo* trig_info)
    :
    m_event_ID	(id),
    m_event_type(type),
    m_trigger_info(trig_info),
    m_event_flags(std::vector<unsigned int>(nDets, 0))
{}

EventInfo::EventInfo(const EventInfo& e)
    :
    m_event_ID( e.m_event_ID ?
		new EventID( *(e.m_event_ID) ) :
		new EventID(0,0) ),
    m_event_type( e.m_event_type ?
		  new EventType( *(e.m_event_type) ):
		  new EventType() ),
    m_trigger_info( e.m_trigger_info?
		    new TriggerInfo(*(e.m_trigger_info) ) :
		    new TriggerInfo() ),
    m_event_flags(e.m_event_flags)
{}


EventInfo& EventInfo::operator=(const EventInfo& e)
{
  if (this != &e) {
    delete m_event_ID;
    m_event_ID = ( e.m_event_ID ?
                   new EventID( *(e.m_event_ID) ) :
                   new EventID(0,0) );
      
    delete m_event_type;
    m_event_type = ( e.m_event_type ?
                     new EventType( *(e.m_event_type) ):
                     new EventType() );

    delete m_trigger_info;
    m_trigger_info = ( e.m_trigger_info?
                       new TriggerInfo(*(e.m_trigger_info) ) :
                       new TriggerInfo() );

    m_event_flags = e.m_event_flags;
  }
  return *this;
}


#if __cplusplus > 201100
EventInfo::EventInfo(EventInfo&& e)
  : m_event_ID (e.m_event_ID),
    m_event_type (e.m_event_type),
    m_trigger_info (e.m_trigger_info),
    m_event_flags (std::move(e.m_event_flags))
{
  e.m_event_ID = 0;
  e.m_event_type = 0;
  e.m_trigger_info = 0;
}


EventInfo& EventInfo::operator=(EventInfo&& e)
{
  if (this != &e) {
    delete m_event_ID;
    m_event_ID = e.m_event_ID;
    e.m_event_ID = 0;
      
    delete m_event_type;
    m_event_type = e.m_event_type;
    e.m_event_type = 0;

    delete m_trigger_info;
    m_trigger_info = e.m_trigger_info;
    e.m_trigger_info = 0;

    m_event_flags = std::move (e.m_event_flags);
  }
  return *this;
}
#endif


EventInfo::~EventInfo()
{
    delete m_event_ID;
    delete m_event_type;
    delete m_trigger_info;
}

void 
EventInfo::setEventID(EventID* pid) { 
  delete m_event_ID; m_event_ID = pid; 
}

void 
EventInfo::setEventType(EventType* pType) { 
  delete m_event_type; m_event_type = pType; 
}


void
EventInfo::setTriggerInfo(TriggerInfo* pTrig) {
  delete m_trigger_info; m_trigger_info = pTrig;
}


bool            
EventInfo::setEventFlags(EventFlagSubDet subDet, unsigned int flags) 
{
    // If event_flags has been cleared on input, reset size to just
    // before lumi
    if (m_event_flags.size() < (unsigned int)Lumi) m_event_flags.resize(Lumi,0);
    if ((unsigned)subDet < m_event_flags.size()) {
        // Combine potentially-existing error states with new flags
        m_event_flags[subDet] = (EF_ERROR_BITS & m_event_flags[subDet]) | (EF_BITS & flags);
        return (true);
    }
    return (false);
}


bool       
EventInfo::setEventFlagBit(EventFlagSubDet subDet, unsigned char bit, bool set)
{
    // If event_flags has been cleared on input, reset size to just
    // before lumi
    if (m_event_flags.size() < (unsigned int)Lumi) m_event_flags.resize(Lumi,0);
    if ((unsigned)subDet < m_event_flags.size() && bit < EF_ERROR_SHIFT) {
        if (set) {
            m_event_flags[subDet] = (EF_ERROR_BITS & m_event_flags[subDet]) |
                                    ((1u << bit) | m_event_flags[subDet]);
        }
        else {
            unsigned int mask = EF_BITS ^ (1u << bit);
            m_event_flags[subDet] = (EF_ERROR_BITS & m_event_flags[subDet]) | 
                                    (mask & m_event_flags[subDet]);
        }
        return (true);
    }
    return (false);
}

bool       
EventInfo::setErrorState(EventFlagSubDet subDet, EventFlagErrorState errorState)
{
    // If event_flags has been cleared on input, reset size to just
    // before lumi
    if (m_event_flags.size() < (unsigned int)Lumi) m_event_flags.resize(Lumi,0);
    // check that subdet index is in range and that we have a valid error state
    if ((unsigned)subDet < m_event_flags.size() &&
        (errorState == NotSet || errorState == Warning || errorState == Error)) {
        // Combine potentially-existing flags with new error states
        m_event_flags[subDet] = (unsigned int)((EF_BITS & m_event_flags[subDet]) | ((unsigned int)errorState << EF_ERROR_SHIFT));
        return (true);
    }
    return (false);
}

    ///  actual interactions per crossing for the current BCID - for in-time pile-up
void
EventInfo::setActualInteractionsPerCrossing(float interactions)
{
    // pack interaction into 16 bits saving the precision to the 0.01
    // level
    unsigned int ivalue = (unsigned int)(interactions*100.f);
    if (ivalue > ACTUAL_INTERACTS_BITS) {
        std::cout << "EventInfo::setActualInteractionsPerCrossing ***  ERROR ***: trying to store too large of value: interactions/ivalue/max value: " << interactions << " " << ivalue << " " << ACTUAL_INTERACTS_BITS << std::endl;
    }
    ivalue &= ACTUAL_INTERACTS_BITS;
    // If event_flags has been cleared on input, reset size AND INCLUDE lumi
    if (m_event_flags.size() < nDets) m_event_flags.resize(nDets, 0);
    m_event_flags[Lumi] |= ivalue; // store lower 16 bits
}


void
EventInfo::setAverageInteractionsPerCrossing(float interactions)
{
    // pack interaction into 16 bits saving the precision to the 0.01
    // level
    unsigned int ivalue = (unsigned int)(interactions*100.f);
    if (ivalue > ACTUAL_INTERACTS_BITS) {
        std::cout << "EventInfo::setAverageInteractionsPerCrossing ***  ERROR ***: trying to store too large of value: interactions/ivalue/max value: " << interactions << " " << ivalue << " " << ACTUAL_INTERACTS_BITS << std::endl;
    }
    ivalue = (ivalue << AVE_INTERACTS_SHIFT) & AVE_INTERACTS_BITS;
    // If event_flags has been cleared on input, reset size AND INCLUDE lumi
    if (m_event_flags.size() < nDets) m_event_flags.resize(nDets, 0);
    m_event_flags[Lumi] |= ivalue; // store upper 16 bits
}
