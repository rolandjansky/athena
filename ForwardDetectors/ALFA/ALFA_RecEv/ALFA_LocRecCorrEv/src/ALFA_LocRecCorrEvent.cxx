/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"


// Default constructor needed by athenaroot/athenapool
ALFA_LocRecCorrEvent::ALFA_LocRecCorrEvent()
{
	m_iAlgoNum= 0;
	m_pot_num = 0;
	m_xBeam   = 0.0;
	m_xLHC    = 0.0;
	m_xPot    = 0.0;
	m_xStat   = 0.0;
	m_yBeam   = 0.0;
	m_yLHC    = 0.0;
	m_yPot    = 0.0;
	m_yStat   = 0.0;
	m_zLHC    = 0.0;
}

// destructor
ALFA_LocRecCorrEvent::~ALFA_LocRecCorrEvent() {}

ALFA_LocRecCorrEvent::ALFA_LocRecCorrEvent(int iAlgoNum, int pot_num, float xLHC, float yLHC, float zLHC, float xPot, float yPot, float xStat, float yStat, float xBeam, float yBeam):
	m_iAlgoNum(iAlgoNum), m_pot_num(pot_num), m_xLHC(xLHC), m_yLHC(yLHC), m_zLHC(zLHC), m_xPot(xPot), m_yPot(yPot), m_xStat(xStat), m_yStat(yStat), m_xBeam(xBeam), m_yBeam(yBeam)
{}
