/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMEVENT_TRTUncompressedHit_h
#define INDETSIMEVENT_TRTUncompressedHit_h

#include "GeneratorObjects/HepMcParticleLink.h"

class TRTUncompressedHit final
{
public:
  TRTUncompressedHit(int hit, int track, int particle,
                     float kinEnergy, float eneDeposit,
                     float preX, float preY, float preZ,
                     float postX, float postY, float postZ,
                     float time);
  TRTUncompressedHit(int hit, const HepMcParticleLink& partLink, int particle,
                     float kinEnergy, float eneDeposit,
                     float preX, float preY, float preZ,
                     float postX, float postY, float postZ,
                     float time);

  // needed by athenaRoot
  TRTUncompressedHit();

  // Destructor:
  ~TRTUncompressedHit() = default;
  
  //copy c'tor defaulted
  TRTUncompressedHit(const TRTUncompressedHit &) = default;
  //assignment defaulted
  TRTUncompressedHit & operator = (const TRTUncompressedHit &) =default;
  //move assignment defaulted
  TRTUncompressedHit & operator = (TRTUncompressedHit &&) = default;

  bool operator < (const TRTUncompressedHit& obj) const
  {return m_hitID < obj.m_hitID;}

  int GetHitID() const
  {return m_hitID;}

  int GetTrackID() const;

  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;


  int GetParticleEncoding() const
  {return m_particleEncoding;}

  float GetKineticEnergy() const
  {return m_kineticEnergy;}

  float GetEnergyDeposit() const
  {return m_energyDeposit;}

  float GetPreStepX() const
  {return m_preStepX;}

  float GetPreStepY() const
  {return m_preStepY;}

  float GetPreStepZ() const
  {return m_preStepZ;}

  float GetPostStepX() const
  {return m_postStepX;}

  float GetPostStepY() const
  {return m_postStepY;}

  float GetPostStepZ() const
  {return m_postStepZ;}

  float GetGlobalTime() const
  {return m_globalTime;}

  void SetGlobalTime(float newGlobalTime)
  {m_globalTime=newGlobalTime;}

private:
  int m_hitID; // To identify the hit
  //  int trackID;
  HepMcParticleLink m_partLink; // link to the particle generating the hit
  int m_particleEncoding;         // PDG id
  float m_kineticEnergy;          // kin energy of the particle
  float m_energyDeposit;          // energy deposit by the hit
  float m_preStepX;
  float m_preStepY;
  float m_preStepZ;
  float m_postStepX;
  float m_postStepY;
  float m_postStepZ;
  float m_globalTime;
};


///////////////////////////////////////////////////////////////////
// open functions:
///////////////////////////////////////////////////////////////////

inline const HepMcParticleLink& TRTUncompressedHit::particleLink() const
{
  return m_partLink;
}


inline float hitTime(const TRTUncompressedHit& hit)
{
  return (float) hit.GetGlobalTime();
}

#endif // INDETSIMEVENT_TRTUncompressedHit_h
