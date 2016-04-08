/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHit_p1_h
#define AFP_TDSimHit_p1_h

//#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

class AFP_TDSimHit_p1
{
 public:
	// needed by athenaRoot 
	AFP_TDSimHit_p1(){};
	friend class AFP_TDSimHitCnv_p1;

private:
	int m_nHitID;
	int m_nTrackID;
	int m_nParticleEncoding;
	float m_fKineticEnergy;
	float m_fEnergyDeposit;
	float m_fWaveLength;
	float m_fPreStepX;
	float m_fPreStepY;
	float m_fPreStepZ;
	float m_fPostStepX;
	float m_fPostStepY;
	float m_fPostStepZ;
	float m_fGlobalTime;

	int m_nStationID;
	int m_nDetectorID;
	int m_nSensitiveElementID;
};


#endif //AFP_TDSimHit_p1_h

