/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDSimHit_p1_h
#define AFP_SIDSimHit_p1_h

//#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

class AFP_SIDSimHit_p1
{
 public:
	// needed by athenaRoot 
	AFP_SIDSimHit_p1(){};
	friend class AFP_SIDSimHitCnv_p1;

private:
	
        int m_nHitID = 0;
	int m_nTrackID = 0;
	int m_nParticleEncoding = 0;
	float m_fKineticEnergy = 0;
	float m_fEnergyDeposit = 0;
	float m_fPreStepX = 0;
	float m_fPreStepY = 0;
	float m_fPreStepZ = 0;
	float m_fPostStepX = 0;
	float m_fPostStepY = 0;
	float m_fPostStepZ = 0;
	float m_fGlobalTime = 0;

	int m_nStationID = 0;
	int m_nDetectorID = 0;
	
	int m_nPixelRow = 0;
	int m_nPixelCol = 0;
	
        bool m_bIsAuxVSID = 0;

};


#endif //AFP_SIDSimHit_p1_h

