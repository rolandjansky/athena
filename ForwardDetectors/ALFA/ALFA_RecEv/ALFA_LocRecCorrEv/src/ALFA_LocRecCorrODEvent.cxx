/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvent.h"


// Default constructor needed by athenaroot/athenapool
ALFA_LocRecCorrODEvent::ALFA_LocRecCorrODEvent()
{
	m_iAlgoNum= 0;
	m_pot_num = 0;
	m_side    = 0;
	m_yBeam   = 0.0;
	m_yLHC    = 0.0;
	m_yPot    = 0.0;
	m_yStat   = 0.0;
	m_zLHC    = 0.0;
}

// destructor
ALFA_LocRecCorrODEvent::~ALFA_LocRecCorrODEvent() {}

ALFA_LocRecCorrODEvent::ALFA_LocRecCorrODEvent(int iAlgoNum, int pot_num, int side, float yLHC, float zLHC, float yPot, float yStat, float yBeam):
	m_iAlgoNum(iAlgoNum), m_pot_num(pot_num), m_side(side), m_yLHC(yLHC), m_zLHC(zLHC), m_yPot(yPot), m_yStat(yStat), m_yBeam(yBeam)
{}
