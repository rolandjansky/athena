/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArFecLvTempDcs.h"

void
LArFecLvTempDcsOcem::print(MsgStream& log) const{
	log << endreq << " OCEM variables : "
	<< " Power On (" << m_powerOn << ") "
	<< " Power Out (" << m_powerOut << ") "
	<< " Interlock (" << m_interlock << ") "
	<< " overCurrent (" << m_overCurrent << ") "
	<< " overTemperature (" << m_overTemperature << ") ";
	if ( m_remote ) log << " remote control of PS ";
	else log << " local control of PS ";
	log 
	<< " PS Output Voltage (" << m_voltage << ") "
	<< " PS Output Current (" << m_current << ") "
	<< " On (" << m_on << ") "
	<< " Out (" << m_out << ") "
	<< " CAN Node State (" << m_state << ") "
	<< endreq;
}

void
LArFecLvTempDcsManifold::print(MsgStream& log) const{
	log << endreq << " Manifold Temperatures : ";
	for(std::vector<float>::const_iterator i=m_temps.begin();
		i != m_temps.end(); ++i)
	log << (*i) << " ";
	log << endreq;
}

void
LArFecLvTempDcsVolts::print(MsgStream& log) const{
	log << endreq << " Crate Voltages : ";
	for(std::vector<float>::const_iterator i=m_volts.begin();
		i != m_volts.end(); ++i)
	log << (*i) << " ";
	log << endreq;
}

void
LArFecLvTempDcsLvps::print(MsgStream& log) const{
	log << endreq << " LVPS : ";
	log << "AVG : "            << m_AVG         << " ";
	log << "AMG : "            << m_AMG         << " ";
	log << "PSOT : "           << m_PSOT        << " ";
	log << "PSOTOR : "         << m_PSOTOR      << " ";
	log << "PS_TEMPERATURE : " << m_temperature << " ";
	log << endreq;
}

void
LArFecLvTempDcs::print(MsgStream& log) const{
	log << endreq << " Crate ";
	log << m_crateName;
	log << " : " << endreq;
	m_OCEM.print(log);
	m_manifold.print(log);
	m_volts.print(log);
	m_lvps.print(log);
	log << "ELMB Status : " << m_ELMBStat << " ";
	log << "FEBMIN : " << m_febMin << " ";
	log << "FEBMAX : " << m_febMax << " ";
	// Still missing some variables
	log << endreq;
}
