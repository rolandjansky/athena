/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_GloRecEv/ALFA_GloRecODEvent.h"


// Default constructor needed by athenaroot/athenapool
ALFA_GloRecODEvent::ALFA_GloRecODEvent()
{
	m_pot_num = 0;
	m_side    = 0;
	m_y       = 0.0;
	m_y_slope = 0.0;
}

// destructor
ALFA_GloRecODEvent::~ALFA_GloRecODEvent() {}

//ALFA_GloRecODEvent::ALFA_GloRecODEvent(int n_pot_num, int n_side , float y_pos): m_pot_num(n_pot_num), m_side(n_side), m_y(y_pos) {}
ALFA_GloRecODEvent::ALFA_GloRecODEvent(int n_pot_num, int n_side , float y_pos, float y_slope): m_pot_num(n_pot_num), m_side(n_side), m_y(y_pos), m_y_slope(y_slope) {}


