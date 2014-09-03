/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBTriggerPatternUnit.h"

/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBTriggerPatternUnit::TBTriggerPatternUnit() 
  : m_triggerWord(0)
{
}



TBTriggerPatternUnit::~TBTriggerPatternUnit()
{ }

///////////////////////
// Data Manipulation //
///////////////////////


void TBTriggerPatternUnit::setTriggerWord(unsigned int triggerword)
{
   
  unsigned int mask = 0x00000001;
  m_triggerWord = triggerword;
  m_triggers.resize(16);
  for (unsigned int i=0; i<16; i++ )
    {
      m_triggers[i] =  m_triggerWord & mask;
      mask *= 2;
    }
}
