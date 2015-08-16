/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloRec/TrigCaloQuality.h"


TrigCaloQuality::TrigCaloQuality(): m_error(0),
				    m_prescale(0),
				    m_persistencyFlag(false) {}

TrigCaloQuality::TrigCaloQuality(TrigCaloQuality & quality): m_error(quality.m_error),
							     m_prescale(0),
							     m_persistencyFlag(false) {}

TrigCaloQuality::~TrigCaloQuality() {}

void TrigCaloQuality::setError(uint32_t error) { 
  m_error=error; 
}

uint32_t TrigCaloQuality::getError() { 
  return m_error; 
}

void TrigCaloQuality::setPrescale(unsigned int prescale) { 
  m_prescale = prescale;
}

unsigned int TrigCaloQuality::getPrescale() { 
  return m_prescale; 
}

void TrigCaloQuality::setPersistencyFlag(bool pers) { 
  m_persistencyFlag = pers;
}

bool TrigCaloQuality::getPersistencyFlag() { 
  return m_persistencyFlag;
}



