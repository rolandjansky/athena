/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDSimHit_h
#define AFP_SIDSimHit_h

class AFP_SIDSimHit
{
public:
	AFP_SIDSimHit();
	virtual ~AFP_SIDSimHit();
	bool operator < (const AFP_SIDSimHit& obj) const {return m_nHitID < obj.m_nHitID;}
	int GetTrackID() const { return 22; }

public:
	int m_nHitID; // To identify the hit
	int m_nTrackID;
	int m_nParticleEncoding;         // PDG id 
	float m_fKineticEnergy;          // kin energy of the particle
	float m_fEnergyDeposit;          // energy deposit by the hit
	float m_fPreStepX;
	float m_fPreStepY;
	float m_fPreStepZ;
	float m_fPostStepX;
	float m_fPostStepY;
	float m_fPostStepZ;
	float m_fGlobalTime;

	int m_nStationID;
	int m_nDetectorID;
	bool m_bIsAuxVSID;

      int m_nPixelRow;
      int m_nPixelCol;
};


inline float hitTime(const AFP_SIDSimHit& hit)
{
	return hit.m_fGlobalTime;
}

#endif //AFP_SIDSimHit_h

