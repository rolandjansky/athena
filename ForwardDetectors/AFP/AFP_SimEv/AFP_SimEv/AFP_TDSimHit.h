/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHit_h
#define AFP_TDSimHit_h

class AFP_TDSimHit
{
 public:
        AFP_TDSimHit(const int& nHitID, const int& nTrackID, const int& nParticleEncoding, const float& fKineticEnergy, const float& fEnergyDeposit, const float& fWaveLength, const float& fPreStepX, const float& fPreStepY, const float& fPreStepZ, const float& fPostStepX, const float& fPostStepY, const float& fPostStepZ, const float& fGlobalTime, const int& nStationID, const int& nDetectorID, const int& nSensitiveElementID);
        AFP_TDSimHit();
        virtual ~AFP_TDSimHit();
        bool operator < (const AFP_TDSimHit& obj) const {return m_nHitID < obj.m_nHitID;}

public://FIXME eventually these should be made private and should be accessed via get/set methods.
        int m_nHitID; // To identify the hit
        int m_nTrackID;
        int m_nParticleEncoding;         // PDG id
        float m_fKineticEnergy;          // kin energy of the particle
        float m_fEnergyDeposit;          // energy deposit by the hit
        float m_fWaveLength;             // wavelength of the photon
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
