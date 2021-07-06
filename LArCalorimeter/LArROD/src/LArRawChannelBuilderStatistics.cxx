/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderStatistics.h"

#include <iostream>

LArRawChannelBuilderStatistics::LArRawChannelBuilderStatistics(unsigned int nerr,
							       unsigned short bitpat):
  m_N_errors(nerr),
  m_bit_pattern(bitpat),
  m_s("")
{
  m_error_count.resize(m_N_errors);
  m_error_string.resize(m_N_errors);
  m_errors_this_event.resize(m_N_errors);
  m_errors_per_event.resize(m_N_errors);
  m_errorsSq_per_event.resize(m_N_errors);
  
  for( unsigned int i=0; i<m_N_errors; i++ )
    {
      m_error_count[i]=0;
      m_errors_this_event[i]=0;
      m_errors_per_event[i]=0;
      m_errorsSq_per_event[i]=0;
    }
  m_events=0;
  
  m_first=true;
}

void LArRawChannelBuilderStatistics::incrementErrorCount(unsigned int nerr)
{
  if(nerr<m_N_errors)
    {
      m_error_count[nerr]++;
      m_errors_this_event[nerr]++;
    }
}

ulonglong LArRawChannelBuilderStatistics::returnErrorCount(unsigned int nerr)
{
  if(nerr<m_N_errors)
    return m_error_count[nerr];
  return 0;
}

ulonglong LArRawChannelBuilderStatistics::returnErrorCountPerEvent(unsigned int nerr)
{
  if(nerr<m_N_errors)
    return m_errors_per_event[nerr];
  return 0;
}

ulonglong LArRawChannelBuilderStatistics::returnErrorCountSqPerEvent(unsigned int nerr)
{
  if(nerr<m_N_errors)
    return m_errorsSq_per_event[nerr];
  return 0;
}

unsigned long LArRawChannelBuilderStatistics::returnEvents()
{
  return m_events;
}

void LArRawChannelBuilderStatistics::setErrorString(unsigned int nerr, const std::string& s)
{
  if(nerr<m_N_errors)
    m_error_string[nerr]=s;
}
std::string LArRawChannelBuilderStatistics::returnErrorString(unsigned int nerr)
{
  if(nerr>=m_N_errors)
    return m_s;
  return m_error_string[nerr];
}
unsigned int LArRawChannelBuilderStatistics::returnMaxErrors()
{
  return m_N_errors;
}

unsigned int LArRawChannelBuilderStatistics::returnBitPattern()
{
  return m_bit_pattern;
}

void LArRawChannelBuilderStatistics::resetErrorcountPerEvent()
{
  for( unsigned int i=0; i<m_N_errors; i++ )
    {
      // errors per event
      m_errors_per_event[i]   += m_errors_this_event[i];
      m_errorsSq_per_event[i] += m_errors_this_event[i] * m_errors_this_event[i];
      
      m_errors_this_event[i]=0;
    }
  m_events++;
}
