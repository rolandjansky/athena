/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "PixelConditionsData/PixelDCSData.h"


std::ostream& operator<<(std::ostream& out, const PixelModuleDCSData& pmdscd)
{
  
  out << std::setw(2) << "Module:"             << std::setw(10) << pmdscd.getModuleID().get_compact()     << std::endl
      << std::setw(20) << "->Temperature:"     << std::setw(10) << pmdscd.getTemperature()                         << std::endl
      << std::setw(20) << "->HV         :"     << std::setw(10) << pmdscd.getHV()                                  << std::endl
      << std::setw(20) << "->FSM status :"     << std::setw(10) << pmdscd.getFSMStatus()                           << std::endl
      << std::setw(20) << "->FSM state  :"     << std::setw(10) << pmdscd.getFSMState()                            << std::endl;

  return out;

}

PixelModuleDCSData::PixelModuleDCSData(): m_ident(0),
					  m_temperature(-9999),
					  m_hightVoltage(-9999),
					  m_fsmStatus("NO_DATA"),
					  m_fsmState("NO_DATA")

{
}

PixelModuleDCSData::PixelModuleDCSData(const Identifier& ident): m_ident(ident),
								 m_temperature(-9999),
								 m_hightVoltage(-9999),
								 m_fsmStatus("NO_DATA"),
								 m_fsmState("NO_DATA")

{
}

PixelModuleDCSData::PixelModuleDCSData(Identifier& ident): m_ident(ident),
							   m_temperature(-9999),
							   m_hightVoltage(-9999),
							   m_fsmStatus("NO_DATA"),
							   m_fsmState("NO_DATA")

{
}

