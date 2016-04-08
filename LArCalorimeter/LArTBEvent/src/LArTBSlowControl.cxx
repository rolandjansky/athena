/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArTBEvent/LArTBSlowControl.h"

LArTBSlowControl::LArTBSlowControl(int cryo_pressure, float ADC_temperature,
				   std::vector<int> FEB_temperature)
{    
  m_cryo_pressure   = cryo_pressure;
  m_ADC_temperature = ADC_temperature;
  m_FEB_temperature = FEB_temperature;
}

LArTBSlowControl::~LArTBSlowControl() { }


