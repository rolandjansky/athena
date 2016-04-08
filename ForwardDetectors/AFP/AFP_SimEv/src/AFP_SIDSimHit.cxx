/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_SimEv/AFP_SIDSimHit.h"

AFP_SIDSimHit::AFP_SIDSimHit(const int& nHitID, const int& nTrackID, const int& nParticleEncoding, const float& fKineticEnergy, const float& fEnergyDeposit, const float& fPreStepX, const float& fPreStepY, const float& fPreStepZ, const float& fPostStepX, const float& fPostStepY, const float& fPostStepZ, const float& fGlobalTime, const int& nStationID, const int& nDetectorID, const bool& bIsAuxVSID, const int& nPixelRow, const int& nPixelCol)
{
  m_nHitID=          nHitID;
  m_nTrackID=        nTrackID;
  m_nParticleEncoding=nParticleEncoding;
  m_fKineticEnergy= fKineticEnergy;
  m_fEnergyDeposit= fEnergyDeposit;
  m_fPreStepX=      fPreStepX;
  m_fPreStepY=      fPreStepY;
  m_fPreStepZ=      fPreStepZ;
  m_fPostStepX=     fPostStepX;
  m_fPostStepY=     fPostStepY;
  m_fPostStepZ=     fPostStepZ;
  m_fGlobalTime=    fGlobalTime;

  m_nStationID=      nStationID;
  m_nDetectorID=     nDetectorID;

  m_nPixelRow=       nPixelRow;
  m_nPixelCol=       nPixelCol;

  m_bIsAuxVSID=bIsAuxVSID;
}



AFP_SIDSimHit::AFP_SIDSimHit( )
{
        m_nHitID=-1;
        m_nTrackID=-1;
        m_nParticleEncoding=-1;
        m_fKineticEnergy=0.0;
        m_fEnergyDeposit=0.0;
        m_fPreStepX=0.0;
        m_fPreStepY=0.0;
        m_fPreStepZ=0.0;
        m_fPostStepX=0.0;
        m_fPostStepY=0.0;
        m_fPostStepZ=0.0;
        m_fGlobalTime=0.0;

        m_nStationID=-1;
        m_nDetectorID=-1;

        m_nPixelRow=-1;
        m_nPixelCol=-1;

        m_bIsAuxVSID=false;
}

AFP_SIDSimHit::~AFP_SIDSimHit()
{

}
