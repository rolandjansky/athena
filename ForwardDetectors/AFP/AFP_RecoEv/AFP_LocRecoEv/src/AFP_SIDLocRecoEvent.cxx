/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocRecoEv/AFP_SIDLocRecoEvent.h"


// Default constructor needed by athenaroot/athenapool
AFP_SIDLocRecoEvent::AFP_SIDLocRecoEvent()
{
	m_iAlgoNum   = 0;
	m_nStationID = 0;
	
	m_x          = 0.0;
	m_y          = 0.0;
	m_z          = 0.0;
	m_x_slope    = 0.0;
	m_y_slope    = 0.0;
	m_z_slope    = 0.0;
	
	m_nHits      = 0;
	m_nHoles     = 0;
	m_fChi2      = 0.0;	
}

// destructor
AFP_SIDLocRecoEvent::~AFP_SIDLocRecoEvent() {}

AFP_SIDLocRecoEvent::AFP_SIDLocRecoEvent(int iAlgoNum, int nStationID, float x_pos, float y_pos, float z_pos, float x_slope, float y_slope, float z_slope, int nHits, int nHoles, float fChi2):
	m_iAlgoNum(iAlgoNum), m_nStationID(nStationID), m_x(x_pos), m_y(y_pos), m_z(z_pos), m_x_slope(x_slope), m_y_slope(y_slope), m_z_slope(z_slope), m_nHits(nHits), m_nHoles(nHoles), m_fChi2(fChi2)
{}
