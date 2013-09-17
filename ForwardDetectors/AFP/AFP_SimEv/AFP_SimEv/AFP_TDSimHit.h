/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHit_h
#define AFP_TDSimHit_h

class AFP_TDSimHit
{
 public:
	AFP_TDSimHit();
	virtual ~AFP_TDSimHit();
	bool operator < (const AFP_TDSimHit& obj) const {return m_nHitID < obj.m_nHitID;}
	int GetTrackID() const { return 22; }

public:
	int m_nHitID; // To identify the hit
	int m_nTrackID;
	int m_nParticleEncoding;         // PDG id 
	float m_fKineticEnergy;          // kin energy of the particle
	float m_fEnergyDeposit;          // energy deposit by the hit
	float m_fWaveLength;          	 // wavelength of the photon
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


inline float hitTime(const AFP_TDSimHit& hit)
{
	return hit.m_fGlobalTime;
}

#endif //AFP_TDSimHit_h

