/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"


// Default constructor needed by athenaroot/athenapool
ALFA_LocRecEvent::ALFA_LocRecEvent()
{
	m_iAlgoNum = 0;
	m_fOverU   = 0.0;
	m_fOverV   = 0.0;
	m_iNumU    = 0;
	m_iNumV    = 0;
	m_pot_num  = 0;
	m_x        = 0.0;
	m_y        = 0.0;
}

// destructor
ALFA_LocRecEvent::~ALFA_LocRecEvent() {}

ALFA_LocRecEvent::ALFA_LocRecEvent(int iAlgoNum, int n_pot_num, float x_pos, float y_pos, float fOverU, float fOverV, int iNumU, int iNumV, std::vector<int> iFibSel):
	m_iAlgoNum(iAlgoNum), m_pot_num(n_pot_num), m_x(x_pos), m_y(y_pos), m_fOverU(fOverU), m_fOverV(fOverV), m_iNumU(iNumU), m_iNumV(iNumV), m_iFibSel(iFibSel)
{}
