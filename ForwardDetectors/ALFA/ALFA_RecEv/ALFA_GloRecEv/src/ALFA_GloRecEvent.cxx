/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_GloRecEv/ALFA_GloRecEvent.h"



// Default constructor needed by athenaroot/athenapool
ALFA_GloRecEvent::ALFA_GloRecEvent()
{
	m_arm             = 0;
	m_x               = 0.0;
	m_y               = 0.0;
	m_x_slope         = 0.0;
	m_y_slope         = 0.0;
	m_near_LocRecCorr = 0;
	m_far_LocRecCorr  = 0;
}

// destructor
ALFA_GloRecEvent::~ALFA_GloRecEvent() {}

// constructor
ALFA_GloRecEvent::ALFA_GloRecEvent(
	int  arm,
	float x_pos,
 	float y_pos,
 	float x_slope,
 	float y_slope,
 	ALFA_LocRecCorrEvent * near_hit,
 	ALFA_LocRecCorrEvent * far_hit
	):m_arm(arm),
	m_x(x_pos),
	m_y(y_pos),
	m_x_slope(x_slope),
	m_y_slope(y_slope),
	m_near_LocRecCorr(near_hit),
	m_far_LocRecCorr(far_hit)
{
}
