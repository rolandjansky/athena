/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_RawEv/versions/AFP_ToFRawData_v2.h"


AFP_ToFRawData_v2::AFP_ToFRawData_v2():
  AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >("MessageSvc"),"AFP_ToFRawData_v2")
{}


AFP_ToFRawData_v2::AFP_ToFRawData_v2(const AFP_ToFRawData_v2& copy):
  AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >("MessageSvc"),"AFP_ToFRawData_v2")
{
  m_header=copy.m_header;
  m_edge=copy.m_edge;
  m_channel=copy.m_channel;
  m_time_or_delayedTrigger=copy.m_time_or_delayedTrigger;
  m_pulseLength_or_triggerPattern=copy.m_pulseLength_or_triggerPattern;
}


uint32_t AFP_ToFRawData_v2::time() const 
{
  if(this->isTrigger())
  {
    ATH_MSG_WARNING("asking for time but this is a trigger word, returning 0");
    return 0;
  }
  return m_time_or_delayedTrigger;
}


void AFP_ToFRawData_v2::setTime(const uint32_t time) 
{
  if(this->isTrigger())
  {
    ATH_MSG_WARNING("trying to set time but this is a trigger word, not setting anything");
    return;
  }
  m_time_or_delayedTrigger = time;
}


uint32_t AFP_ToFRawData_v2::pulseLength() const 
{
  if(this->isTrigger())
  {
    ATH_MSG_WARNING("asking for pulseLength but this is a trigger word, returning 0");
    return 0;
  }
  return m_pulseLength_or_triggerPattern;
}


void AFP_ToFRawData_v2::setPulseLength(const uint32_t pulseLength) 
{
  if(this->isTrigger())
  {
    ATH_MSG_WARNING("trying to set pulseLength but this is a trigger word, not setting anything");
    return;
  }
  m_pulseLength_or_triggerPattern = pulseLength;
}


uint16_t AFP_ToFRawData_v2::delayedTrigger() const 
{
  if(!this->isTrigger())
  {
    ATH_MSG_WARNING("asking for delayedTrigger but this is not a trigger word, returning 0");
    return 0;
  }
  return m_time_or_delayedTrigger;
}


void AFP_ToFRawData_v2::setDelayedTrigger(const uint32_t delayedTrigger) 
{
  if(!this->isTrigger())
  {
    ATH_MSG_WARNING("trying to set delayedTrigger but this is not a trigger word, not setting anything");
    return;
  }
  m_time_or_delayedTrigger = delayedTrigger;
}


uint32_t AFP_ToFRawData_v2::triggerPattern() const 
{
  if(!this->isTrigger())
  {
    ATH_MSG_WARNING("asking for triggerPattern but this is not a trigger word, returning 0");
    return 0;
  }
  return m_pulseLength_or_triggerPattern;
}


void AFP_ToFRawData_v2::setTriggerPattern(const uint32_t triggerPattern) 
{
  if(!this->isTrigger())
  {
    ATH_MSG_WARNING("trying to set triggerPattern but this is not a trigger word, not setting anything");
    return;
  }
  m_pulseLength_or_triggerPattern = triggerPattern;
}

