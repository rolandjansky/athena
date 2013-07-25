/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecEv/ALFA_LocRecODEvent.h"


// Default constructor needed by athenaroot/athenapool
ALFA_LocRecODEvent::ALFA_LocRecODEvent()
{
	m_iAlgoNum = 0;
	m_fOverY   = 0.0;
	m_iNumY    = 0;
	m_pot_num  = 0;
	m_side     = 0;
	m_y        = 0.0;
}

// destructor
ALFA_LocRecODEvent::~ALFA_LocRecODEvent() {}

ALFA_LocRecODEvent::ALFA_LocRecODEvent(int iAlgoNum, int n_pot_num, int n_side , float y_pos, float fOverY, int iNumY, std::vector<int> iFibSel):
	m_iAlgoNum(iAlgoNum), m_pot_num(n_pot_num), m_side(n_side), m_y(y_pos), m_fOverY(fOverY), m_iNumY(iNumY), m_iFibSel(iFibSel)
{}



