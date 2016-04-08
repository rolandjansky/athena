/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTBEvent/LArTBEvent.h"

LArTBEvent::LArTBEvent(int eventNumber) 
  : m_eventNumber(eventNumber),
    m_burstNumber(-99), m_random(-99), m_tdcTime(-1),
    m_TDC_min(0),
    m_tdcAntiTime(-1),
    m_time(-1),
    m_hms_data(0),
    m_eventNumberInROD(-1), m_eventNumberInMiniROD(-1) {}

LArTBEvent::LArTBEvent(int eventNumber, int burstNumber, int random, 
		       int tdcTime, int TDC_min, int tdcAntiTime,
		       float time, int hms_data,
		       int eventNumberInROD, int eventNumberInMiniROD)
{
  m_eventNumber = eventNumber;
  m_burstNumber = burstNumber; 
  m_random      = random;
  m_tdcTime     = tdcTime; 
  m_TDC_min     = TDC_min;
  m_tdcAntiTime = tdcAntiTime; 
  m_time        = time; 
  m_hms_data    = hms_data;
  m_eventNumberInROD     = eventNumberInROD;
  m_eventNumberInMiniROD = eventNumberInMiniROD;
}
LArTBEvent::~LArTBEvent() {}

/** copy constructor */
LArTBEvent::LArTBEvent(const LArTBEvent& larTBEvent)
{
  m_eventNumber = larTBEvent.m_eventNumber;
  m_burstNumber = larTBEvent.m_burstNumber;
  m_random      = larTBEvent.m_random;
  m_tdcTime     = larTBEvent.m_tdcTime;
  m_TDC_min     = larTBEvent.m_TDC_min;
  m_tdcAntiTime = larTBEvent.m_tdcAntiTime;
  m_time        = larTBEvent.m_time;
  m_hms_data    = larTBEvent.m_hms_data;
  m_eventNumberInROD     = larTBEvent.m_eventNumberInROD;
  m_eventNumberInMiniROD = larTBEvent.m_eventNumberInMiniROD;
}

/** affectation operator */
LArTBEvent& LArTBEvent::operator = ( const LArTBEvent & larTBEvent) 
{
  if(this != &larTBEvent)
    {
      m_eventNumber = larTBEvent.m_eventNumber;
      m_burstNumber = larTBEvent.m_burstNumber;
      m_random      = larTBEvent.m_random;
      m_tdcTime     = larTBEvent.m_tdcTime;
      m_TDC_min     = larTBEvent.m_TDC_min;
      m_tdcAntiTime = larTBEvent.m_tdcAntiTime;
      m_time        = larTBEvent.m_time;
      m_hms_data    = larTBEvent.m_hms_data;
      m_eventNumberInROD     = larTBEvent.m_eventNumberInROD;
      m_eventNumberInMiniROD = larTBEvent.m_eventNumberInMiniROD;
    }
  return *this;
}
/** comparison operator: rely only on the run and event number. */
bool LArTBEvent::operator == (const LArTBEvent & larTBEvent) const {
  return ( m_eventNumber == larTBEvent.m_eventNumber);
}


