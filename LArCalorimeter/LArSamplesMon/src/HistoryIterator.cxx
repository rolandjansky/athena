/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/HistoryIterator.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/Data.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


HistoryIterator::HistoryIterator(const Interface& interface, unsigned int pos, 
                                             double eMin, double adcMaxMin) 
  : m_interface(&interface), m_pos(pos),
    m_eMin(eMin), m_adcMaxMin(adcMaxMin)
{
  if (!isValid()) next();
}


const History* HistoryIterator::history() const
{
 return m_interface->cellHistory(m_pos);
}


bool HistoryIterator::isValid() const
{
  if (m_pos >= Definitions::nChannels) return false;
  unsigned int size = m_interface->historySize(m_pos);
  if (size == 0) return false;    
  if (m_eMin < 0 && m_adcMaxMin < 0) return true;
  
  const History* history = m_interface->cellHistory(m_pos);
  bool found = false; 
  for (unsigned int i = 0; i < history->nData(); i++) {
    if (history->data(i)->energy() > m_eMin && history->data(i)->adcMax() > m_adcMaxMin) {
      found = true;
      break;
    }
  }
  return found;
}


const History* HistoryIterator::next()
{
  m_pos++;
  
  for (; m_pos < m_interface->end(); m_pos++)
    if (isValid()) break;
   
  return history();
}


HistoryIterator* HistoryIterator::operator++()
{
  next();
  return this;
}
