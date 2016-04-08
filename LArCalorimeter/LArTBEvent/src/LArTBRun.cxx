/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArTBEvent/LArTBRun.h"

LArTBRun::LArTBRun( int type, int runNo, int asynchronous, float temperature)
  : m_particleID(0),
    m_ebeam(0),
    m_cryoX(0),
    m_tableY(0)
{    
  m_type = type;
  m_runNumber = runNo;
  m_asynchronous = asynchronous;
  m_temperature = temperature;
}

LArTBRun::LArTBRun( int type, int runNo, int asynchronous, float temperature,
		    short particleID, float ebeam, float cryoX, float tableY )
{    
  m_type = type;
  m_runNumber = runNo;
  m_asynchronous = asynchronous;
  m_temperature = temperature;
  m_particleID = particleID;
  m_ebeam = ebeam;
  m_cryoX  = cryoX;
  m_tableY = tableY;

}

LArTBRun::~LArTBRun() { }

