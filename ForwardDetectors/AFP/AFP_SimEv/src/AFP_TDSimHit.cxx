/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_SimEv/AFP_TDSimHit.h"

AFP_TDSimHit::AFP_TDSimHit( )
{
	m_nHitID=-1;
	m_nTrackID=-1;
	m_nParticleEncoding=-1;
	m_fKineticEnergy=0.0;
	m_fEnergyDeposit=0.0;
	m_fWaveLength=0.0;
	m_fPreStepX=0.0;
	m_fPreStepY=0.0;
	m_fPreStepZ=0.0;
	m_fPostStepX=0.0;
	m_fPostStepY=0.0;
	m_fPostStepZ=0.0;
	m_fGlobalTime=0.0;

	m_nStationID=-1;
	m_nDetectorID=-1;
	m_nSensitiveElementID=0;
}

AFP_TDSimHit::~AFP_TDSimHit()
{

}

