/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocRecoEv/AFP_TDLocRecoEvent.h"


// Default constructor needed by athenaroot/athenapool
AFP_TDLocRecoEvent::AFP_TDLocRecoEvent()
{
	m_iAlgoNum    = 0;
	m_nStationID  = 0;
	m_nDetectorID = 0;
	m_nTrainID    = 0;
	m_fTrainTime  = 0.;
	m_nTrainSize  = 0;
	m_nSaturation = 0;
}

// destructor
AFP_TDLocRecoEvent::~AFP_TDLocRecoEvent() {}

AFP_TDLocRecoEvent::AFP_TDLocRecoEvent(int iAlgoNum, int nStationID, int nDetectorID, int nTrainID, float fTrainTime, int nTrainSize, int nSaturation):
	m_iAlgoNum(iAlgoNum), m_nStationID(nStationID), m_nDetectorID(nDetectorID), m_nTrainID(nTrainID), m_fTrainTime(fTrainTime), m_nTrainSize(nTrainSize), m_nSaturation(nSaturation)
{}



