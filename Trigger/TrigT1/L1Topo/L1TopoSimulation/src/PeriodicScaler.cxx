/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : PeriodicScaler
 *
 * @brief periodic prescale/FA engine
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold <Tomasz.Bold@cern.ch> - UST-AGH changed to float prescale factor (enjoy understanding how it works)
 *
 * File and Version Information:
 * $Id: PeriodicScaler.cxx,v 1.3 2008-05-06 09:19:19 tbold Exp $
 **********************************************************************************/

#include "./PeriodicScaler.h"


LVL1::PeriodicScaler::PeriodicScaler() {
  reset();
}

bool LVL1::PeriodicScaler::decision(float factor) {
  m_counter++;
  unsigned N = static_cast<unsigned>(m_counter/factor);
  if ( m_last != N ) {
    m_last = N;
    return true;
  } 
  return false;
}

void LVL1::PeriodicScaler::reset() {
  m_counter = 0;
  m_last = 0;
}


LVL1::PeriodicScalerTake1st::PeriodicScalerTake1st() {
  reset();
}

bool LVL1::PeriodicScalerTake1st::decision(float factor) {
  m_counter++;
  unsigned offset = static_cast<unsigned>(factor)-1;
  unsigned N = static_cast<unsigned>((m_counter+offset)/factor);
  if ( m_last != N ) {
    m_last = N;
    return true;
  } 
  return false;
}

